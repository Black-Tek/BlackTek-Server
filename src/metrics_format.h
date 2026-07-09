// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include "metrics_types.h"
#include "enums.h"
#include "tools.h"

#include <string>
#include <string_view>
#include <fmt/format.h>

namespace BlackTek::Metrics
{
    [[nodiscard]] inline std::string_view PresetName(uint8_t preset_index) noexcept
    {
        switch (preset_index)
        {
            case 0: return "Classic";
            case 1: return "Scaled";
            case 2: return "Balanced";
            case 3: return "Absorption";
            case 4: return "Tabletop";
            case 5: return "Exponential";
            case 6: return "Proportional";
            case 7: return "Gradual";
            default: return "Custom";
        }
    }

    [[nodiscard]] inline std::string_view OutputFormulaName(uint8_t value) noexcept
    {
        switch (value)
        {
            case 0: return "Flat";
            case 1: return "Linear";
            case 2: return "LinearRange";
            case 3: return "Power";
            default: return "Unknown";
        }
    }

    [[nodiscard]] inline std::string_view ResistanceFormulaName(uint8_t value) noexcept
    {
        switch (value)
        {
            case 0: return "Identity";
            case 1: return "LinearRandom";
            case 2: return "Parity";
            case 3: return "Percent";
            default: return "Unknown";
        }
    }

    [[nodiscard]] inline std::string_view ResolutionFormulaName(uint8_t value) noexcept
    {
        switch (value)
        {
            case 0: return "Subtractive";
            case 1: return "RatioMitigation";
            case 2: return "ScaledDivision";
            case 3: return "Layered";
            default: return "Unknown";
        }
    }

    [[nodiscard]] inline std::string_view BindKeyName(uint8_t value) noexcept
    {
        switch (value)
        {
            case 0:  return "Level";
            case 1:  return "MagicLevel";
            case 2:  return "SkillLevel";
            case 3:  return "Attack";
            case 4:  return "Defense";
            case 5:  return "WeaponAttack";
            case 6:  return "WeaponDefense";
            case 7:  return "WeaponSkill";
            case 8:  return "Health";
            case 9:  return "MaxHealth";
            case 10: return "Mana";
            case 11: return "MaxMana";
            case 12: return "Soul";
            case 13: return "MaxSoul";
            case 14: return "Stamina";
            default: return "None";
        }
    }

    [[nodiscard]] inline std::string_view BindSourceName(uint8_t value) noexcept
    {
        return value == 1 ? "Target" : "Caster";
    }

    [[nodiscard]] inline std::string_view SituationName(uint8_t bitmask) noexcept
    {
        switch (bitmask)
        {
            case 1: return "pvp";
            case 2: return "pvm";
            case 4: return "mvp";
            case 8: return "mvm";
            default: return "unknown";
        }
    }

    [[nodiscard]] inline std::string_view OriginName(uint8_t value) noexcept
    {
        switch (value)
        {
            case 0:  return "None";
            case 1:  return "Condition";
            case 2:  return "Spell";
            case 3:  return "Melee";
            case 4:  return "Ranged";
            case 5:  return "Fist";
            case 6:  return "Sword";
            case 7:  return "Axe";
            case 8:  return "Club";
            case 9:  return "Wand";
            case 10: return "Rod";
            case 11: return "Bow";
            case 12: return "Crossbow";
            case 13: return "Throwable";
            case 14: return "Augment";
            case 15: return "Absorb";
            case 16: return "Restore";
            case 17: return "Replenish";
            case 18: return "Revive";
            case 19: return "Reflect";
            case 20: return "Deflect";
            case 21: return "Ricochet";
            case 22: return "Piercing";
            case 23: return "LifeSteal";
            case 24: return "ManaSteal";
            case 25: return "StaminaSteal";
            case 26: return "SoulSteal";
            default: return "Unknown";
        }
    }

    [[nodiscard]] inline std::string_view BlockTypeName(uint8_t value) noexcept
    {
        switch (value)
        {
            case 0: return "NoBlock";
            case 1: return "Defensive";
            case 2: return "Armor";
            case 3: return "Immunity";
            case 4: return "Dodge";
            default: return "Unknown";
        }
    }

    [[nodiscard]] inline std::string_view ModTypeName(uint8_t stance, uint8_t mod_type) noexcept
    {
        if (stance == 1) // DamageModifier::Stance::Defense
        {
            switch (mod_type)
            {
                case 1: return "Absorb";
                case 2: return "Restore";
                case 3: return "Replenish";
                case 4: return "Revive";
                case 5: return "Reflect";
                case 6: return "Deflect";
                case 7: return "Ricochet";
                case 8: return "Resist";
                case 9: return "Reform";
                case 10: return "Weakness";
                default: return "None";
            }
        }

        switch (mod_type) // DamageModifier::Stance::Attack
        {
            case 1: return "Lifesteal";
            case 2: return "Manasteal";
            case 3: return "Staminasteal";
            case 4: return "Soulsteal";
            case 5: return "Critical";
            case 6: return "Piercing";
            case 7: return "Conversion";
            case 8: return "Regeneration";
            case 9: return "Attunement";
            case 10: return "Vigor";
            case 11: return "Transcendence";
            default: return "None";
        }
    }

    [[nodiscard]] inline std::string_view ConditionTypeName(uint8_t value) noexcept
    {
        switch (static_cast<ConditionType_t>(value))
        {
            case CONDITION_FIRE:     return "Fire";
            case CONDITION_ENERGY:   return "Energy";
            case CONDITION_POISON:   return "Poison";
            case CONDITION_DROWN:    return "Drown";
            case CONDITION_FREEZING: return "Freezing";
            case CONDITION_DAZZLED:  return "Dazzled";
            case CONDITION_CURSED:   return "Cursed";
            case CONDITION_BLEEDING: return "Bleeding";
            default:                 return "Other";
        }
    }

    [[nodiscard]] inline std::string JsonEscape(std::string_view text)
    {
        std::string escaped;
        escaped.reserve(text.size());
        for (char character : text)
        {
            switch (character)
            {
                case '"':  escaped += "\\\""; break;
                case '\\': escaped += "\\\\"; break;
                case '\n': escaped += "\\n";  break;
                case '\r': escaped += "\\r";  break;
                case '\t': escaped += "\\t";  break;
                default:   escaped += character; break;
            }
        }
        return escaped;
    }

    [[nodiscard]] inline std::string FormatStrikeCsv(const StrikeRecord& record, std::string_view caster_name, std::string_view target_name)
    {
        return fmt::format(
            "{},{},{},{},{},{},{},{},{},{},{},{},{},"
            "{},{},{},{},{},{},{},{},{},{},{},{},"
            "{},{},{},{},{},{},{},{},{},"
            "{},{},{},{},"
            "{},{},{},{}",
            record.timestamp_ms, record.combat_id, record.caster_id, caster_name, record.caster_vocation, record.caster_level,
            record.target_id, target_name, record.target_vocation, record.target_level, SituationName(record.situation), OriginName(record.origin), record.damage_type,
            PresetName(record.formula.output_preset), PresetName(record.formula.defense_preset), PresetName(record.formula.armor_preset), PresetName(record.formula.resolution_preset),
            OutputFormulaName(record.formula.output_formula), ResistanceFormulaName(record.formula.defense_formula), ResistanceFormulaName(record.formula.armor_formula), ResolutionFormulaName(record.formula.resolution_formula),
            BindKeyName(record.formula.bind_key), BindSourceName(record.formula.bind_source), record.formula.has_per_combat_override, record.formula.has_lua_override,
            record.raw_output, record.defense_value, record.armor_value, record.post_defense, record.post_armor,
            record.modifier_delta, record.final_damage, record.mana_drained, BlockTypeName(record.block_type),
            (record.flags & static_cast<uint8_t>(StrikeFlag::WasCritical)) != 0,
            (record.flags & static_cast<uint8_t>(StrikeFlag::WasFatal)) != 0,
            (record.flags & static_cast<uint8_t>(StrikeFlag::HadAugmentMods)) != 0,
            (record.flags & static_cast<uint8_t>(StrikeFlag::HadBounce)) != 0,
            record.leech_health, record.leech_mana, record.leech_stamina, record.leech_soul);
    }

    [[nodiscard]] inline std::string FormatModifierCsv(const ModifierRecord& record)
    {
        return fmt::format("{},{},{},{},{},{},{},{},{},{}",
            record.timestamp_ms, record.caster_id, record.target_id, record.modifier_guid, record.stance == 1 ? "Defense" : "Attack",
            ModTypeName(record.stance, record.mod_type), record.damage_type, record.damage_before, record.damage_after, record.secondary_amount);
    }

    [[nodiscard]] inline std::string FormatHealCsv(const HealRecord& record)
    {
        return fmt::format("{},{},{},{},{},{},{},{},{},{},{}",
            record.timestamp_ms, record.combat_id, record.caster_id, record.target_id, OriginName(record.origin), record.stat_target,
            record.amount_requested, record.amount_healed, record.caster_vocation, record.target_vocation, SituationName(record.situation));
    }

    [[nodiscard]] inline std::string FormatConditionCsv(const ConditionRecord& record)
    {
        return fmt::format("{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}",
            record.timestamp_ms, record.instance_guid, record.applier_id, record.target_id, record.source_combat_id,
            static_cast<int>(record.record_type), ConditionTypeName(record.condition_type), record.combat_type,
            record.expected_ticks, record.per_tick_amount, record.total_expected_dmg,
            record.tick_number, record.tick_damage, record.running_total,
            record.was_fatal, record.applier_vocation, record.target_vocation, SituationName(record.situation));
    }

    [[nodiscard]] inline std::string FormatAnomalyCsv(int64_t combat_id, uint64_t strike_count_at_detection, const FormulaSignature& expected, const FormulaSnapshot& observed)
    {
        return fmt::format("{},{},{},{},{},{},{},{},{},{}",
            OTSYS_TIME(), combat_id, strike_count_at_detection,
            PresetName(expected.output_preset), PresetName(expected.resolution_preset), BindKeyName(expected.bind_key),
            PresetName(observed.output_preset), PresetName(observed.resolution_preset), BindKeyName(observed.bind_key),
            observed.has_per_combat_override or observed.has_lua_override);
    }

    [[nodiscard]] inline std::string FormatStrikeJsonl(const StrikeRecord& record, std::string_view caster_name, std::string_view target_name)
    {
        return fmt::format(
            "{{\"timestamp_ms\":{},\"combat_id\":{},\"caster_id\":{},\"caster_name\":\"{}\",\"caster_vocation\":{},\"caster_level\":{},"
            "\"target_id\":{},\"target_name\":\"{}\",\"target_vocation\":{},\"target_level\":{},\"situation\":\"{}\",\"origin\":\"{}\",\"damage_type\":{},"
            "\"output_preset\":\"{}\",\"defense_preset\":\"{}\",\"armor_preset\":\"{}\",\"resolution_preset\":\"{}\","
            "\"bind_key\":\"{}\",\"bind_source\":\"{}\",\"has_override\":{},\"has_lua\":{},"
            "\"raw_output\":{},\"defense_value\":{},\"armor_value\":{},\"post_defense\":{},\"post_armor\":{},"
            "\"modifier_delta\":{},\"final_damage\":{},\"mana_drained\":{},\"block_type\":\"{}\","
            "\"was_critical\":{},\"was_fatal\":{},\"had_augment_mods\":{},\"had_bounce\":{},\"was_batched\":{},"
            "\"leech_health\":{},\"leech_mana\":{},\"leech_stamina\":{},\"leech_soul\":{}}}",
            record.timestamp_ms, record.combat_id, record.caster_id, JsonEscape(caster_name), record.caster_vocation, record.caster_level,
            record.target_id, JsonEscape(target_name), record.target_vocation, record.target_level, SituationName(record.situation), OriginName(record.origin), record.damage_type,
            PresetName(record.formula.output_preset), PresetName(record.formula.defense_preset), PresetName(record.formula.armor_preset), PresetName(record.formula.resolution_preset),
            BindKeyName(record.formula.bind_key), BindSourceName(record.formula.bind_source), record.formula.has_per_combat_override, record.formula.has_lua_override,
            record.raw_output, record.defense_value, record.armor_value, record.post_defense, record.post_armor,
            record.modifier_delta, record.final_damage, record.mana_drained, BlockTypeName(record.block_type),
            (record.flags & static_cast<uint8_t>(StrikeFlag::WasCritical)) != 0,
            (record.flags & static_cast<uint8_t>(StrikeFlag::WasFatal)) != 0,
            (record.flags & static_cast<uint8_t>(StrikeFlag::HadAugmentMods)) != 0,
            (record.flags & static_cast<uint8_t>(StrikeFlag::HadBounce)) != 0,
            (record.flags & static_cast<uint8_t>(StrikeFlag::WasBatched)) != 0,
            record.leech_health, record.leech_mana, record.leech_stamina, record.leech_soul);
    }
}
