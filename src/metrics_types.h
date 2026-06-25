// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include "metrics_config.h"

#include <cstdint>
#include <limits>

namespace BlackTek::Metrics
{
    [[nodiscard]] inline int16_t ClampToInt16(int64_t value) noexcept
    {
        if (value < std::numeric_limits<int16_t>::min())
            return std::numeric_limits<int16_t>::min();
        if (value > std::numeric_limits<int16_t>::max())
            return std::numeric_limits<int16_t>::max();
        return static_cast<int16_t>(value);
    }

    enum class ConditionRecordType : uint8_t
    {
        Application,
        Tick,
        Expiry,
        Cancelled
    };

    enum class StrikeFlag : uint8_t
    {
        WasCritical    = 1 << 0,
        WasFatal       = 1 << 1,
        HadAugmentMods = 1 << 2,
        HadBounce      = 1 << 3,
        WasBatched     = 1 << 4, // AoE batch path; pipeline-stage fields are 0
    };

    // Captures the exact formula configuration that governed one strike. Per-combat
    // overrides and Lua-compiled stages mean two strikes from two different spells can
    // use entirely different formulas even though combat.toml looks uniform.
    struct FormulaSnapshot
    {
        uint8_t output_formula     = 0;
        uint8_t defense_formula    = 0;
        uint8_t armor_formula      = 0;
        uint8_t resolution_formula = 0;

        uint8_t output_preset     = 0;
        uint8_t defense_preset    = 0;
        uint8_t armor_preset      = 0;
        uint8_t resolution_preset = 0;

        uint8_t bind_key    = 0xFF; // NO_BIND_KEY sentinel when formula bind is unavailable
        uint8_t bind_source = 0;

        bool has_per_combat_override = false;
        bool has_lua_override        = false;
    };

    // Capture buffer for one strike's pipeline data. Stored as a static on Combat
    // (not an instance member) so sizeof(Combat) is unaffected regardless of ENABLED.
    // Fields are always present so MSVC can type-check all if constexpr branches;
    // dead writes when ENABLED=false are eliminated by the Release optimizer.
    struct StrikeCapture
    {
        uint32_t raw_output     = 0;
        uint32_t defense_value  = 0;
        uint32_t armor_value    = 0;
        uint32_t post_defense   = 0;
        uint32_t post_armor     = 0;
        uint32_t modifier_procs = 0;
        uint8_t  block_type     = 0;
        uint8_t  flags          = 0;
        FormulaSnapshot formula{};
    };

    struct StrikeRecord
    {
        uint64_t timestamp_ms = 0;
        uint32_t caster_id    = 0;
        uint32_t target_id    = 0;
        int64_t  combat_id    = -1;

        uint8_t  situation    = 0;
        uint8_t  origin       = 0;
        uint16_t damage_type  = 0;

        uint8_t  caster_vocation = 0;
        uint8_t  target_vocation = 0;
        uint16_t caster_level    = 0;
        uint16_t target_level    = 0;

        FormulaSnapshot formula{};

        uint32_t raw_output    = 0;
        uint32_t defense_value = 0;
        uint32_t armor_value   = 0;
        uint32_t post_defense  = 0;
        uint32_t post_armor    = 0;

        int32_t modifier_delta = 0;

        uint32_t final_damage = 0;
        uint32_t mana_drained = 0;
        uint8_t  block_type   = 0;

        uint8_t flags = 0;

        uint32_t leech_health  = 0;
        uint32_t leech_mana    = 0;
        uint32_t leech_stamina = 0;
        uint32_t leech_soul    = 0;
    };

    struct ModifierRecord
    {
        uint64_t timestamp_ms = 0;
        uint32_t caster_id    = 0;
        uint32_t target_id    = 0;

        uint64_t modifier_guid = 0; // 0 when more than one modifier contributed

        uint8_t  stance      = 0;
        uint8_t  mod_type    = 0;
        uint16_t damage_type = 0;

        uint32_t damage_before    = 0;
        uint32_t damage_after     = 0;
        uint32_t secondary_amount = 0;
    };

    struct HealRecord
    {
        uint64_t timestamp_ms = 0;
        uint32_t caster_id    = 0;
        uint32_t target_id    = 0;
        int64_t  combat_id    = -1;

        uint8_t  origin      = 0;
        uint8_t  stat_target = 0; // 0=HP, 1=Mana, 2=Soul, 3=Stamina

        uint32_t amount_requested = 0;
        uint32_t amount_healed    = 0;

        uint8_t caster_vocation = 0;
        uint8_t target_vocation = 0;
        uint8_t situation       = 0;
    };

    // Application + tick records share instance_guid for correlation.
    struct ConditionRecord
    {
        uint64_t timestamp_ms = 0;

        uint64_t instance_guid    = 0;
        uint32_t applier_id       = 0;
        uint32_t target_id        = 0;
        int64_t  source_combat_id = -1;

        uint8_t record_type = 0; // ConditionRecordType

        uint8_t  condition_type = 0;
        uint16_t combat_type    = 0;

        uint16_t expected_ticks     = 0;
        uint32_t per_tick_amount    = 0;
        uint32_t total_expected_dmg = 0;

        uint16_t tick_number   = 0;
        uint32_t tick_damage   = 0;
        uint32_t running_total = 0;

        bool was_fatal = false;

        uint8_t applier_vocation = 0;
        uint8_t target_vocation  = 0;
        uint8_t situation        = 0;
    };

    // Grouping dimensions for the strike aggregate map. Packs into a uint64_t key via
    // Pack() so it can be used directly as a gtl::flat_hash_map key without a custom
    // hash/equality specialization.
    struct AggregateKey
    {
        uint8_t  situation         = 0;
        uint8_t  attacker_vocation = 0;
        uint8_t  defender_vocation = 0;
        uint8_t  origin            = 0;
        uint16_t damage_type       = 0;
        int16_t  combat_id_bucket  = 0; // combat_id clamped to int16 range; melee = -1

        [[nodiscard]] uint64_t Pack() const noexcept
        {
            return (static_cast<uint64_t>(situation)         << 56)
                 | (static_cast<uint64_t>(attacker_vocation)  << 48)
                 | (static_cast<uint64_t>(defender_vocation)  << 40)
                 | (static_cast<uint64_t>(origin)             << 32)
                 | (static_cast<uint64_t>(damage_type)        << 16)
                 |  static_cast<uint64_t>(static_cast<uint16_t>(combat_id_bucket));
        }
    };

    // Running aggregate for one AggregateKey. Carries the originating key fields
    // alongside the metrics so a query can read a row without re-deriving them.
    struct AggregateData
    {
        AggregateKey key{};

        uint64_t total_strikes  = 0;
        uint64_t total_blocked  = 0;
        uint64_t total_critical = 0;
        uint64_t total_fatal    = 0;

        uint64_t sum_raw_output  = 0;
        uint64_t sum_defense_abs = 0;
        uint64_t sum_armor_abs   = 0;
        int64_t  sum_modifier_abs = 0; // negative = weakness/augments net-increased damage
        uint64_t sum_final_damage = 0;

        uint32_t min_damage = 0xFFFFFFFFu;
        uint32_t max_damage = 0;

        uint64_t sum_leech_health = 0;
        uint64_t sum_leech_mana   = 0;

        uint32_t modifier_procs = 0;
    };

    struct ModifierAggregateKey
    {
        uint8_t stance   = 0;
        uint8_t mod_type = 0;

        [[nodiscard]] uint16_t Pack() const noexcept
        {
            return (static_cast<uint16_t>(stance) << 8) | static_cast<uint16_t>(mod_type);
        }
    };

    struct ModifierAggregateData
    {
        ModifierAggregateKey key{};

        uint64_t total_procs          = 0;
        uint64_t sum_damage_before    = 0;
        uint64_t sum_damage_after     = 0;
        uint64_t sum_secondary_amount = 0;
    };

    // Grouping dimensions for the heal aggregate map.
    struct HealAggregateKey
    {
        uint8_t situation       = 0;
        uint8_t caster_vocation = 0;
        uint8_t target_vocation = 0;
        uint8_t origin          = 0;
        uint8_t stat_target     = 0;

        [[nodiscard]] uint64_t Pack() const noexcept
        {
            return (static_cast<uint64_t>(situation)       << 32)
                 | (static_cast<uint64_t>(caster_vocation)  << 24)
                 | (static_cast<uint64_t>(target_vocation)  << 16)
                 | (static_cast<uint64_t>(origin)           << 8)
                 |  static_cast<uint64_t>(stat_target);
        }
    };

    struct HealAggregateData
    {
        HealAggregateKey key{};

        uint64_t total_heals          = 0;
        uint64_t sum_amount_requested = 0;
        uint64_t sum_amount_healed    = 0;

        uint32_t min_heal = 0xFFFFFFFFu;
        uint32_t max_heal = 0;
    };

    // Formula configuration observed for one distinct combat_id. FormulaUsageMap is
    // keyed directly by combat_id (int64_t), no packing needed.
    struct FormulaSignature
    {
        uint8_t output_formula     = 0;
        uint8_t defense_formula    = 0;
        uint8_t armor_formula      = 0;
        uint8_t resolution_formula = 0;

        uint8_t output_preset     = 0;
        uint8_t defense_preset    = 0;
        uint8_t armor_preset      = 0;
        uint8_t resolution_preset = 0;

        uint8_t bind_key    = 0xFF;
        uint8_t bind_source = 0;

        bool has_per_combat_override = false;
        bool has_lua_override        = false;

        uint64_t first_seen_ms       = 0;
        uint64_t total_strikes       = 0;
        uint32_t inconsistency_count = 0;
    };

    // Grouping dimensions for the condition aggregate map.
    struct ConditionAggregateKey
    {
        uint8_t  situation        = 0;
        uint8_t  applier_vocation = 0;
        uint8_t  target_vocation  = 0;
        uint8_t  condition_type   = 0;
        uint16_t combat_type      = 0;
        int16_t  source_combat_id = 0; // clamped to int16 range

        [[nodiscard]] uint64_t Pack() const noexcept
        {
            return (static_cast<uint64_t>(situation)        << 56)
                 | (static_cast<uint64_t>(applier_vocation)  << 48)
                 | (static_cast<uint64_t>(target_vocation)   << 40)
                 | (static_cast<uint64_t>(condition_type)    << 32)
                 | (static_cast<uint64_t>(combat_type)       << 16)
                 |  static_cast<uint64_t>(static_cast<uint16_t>(source_combat_id));
        }
    };

    struct ConditionAggregateData
    {
        ConditionAggregateKey key{};

        uint64_t total_applications = 0;
        uint64_t total_ticks        = 0;
        uint64_t total_damage       = 0;
        uint64_t total_fatal_ticks  = 0;

        uint64_t total_completed = 0;
        uint64_t total_cancelled = 0;

        uint32_t min_tick_damage = 0xFFFFFFFFu;
        uint32_t max_tick_damage = 0;

        uint64_t sum_expected_dmg = 0;
    };
}
