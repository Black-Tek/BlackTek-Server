// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include "metrics_config.h"
#include "metrics_types.h"
#include "metrics_store.h"

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace BlackTek::Metrics
{
    struct LiveLogConfig
    {
        bool enabled = true;
        std::string directory = "logs/combat/metrics";
        std::string format = "csv";
        uint32_t max_file_mb = 100;
        bool daily_rotation = true;
    };

    struct RollingDumpConfig
    {
        bool enabled = true;
        double interval_hours = 4.0;
        std::string directory = "logs/combat/metrics/dumps";
        std::string format = "json"; // "json" | "csv"
        bool reset_after_dump = false;
        uint32_t keep_dump_count = 24;
    };

    struct RuntimeConfig
    {
        bool enabled            = true;
        bool capture_pipeline   = true;
        bool capture_formulas   = true;
        bool capture_modifiers  = true;
        bool capture_healing    = true;
        bool capture_conditions = true;

        LiveLogConfig live_log{};
        RollingDumpConfig rolling_dump{};
    };

    extern RuntimeConfig g_metrics_runtime;

    void Initialize();
    void RecordStrike(const StrikeRecord& record, uint32_t modifier_proc_count, std::string_view caster_name, std::string_view target_name);
    void RecordModifier(const ModifierRecord& record);
    void RecordHeal(const HealRecord& record);
    void RecordCondition(const ConditionRecord& record);
    void RecordFormulaUsage(int64_t combat_id, const FormulaSnapshot& snapshot, uint64_t timestamp_ms);
    void RecordFormulaBind(int64_t combat_id, uint8_t situation_index, uint8_t bind_key, uint8_t bind_source);
    [[nodiscard]] std::optional<std::pair<uint8_t, uint8_t>> LookupFormulaBind(int64_t combat_id, uint8_t situation_index) noexcept;

    struct ConditionSource
    {
        uint32_t applier_id = 0;
        int64_t source_combat_id = -1;
        uint64_t instance_guid = 0;
        uint32_t running_total = 0;
        uint16_t tick_count = 0;
    };

    void ClearConditionSource(uint32_t target_id, uint8_t condition_type) noexcept;
    void RegisterConditionSource(uint32_t target_id, uint8_t condition_type, const ConditionSource& source);
    [[nodiscard]] std::optional<ConditionSource> LookupConditionSource(uint32_t target_id, uint8_t condition_type) noexcept;
    [[nodiscard]] std::optional<ConditionSource> AccumulateConditionTick(uint32_t target_id, uint8_t condition_type, uint32_t tick_damage) noexcept;

    enum class QuerySource : uint8_t { Session, Window };

    struct StrikeFilter
    {
        std::optional<uint8_t>  situation;
        std::optional<uint8_t>  attacker_vocation;
        std::optional<uint8_t>  defender_vocation;
        std::optional<uint8_t>  origin;
        std::optional<uint16_t> damage_type;
        std::optional<int64_t>  combat_id;
    };

    enum class StrikeGroupDim : uint8_t { Situation, AttackerVocation, DefenderVocation, Origin, DamageType, CombatId };

    struct StrikeQueryOptions
    {
        QuerySource source = QuerySource::Session;
        StrikeFilter filter{};
        std::vector<StrikeGroupDim> group_by{};
        std::string sort_by = "total_strikes";
        bool ascending = false;
        uint32_t limit = 50;
    };

    struct StrikeQueryRow
    {
        uint8_t  situation         = 0;
        uint8_t  attacker_vocation = 0;
        uint8_t  defender_vocation = 0;
        uint8_t  origin            = 0;
        uint16_t damage_type       = 0;
        int64_t  combat_id         = 0;

        uint64_t total_strikes = 0;
        double avg_damage = 0;
        double avg_raw = 0;
        double defense_mitigation_pct = 0;
        double armor_mitigation_pct = 0;
        double modifier_mitigation_pct = 0;
        double overall_mitigation_pct = 0;
        double block_rate = 0;
        double crit_rate = 0;
        double kill_rate = 0;
        double leech_efficiency = 0;
        uint32_t min_damage = 0;
        uint32_t max_damage = 0;
    };

    [[nodiscard]] std::vector<StrikeQueryRow> Query(const StrikeQueryOptions& options);

    struct EventFilter
    {
        std::optional<uint32_t> caster_id;
        std::optional<uint32_t> target_id;
        std::optional<uint16_t> damage_type;
        std::optional<uint32_t> min_damage;
    };

    [[nodiscard]] std::vector<StrikeRecord> QueryEvents(const EventFilter& filter, uint32_t limit);

    struct ModifierQueryRow
    {
        uint8_t stance = 0;
        uint8_t mod_type = 0;
        uint64_t total_procs = 0;
        double avg_damage_delta_pct = 0;
        double avg_secondary_amount = 0;
    };

    [[nodiscard]] std::vector<ModifierQueryRow> QueryModifiers(const std::string& sort_by, uint32_t limit);

    struct ConditionFilter
    {
        std::optional<uint8_t> situation;
        std::optional<uint8_t> condition_type;
    };

    enum class ConditionGroupDim : uint8_t { ConditionType, ApplierVocation, TargetVocation, Situation };

    struct ConditionQueryOptions
    {
        QuerySource source = QuerySource::Session;
        ConditionFilter filter{};
        std::vector<ConditionGroupDim> group_by{};
        std::string sort_by = "total_damage";
        uint32_t limit = 30;
    };

    struct ConditionQueryRow
    {
        uint8_t condition_type   = 0;
        uint8_t applier_vocation = 0;
        uint8_t target_vocation  = 0;
        uint8_t situation        = 0;

        uint64_t total_applications = 0;
        double avg_damage_per_app = 0;
        double avg_damage_per_tick = 0;
        double avg_ticks_per_app = 0;
        double completion_rate = 0;
        double realised_pct = 0;
        double fatal_tick_rate = 0;
        uint64_t total_damage = 0;
    };

    [[nodiscard]] std::vector<ConditionQueryRow> QueryConditions(const ConditionQueryOptions& options);

    struct ConditionEventFilter
    {
        std::optional<uint32_t> target_id;
        std::optional<uint8_t>  condition_type;
        std::optional<uint8_t>  record_type;
    };

    [[nodiscard]] std::vector<ConditionRecord> QueryConditionEvents(const ConditionEventFilter& filter, uint32_t limit);

    struct FormulaUsageRow
    {
        int64_t combat_id = 0;
        FormulaSignature signature{};
        bool is_anomaly = false;
    };

    [[nodiscard]] std::vector<FormulaUsageRow> QueryFormulaUsage(uint32_t limit);


    void Reset(ResetScope scope);
    bool Export(const std::string& path, const std::string& format, bool include_ring);
    void RunRollingDump();
}
