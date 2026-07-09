// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"
#include "metrics_store.h"

#include <algorithm>
#include <limits>

namespace BlackTek::Metrics
{
    AggregateStore g_metrics_store;

    void AggregateStore::RecordStrike(const StrikeRecord& record, uint32_t modifier_proc_count) noexcept
    {
        strike_ring_.Push(record);

        AggregateKey key{};
        key.situation         = record.situation;
        key.attacker_vocation = record.caster_vocation;
        key.defender_vocation = record.target_vocation;
        key.origin            = record.origin;
        key.damage_type       = record.damage_type;
        key.combat_id_bucket  = ClampToInt16(record.combat_id);

        auto& data = strike_aggregates_[key.Pack()];
        data.key = key;

        ++data.total_strikes;

        const bool blocked  = record.block_type != 0; // BlockType::NoBlock == 0
        const bool critical = (record.flags & static_cast<uint8_t>(StrikeFlag::WasCritical)) != 0;
        const bool fatal    = (record.flags & static_cast<uint8_t>(StrikeFlag::WasFatal)) != 0;
        const bool batched  = (record.flags & static_cast<uint8_t>(StrikeFlag::WasBatched)) != 0;

        if (blocked)
            ++data.total_blocked;
        if (critical)
            ++data.total_critical;
        if (fatal)
            ++data.total_fatal;

        if (not batched)
        {
            data.sum_raw_output  += record.raw_output;
            data.sum_defense_abs += (record.raw_output > record.post_defense) ? (record.raw_output - record.post_defense) : 0;
            data.sum_armor_abs   += (record.post_defense > record.post_armor) ? (record.post_defense - record.post_armor) : 0;
            data.sum_modifier_abs += static_cast<int64_t>(record.post_armor) - static_cast<int64_t>(record.final_damage);
        }

        data.sum_final_damage += record.final_damage;
        data.min_damage = std::min(data.min_damage, record.final_damage);
        data.max_damage = std::max(data.max_damage, record.final_damage);

        data.sum_leech_health += record.leech_health;
        data.sum_leech_mana   += record.leech_mana;

        data.modifier_procs += modifier_proc_count;
    }

    void AggregateStore::RecordModifier(const ModifierRecord& record) noexcept
    {
        modifier_ring_.Push(record);

        ModifierAggregateKey key{};
        key.stance   = record.stance;
        key.mod_type = record.mod_type;

        auto& data = modifier_aggregates_[key.Pack()];
        data.key = key;

        ++data.total_procs;
        data.sum_damage_before    += record.damage_before;
        data.sum_damage_after     += record.damage_after;
        data.sum_secondary_amount += record.secondary_amount;
    }

    void AggregateStore::RecordHeal(const HealRecord& record) noexcept
    {
        heal_ring_.Push(record);

        HealAggregateKey key{};
        key.situation       = record.situation;
        key.caster_vocation = record.caster_vocation;
        key.target_vocation = record.target_vocation;
        key.origin          = record.origin;
        key.stat_target      = record.stat_target;

        auto& data = heal_aggregates_[key.Pack()];
        data.key = key;

        ++data.total_heals;
        data.sum_amount_requested += record.amount_requested;
        data.sum_amount_healed    += record.amount_healed;
        data.min_heal = std::min(data.min_heal, record.amount_healed);
        data.max_heal = std::max(data.max_heal, record.amount_healed);
    }

    void AggregateStore::RecordCondition(const ConditionRecord& record) noexcept
    {
        condition_ring_.Push(record);

        ConditionAggregateKey key{};
        key.situation        = record.situation;
        key.applier_vocation = record.applier_vocation;
        key.target_vocation  = record.target_vocation;
        key.condition_type   = record.condition_type;
        key.combat_type      = record.combat_type;
        key.source_combat_id = ClampToInt16(record.source_combat_id);

        auto& data = condition_aggregates_[key.Pack()];
        data.key = key;

        const auto type = static_cast<ConditionRecordType>(record.record_type);

        switch (type)
        {
            case ConditionRecordType::Application:
                ++data.total_applications;
                data.sum_expected_dmg += record.total_expected_dmg;
                break;

            case ConditionRecordType::Tick:
                ++data.total_ticks;
                data.total_damage += record.tick_damage;
                data.min_tick_damage = std::min(data.min_tick_damage, record.tick_damage);
                data.max_tick_damage = std::max(data.max_tick_damage, record.tick_damage);
                if (record.was_fatal)
                    ++data.total_fatal_ticks;
                break;

            case ConditionRecordType::Expiry:
                ++data.total_completed;
                break;

            case ConditionRecordType::Cancelled:
                ++data.total_cancelled;
                break;
        }
    }

    bool AggregateStore::UpdateFormulaUsage(int64_t combat_id, const FormulaSnapshot& snapshot, uint64_t timestamp_ms) noexcept
    {
        auto it = formula_usage_map_.find(combat_id);

        if (it == formula_usage_map_.end())
        {
            FormulaSignature signature{};
            signature.output_formula     = snapshot.output_formula;
            signature.defense_formula    = snapshot.defense_formula;
            signature.armor_formula      = snapshot.armor_formula;
            signature.resolution_formula = snapshot.resolution_formula;
            signature.output_preset      = snapshot.output_preset;
            signature.defense_preset     = snapshot.defense_preset;
            signature.armor_preset       = snapshot.armor_preset;
            signature.resolution_preset  = snapshot.resolution_preset;
            signature.bind_key    = snapshot.bind_key;
            signature.bind_source = snapshot.bind_source;
            signature.has_per_combat_override = snapshot.has_per_combat_override;
            signature.has_lua_override        = snapshot.has_lua_override;
            signature.first_seen_ms = timestamp_ms;
            signature.total_strikes = 1;

            formula_usage_map_.emplace(combat_id, signature);
            return false;
        }

        FormulaSignature& signature = it->second;
        ++signature.total_strikes;

        const bool matches =
            signature.output_formula     == snapshot.output_formula and
            signature.defense_formula    == snapshot.defense_formula and
            signature.armor_formula      == snapshot.armor_formula and
            signature.resolution_formula == snapshot.resolution_formula and
            signature.output_preset      == snapshot.output_preset and
            signature.defense_preset     == snapshot.defense_preset and
            signature.armor_preset       == snapshot.armor_preset and
            signature.resolution_preset  == snapshot.resolution_preset and
            signature.bind_key           == snapshot.bind_key and
            signature.bind_source        == snapshot.bind_source;

        if (not matches)
        {
            ++signature.inconsistency_count;
            return true;
        }

        return false;
    }

    std::optional<FormulaSignature> AggregateStore::GetFormulaSignature(int64_t combat_id) const
    {
        const auto it = formula_usage_map_.find(combat_id);
        if (it == formula_usage_map_.end())
            return std::nullopt;
        return it->second;
    }

    std::vector<AggregateData> AggregateStore::SnapshotStrikeAggregates() const
    {
        std::vector<AggregateData> aggregates;
        aggregates.reserve(strike_aggregates_.size());
        for (const auto& [packed_key, data] : strike_aggregates_)
            aggregates.push_back(data);
        return aggregates;
    }

    std::vector<HealAggregateData> AggregateStore::SnapshotHealAggregates() const
    {
        std::vector<HealAggregateData> aggregates;
        aggregates.reserve(heal_aggregates_.size());
        for (const auto& [packed_key, data] : heal_aggregates_)
            aggregates.push_back(data);
        return aggregates;
    }

    std::vector<ConditionAggregateData> AggregateStore::SnapshotConditionAggregates() const
    {
        std::vector<ConditionAggregateData> aggregates;
        aggregates.reserve(condition_aggregates_.size());
        for (const auto& [packed_key, data] : condition_aggregates_)
            aggregates.push_back(data);
        return aggregates;
    }

    std::vector<ModifierAggregateData> AggregateStore::SnapshotModifierAggregates() const
    {
        std::vector<ModifierAggregateData> aggregates;
        aggregates.reserve(modifier_aggregates_.size());
        for (const auto& [packed_key, data] : modifier_aggregates_)
            aggregates.push_back(data);
        return aggregates;
    }

    std::vector<FormulaSignature> AggregateStore::SnapshotFormulaUsage() const
    {
        std::vector<FormulaSignature> aggregates;
        aggregates.reserve(formula_usage_map_.size());
        for (const auto& [id, signature] : formula_usage_map_)
            aggregates.push_back(signature);
        return aggregates;
    }

    void AggregateStore::Reset(ResetScope scope) noexcept
    {
        switch (scope)
        {
            case ResetScope::Session:
                strike_aggregates_.clear();
                heal_aggregates_.clear();
                condition_aggregates_.clear();
                modifier_aggregates_.clear();
                break;

            case ResetScope::Window:
                strike_ring_.Reset();
                modifier_ring_.Reset();
                heal_ring_.Reset();
                condition_ring_.Reset();
                break;

            case ResetScope::Formulas:
                formula_usage_map_.clear();
                break;

            case ResetScope::All:
                Reset(ResetScope::Session);
                Reset(ResetScope::Window);
                Reset(ResetScope::Formulas);
                break;
        }
    }
}
