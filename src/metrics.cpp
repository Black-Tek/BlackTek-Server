// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"
#include "metrics.h"
#include "metrics_format.h"
#include "console.h"
#include "scheduler.h"
#include "tools.h"

#include <algorithm>
#include <ctime>
#include <fstream>
#include <filesystem>
#include <limits>
#include <toml++/toml.h>

namespace BlackTek::Metrics
{
    RuntimeConfig g_metrics_runtime{};

    namespace
    {
        gtl::flat_hash_map<uint64_t, std::pair<uint8_t, uint8_t>> g_formula_bind_table;
        gtl::flat_hash_map<uint64_t, ConditionSource> g_condition_source_map;

        [[nodiscard]] uint64_t PackBindKey(int64_t combat_id, uint8_t situation_index) noexcept
        {
            return (static_cast<uint64_t>(combat_id) << 8) | situation_index;
        }

        [[nodiscard]] uint64_t PackConditionSourceKey(uint32_t target_id, uint8_t condition_type) noexcept
        {
            return (static_cast<uint64_t>(target_id) << 8) | condition_type;
        }

        [[nodiscard]] toml::table SafeParseFile(std::string_view path)
        {
            try
            {
                return toml::parse_file(path);
            }
            catch (const toml::parse_error& err)
            {
                Console::Warn("[Metrics] Failed to parse {}: {}", path, err.description());
                return toml::table{};
            }
        }

        void PushLiveLogLine(std::ofstream* file, std::string text)
        {
            Console::Message msg(std::move(text), Console::MessageType::Log);
            msg.target_file = file;
            Console::PushAndNotify(std::move(msg));
        }

        [[nodiscard]] AggregateKey KeyFromStrike(const StrikeRecord& r) noexcept
        {
            AggregateKey key{};
            key.situation         = r.situation;
            key.attacker_vocation = r.caster_vocation;
            key.defender_vocation = r.target_vocation;
            key.origin            = r.origin;
            key.damage_type       = r.damage_type;
            key.combat_id_bucket  = ClampToInt16(r.combat_id);
            return key;
        }

        void AccumulateStrikeInto(AggregateData& data, const StrikeRecord& r) noexcept
        {
            ++data.total_strikes;

            const bool blocked  = r.block_type != 0;
            const bool critical = (r.flags & static_cast<uint8_t>(StrikeFlag::WasCritical)) != 0;
            const bool fatal    = (r.flags & static_cast<uint8_t>(StrikeFlag::WasFatal)) != 0;
            const bool batched  = (r.flags & static_cast<uint8_t>(StrikeFlag::WasBatched)) != 0;

            if (blocked)  ++data.total_blocked;
            if (critical) ++data.total_critical;
            if (fatal)    ++data.total_fatal;

            if (not batched)
            {
                data.sum_raw_output  += r.raw_output;
                data.sum_defense_abs += (r.raw_output > r.post_defense) ? (r.raw_output - r.post_defense) : 0;
                data.sum_armor_abs   += (r.post_defense > r.post_armor) ? (r.post_defense - r.post_armor) : 0;
                data.sum_modifier_abs += static_cast<int64_t>(r.post_armor) - static_cast<int64_t>(r.final_damage);
            }

            data.sum_final_damage += r.final_damage;
            data.min_damage = std::min(data.min_damage, r.final_damage);
            data.max_damage = std::max(data.max_damage, r.final_damage);

            data.sum_leech_health += r.leech_health;
            data.sum_leech_mana   += r.leech_mana;
        }

        [[nodiscard]] std::vector<AggregateData> CollectStrikeAggregates(QuerySource source)
        {
            if (source == QuerySource::Session)
                return g_metrics_store.SnapshotStrikeAggregates();

            gtl::flat_hash_map<uint64_t, AggregateData> local;

            for (const auto& record : g_metrics_store.SnapshotStrikes())
            {
                const AggregateKey key = KeyFromStrike(record);
                auto& data = local[key.Pack()];
                data.key = key;
                AccumulateStrikeInto(data, record);
            }

            std::vector<AggregateData> aggregates;
            aggregates.reserve(local.size());

            for (auto& [packed, data] : local)
                aggregates.push_back(data);

            return aggregates;
        }

        [[nodiscard]] bool PassesStrikeFilter(const AggregateData& d, const StrikeFilter& f) noexcept
        {
            if (f.situation and d.key.situation != *f.situation) return false;
            if (f.attacker_vocation and d.key.attacker_vocation != *f.attacker_vocation) return false;
            if (f.defender_vocation and d.key.defender_vocation != *f.defender_vocation) return false;
            if (f.origin and d.key.origin != *f.origin) return false;
            if (f.damage_type and d.key.damage_type != *f.damage_type) return false;
            if (f.combat_id and d.key.combat_id_bucket != ClampToInt16(*f.combat_id)) return false;
            return true;
        }

        [[nodiscard]] std::string BuildGroupKey(const std::vector<StrikeGroupDim>& dims, const AggregateKey& k)
        {
            std::string keyString;

            for (auto dim : dims)
            {
                switch (dim)
                {
                    case StrikeGroupDim::Situation:         keyString += fmt::format("s{}|", k.situation); break;
                    case StrikeGroupDim::AttackerVocation:  keyString += fmt::format("a{}|", k.attacker_vocation); break;
                    case StrikeGroupDim::DefenderVocation:  keyString += fmt::format("d{}|", k.defender_vocation); break;
                    case StrikeGroupDim::Origin:             keyString += fmt::format("o{}|", k.origin); break;
                    case StrikeGroupDim::DamageType:        keyString += fmt::format("t{}|", k.damage_type); break;
                    case StrikeGroupDim::CombatId:           keyString += fmt::format("c{}|", k.combat_id_bucket); break;
                }
            }
            return keyString;
        }

        [[nodiscard]] AggregateKey ProjectGroupKey(const std::vector<StrikeGroupDim>& dims, const AggregateKey& k) noexcept
        {
            AggregateKey projectedKey{};

            for (auto dim : dims)
            {
                switch (dim)
                {
                    case StrikeGroupDim::Situation:         projectedKey.situation = k.situation; break;
                    case StrikeGroupDim::AttackerVocation:  projectedKey.attacker_vocation = k.attacker_vocation; break;
                    case StrikeGroupDim::DefenderVocation:  projectedKey.defender_vocation = k.defender_vocation; break;
                    case StrikeGroupDim::Origin:             projectedKey.origin = k.origin; break;
                    case StrikeGroupDim::DamageType:        projectedKey.damage_type = k.damage_type; break;
                    case StrikeGroupDim::CombatId:           projectedKey.combat_id_bucket = k.combat_id_bucket; break;
                }
            }
            return projectedKey;
        }

        [[nodiscard]] StrikeQueryRow ToStrikeRow(const AggregateData& d)
        {
            StrikeQueryRow row{};
            row.situation         = d.key.situation;
            row.attacker_vocation = d.key.attacker_vocation;
            row.defender_vocation = d.key.defender_vocation;
            row.origin            = d.key.origin;
            row.damage_type       = d.key.damage_type;
            row.combat_id         = d.key.combat_id_bucket;
            row.total_strikes     = d.total_strikes;

            const double strikes = static_cast<double>(std::max<uint64_t>(d.total_strikes, 1));
            const double rawSum  = static_cast<double>(std::max<uint64_t>(d.sum_raw_output, 1));
            const double postDefenseTotal = static_cast<double>(d.sum_raw_output) - static_cast<double>(d.sum_defense_abs);

            row.avg_damage              = static_cast<double>(d.sum_final_damage) / strikes;
            row.avg_raw                 = static_cast<double>(d.sum_raw_output) / strikes;
            row.defense_mitigation_pct  = static_cast<double>(d.sum_defense_abs) / rawSum * 100.0;
            row.armor_mitigation_pct    = postDefenseTotal > 0.0 ? static_cast<double>(d.sum_armor_abs) / postDefenseTotal * 100.0 : 0.0;
            row.modifier_mitigation_pct = static_cast<double>(d.sum_modifier_abs) / rawSum * 100.0;
            row.overall_mitigation_pct  = (static_cast<double>(d.sum_raw_output) - static_cast<double>(d.sum_final_damage)) / rawSum * 100.0;
            row.block_rate              = static_cast<double>(d.total_blocked) / strikes * 100.0;
            row.crit_rate               = static_cast<double>(d.total_critical) / strikes * 100.0;
            row.kill_rate               = static_cast<double>(d.total_fatal) / strikes * 100.0;
            row.leech_efficiency        = static_cast<double>(d.sum_leech_health) / static_cast<double>(std::max<uint64_t>(d.sum_final_damage, 1)) * 100.0;
            row.min_damage = (d.min_damage == 0xFFFFFFFFu) ? 0 : d.min_damage;
            row.max_damage = d.max_damage;
            return row;
        }

        [[nodiscard]] double StrikeRowMetric(const StrikeQueryRow& row, const std::string& name) noexcept
        {
            if (name == "total_strikes") return static_cast<double>(row.total_strikes);
            if (name == "avg_damage") return row.avg_damage;
            if (name == "avg_raw") return row.avg_raw;
            if (name == "defense_mitigation_pct") return row.defense_mitigation_pct;
            if (name == "armor_mitigation_pct") return row.armor_mitigation_pct;
            if (name == "modifier_mitigation_pct") return row.modifier_mitigation_pct;
            if (name == "overall_mitigation_pct") return row.overall_mitigation_pct;
            if (name == "block_rate") return row.block_rate;
            if (name == "crit_rate") return row.crit_rate;
            if (name == "kill_rate") return row.kill_rate;
            if (name == "leech_efficiency") return row.leech_efficiency;
            if (name == "min_damage") return static_cast<double>(row.min_damage);
            if (name == "max_damage") return static_cast<double>(row.max_damage);
            return static_cast<double>(row.total_strikes);
        }
    }

    void Initialize()
    {
        const auto tbl = SafeParseFile("config/metrics.toml");
        const auto metricsTbl = tbl["metrics"];

        g_metrics_runtime.enabled            = metricsTbl["enabled"].value_or(true);
        g_metrics_runtime.capture_pipeline    = metricsTbl["capture_pipeline"].value_or(true);
        g_metrics_runtime.capture_formulas    = metricsTbl["capture_formulas"].value_or(true);
        g_metrics_runtime.capture_modifiers   = metricsTbl["capture_modifiers"].value_or(true);
        g_metrics_runtime.capture_healing     = metricsTbl["capture_healing"].value_or(true);
        g_metrics_runtime.capture_conditions  = metricsTbl["capture_conditions"].value_or(true);

        const auto liveLogTbl = tbl["metrics"]["live_log"];
        g_metrics_runtime.live_log.enabled        = liveLogTbl["enabled"].value_or(true);
        g_metrics_runtime.live_log.directory      = liveLogTbl["directory"].value_or<std::string>("logs/combat/metrics");
        g_metrics_runtime.live_log.format         = liveLogTbl["format"].value_or<std::string>("csv");
        g_metrics_runtime.live_log.max_file_mb    = static_cast<uint32_t>(liveLogTbl["max_file_mb"].value_or<int64_t>(100));
        g_metrics_runtime.live_log.daily_rotation = liveLogTbl["daily_rotation"].value_or(true);

        const auto dumpTbl = tbl["metrics"]["rolling_dump"];
        g_metrics_runtime.rolling_dump.enabled          = dumpTbl["enabled"].value_or(true);
        g_metrics_runtime.rolling_dump.interval_hours   = dumpTbl["interval_hours"].value_or(4.0);
        g_metrics_runtime.rolling_dump.directory        = dumpTbl["directory"].value_or<std::string>("logs/combat/metrics/dumps");
        g_metrics_runtime.rolling_dump.format           = dumpTbl["format"].value_or<std::string>("json");
        g_metrics_runtime.rolling_dump.reset_after_dump = dumpTbl["reset_after_dump"].value_or(false);
        g_metrics_runtime.rolling_dump.keep_dump_count  = static_cast<uint32_t>(dumpTbl["keep_dump_count"].value_or<int64_t>(24));

        if constexpr (not ENABLED)
            return;

        if (not g_metrics_runtime.enabled)
            return;

        if (g_metrics_runtime.live_log.enabled)
        {
            // Registered before OpenMetricsChannels so the very first file (and every
            // file OpenMetricsChannel later creates on daily/size rotation) gets a
            // header written synchronously at open time
            Console::SetMetricsChannelHeader(0,
                "timestamp_ms,combat_id,caster_id,caster_name,caster_vocation,caster_level,"
                "target_id,target_name,target_vocation,target_level,situation,origin,damage_type,"
                "output_preset,defense_preset,armor_preset,resolution_preset,"
                "output_formula,defense_formula,armor_formula,resolution_formula,"
                "bind_key,bind_source,has_override,has_lua,"
                "raw_output,defense_value,armor_value,post_defense,post_armor,"
                "modifier_delta,final_damage,mana_drained,block_type,"
                "was_critical,was_fatal,had_augment_mods,had_bounce,"
                "leech_health,leech_mana,leech_stamina,leech_soul");

            Console::SetMetricsChannelHeader(1,
                "timestamp_ms,caster_id,target_id,modifier_guid,stance,mod_type,"
                "damage_type,damage_before,damage_after,secondary_amount");

            Console::SetMetricsChannelHeader(2,
                "timestamp_ms,combat_id,caster_id,target_id,origin,stat_target,"
                "amount_requested,amount_healed,caster_vocation,target_vocation,situation");

            Console::SetMetricsChannelHeader(3,
                "timestamp_ms,instance_guid,applier_id,target_id,source_combat_id,"
                "record_type,condition_type,combat_type,"
                "expected_ticks,per_tick_amount,total_expected_dmg,"
                "tick_number,tick_damage,running_total,"
                "was_fatal,applier_vocation,target_vocation,situation");

            Console::SetMetricsChannelHeader(4,
                "timestamp_ms,combat_id,strike_count_at_detection,"
                "expected_output_preset,expected_resolution_preset,expected_bind_key,"
                "observed_output_preset,observed_resolution_preset,observed_bind_key,"
                "has_per_combat_override_or_lua");

            Console::OpenMetricsChannels(g_metrics_runtime.live_log.directory, g_metrics_runtime.live_log.format, g_metrics_runtime.live_log.max_file_mb);
        }

        if (g_metrics_runtime.rolling_dump.enabled and g_metrics_runtime.rolling_dump.interval_hours > 0.0)
        {
            const auto delayMs = static_cast<uint32_t>(g_metrics_runtime.rolling_dump.interval_hours * 3600.0 * 1000.0);
            g_scheduler.addEvent(createSchedulerTask(delayMs, []() { RunRollingDump(); }));
        }
    }

    void RecordStrike(const StrikeRecord& record, uint32_t modifier_proc_count, std::string_view caster_name, std::string_view target_name)
    {
        if constexpr (not ENABLED)
            return;

        if (not g_metrics_runtime.enabled)
            return;

        g_metrics_store.RecordStrike(record, modifier_proc_count);

        if (g_metrics_runtime.live_log.enabled)
        {
            // todo: implement working "both" option, we fall back to jsonl for now
            const std::string line = (g_metrics_runtime.live_log.format == "jsonl" or g_metrics_runtime.live_log.format == "both")
                ? FormatStrikeJsonl(record, caster_name, target_name)
                : FormatStrikeCsv(record, caster_name, target_name);
            PushLiveLogLine(&Console::metrics_strikes_file, line);
        }
    }

    void RecordModifier(const ModifierRecord& record)
    {
        if constexpr (not ENABLED)
            return;

        if (not g_metrics_runtime.enabled or not g_metrics_runtime.capture_modifiers)
            return;

        g_metrics_store.RecordModifier(record);

        if (g_metrics_runtime.live_log.enabled)
            PushLiveLogLine(&Console::metrics_modifiers_file, FormatModifierCsv(record));
    }

    void RecordHeal(const HealRecord& record)
    {
        if constexpr (not ENABLED)
            return;

        if (not g_metrics_runtime.enabled or not g_metrics_runtime.capture_healing)
            return;

        g_metrics_store.RecordHeal(record);

        if (g_metrics_runtime.live_log.enabled)
            PushLiveLogLine(&Console::metrics_heals_file, FormatHealCsv(record));
    }

    void RecordCondition(const ConditionRecord& record)
    {
        if constexpr (not ENABLED)
            return;

        if (not g_metrics_runtime.enabled or not g_metrics_runtime.capture_conditions)
            return;

        g_metrics_store.RecordCondition(record);

        if (g_metrics_runtime.live_log.enabled)
            PushLiveLogLine(&Console::metrics_conditions_file, FormatConditionCsv(record));
    }

    void RecordFormulaUsage(int64_t combat_id, const FormulaSnapshot& snapshot, uint64_t timestamp_ms)
    {
        if constexpr (not ENABLED)
            return;

        if (not g_metrics_runtime.enabled or not g_metrics_runtime.capture_formulas)
            return;

        const bool mismatch = g_metrics_store.UpdateFormulaUsage(combat_id, snapshot, timestamp_ms);

        if (mismatch and g_metrics_runtime.live_log.enabled)
        {
            // The signature stored under combat_id is the first-seen baseline that this
            // snapshot was just compared against so we re-read it to report expected-vs-observed.
            const auto baseline = g_metrics_store.GetFormulaSignature(combat_id);
            if (baseline)
            {
                PushLiveLogLine(&Console::metrics_anomalies_file,
                    FormatAnomalyCsv(combat_id, baseline->total_strikes, *baseline, snapshot));
            }
        }
    }

    void RecordFormulaBind(int64_t combat_id, uint8_t situation_index, uint8_t bind_key, uint8_t bind_source)
    {
        if constexpr (not ENABLED)
            return;

        g_formula_bind_table[PackBindKey(combat_id, situation_index)] = { bind_key, bind_source };
    }

    std::optional<std::pair<uint8_t, uint8_t>> LookupFormulaBind(int64_t combat_id, uint8_t situation_index) noexcept
    {
        const auto it = g_formula_bind_table.find(PackBindKey(combat_id, situation_index));
        if (it == g_formula_bind_table.end())
            return std::nullopt;
        return it->second;
    }

    void RegisterConditionSource(uint32_t target_id, uint8_t condition_type, const ConditionSource& source)
    {
        if constexpr (not ENABLED)
            return;

        g_condition_source_map[PackConditionSourceKey(target_id, condition_type)] = source;
    }

    std::optional<ConditionSource> LookupConditionSource(uint32_t target_id, uint8_t condition_type) noexcept
    {
        const auto it = g_condition_source_map.find(PackConditionSourceKey(target_id, condition_type));
        if (it == g_condition_source_map.end())
            return std::nullopt;
        return it->second;
    }

    void ClearConditionSource(uint32_t target_id, uint8_t condition_type) noexcept
    {
        g_condition_source_map.erase(PackConditionSourceKey(target_id, condition_type));
    }

    std::optional<ConditionSource> AccumulateConditionTick(uint32_t target_id, uint8_t condition_type, uint32_t tick_damage) noexcept
    {
        const auto it = g_condition_source_map.find(PackConditionSourceKey(target_id, condition_type));
        if (it == g_condition_source_map.end())
            return std::nullopt;

        it->second.running_total += tick_damage;
        ++it->second.tick_count;
        return it->second;
    }

    std::vector<StrikeQueryRow> Query(const StrikeQueryOptions& options)
    {
        auto aggregates = CollectStrikeAggregates(options.source);

        gtl::flat_hash_map<std::string, AggregateData> buckets;

        for (const auto& d : aggregates)
        {
            if (not PassesStrikeFilter(d, options.filter))
                continue;

            const std::string groupKeyStr = BuildGroupKey(options.group_by, d.key);
            auto& bucket = buckets[groupKeyStr];

            if (bucket.total_strikes == 0 and bucket.total_blocked == 0)
                bucket.key = ProjectGroupKey(options.group_by, d.key);

            bucket.total_strikes  += d.total_strikes;
            bucket.total_blocked  += d.total_blocked;
            bucket.total_critical += d.total_critical;
            bucket.total_fatal    += d.total_fatal;
            bucket.sum_raw_output  += d.sum_raw_output;
            bucket.sum_defense_abs += d.sum_defense_abs;
            bucket.sum_armor_abs   += d.sum_armor_abs;
            bucket.sum_modifier_abs += d.sum_modifier_abs;
            bucket.sum_final_damage += d.sum_final_damage;
            bucket.min_damage = std::min(bucket.min_damage, d.min_damage);
            bucket.max_damage = std::max(bucket.max_damage, d.max_damage);
            bucket.sum_leech_health += d.sum_leech_health;
            bucket.sum_leech_mana   += d.sum_leech_mana;
            bucket.modifier_procs   += d.modifier_procs;
        }

        std::vector<StrikeQueryRow> rows;
        rows.reserve(buckets.size());
        for (const auto& [groupKeyStr, data] : buckets)
            rows.push_back(ToStrikeRow(data));

        std::ranges::sort(rows, [&](const StrikeQueryRow& a, const StrikeQueryRow& b)
        {
            return options.ascending
                ? StrikeRowMetric(a, options.sort_by) < StrikeRowMetric(b, options.sort_by)
                : StrikeRowMetric(a, options.sort_by) > StrikeRowMetric(b, options.sort_by);
        });

        if (rows.size() > options.limit)
            rows.resize(options.limit);

        return rows;
    }

    std::vector<StrikeRecord> QueryEvents(const EventFilter& filter, uint32_t limit)
    {
        auto events = g_metrics_store.SnapshotStrikes();
        std::vector<StrikeRecord> results;

        for (auto it = events.rbegin(); it != events.rend() and results.size() < limit; ++it)
        {
            const auto& strikeEvent = *it;
            if (filter.caster_id and strikeEvent.caster_id != *filter.caster_id) continue;
            if (filter.target_id and strikeEvent.target_id != *filter.target_id) continue;
            if (filter.damage_type and strikeEvent.damage_type != *filter.damage_type) continue;
            if (filter.min_damage and strikeEvent.final_damage < *filter.min_damage) continue;
            results.push_back(strikeEvent);
        }
        return results;
    }

    std::vector<ModifierQueryRow> QueryModifiers(const std::string& sort_by, uint32_t limit)
    {
        std::vector<ModifierQueryRow> rows;

        for (const auto& data : g_metrics_store.SnapshotModifierAggregates())
        {
            ModifierQueryRow row{};
            row.stance = data.key.stance;
            row.mod_type = data.key.mod_type;
            row.total_procs = data.total_procs;

            const double before = static_cast<double>(std::max<uint64_t>(data.sum_damage_before, 1));
            row.avg_damage_delta_pct = (static_cast<double>(data.sum_damage_after) - static_cast<double>(data.sum_damage_before)) / before * 100.0;
            row.avg_secondary_amount = static_cast<double>(data.sum_secondary_amount) / static_cast<double>(std::max<uint64_t>(data.total_procs, 1));

            rows.push_back(row);
        }

        std::ranges::sort(rows, [&](const ModifierQueryRow& a, const ModifierQueryRow& b)
        {
            if (sort_by == "avg_secondary_amount") return a.avg_secondary_amount > b.avg_secondary_amount;
            if (sort_by == "avg_damage_delta_pct") return a.avg_damage_delta_pct > b.avg_damage_delta_pct;
            return a.total_procs > b.total_procs;
        });

        if (rows.size() > limit)
            rows.resize(limit);

        return rows;
    }

    std::vector<ConditionQueryRow> QueryConditions(const ConditionQueryOptions& options)
    {
        std::vector<ConditionAggregateData> source;
        if (options.source == QuerySource::Session)
        {
            source = g_metrics_store.SnapshotConditionAggregates();
        }
        else
        {
            // Window source re-derives aggregates from the ring buffer snapshot ONLY
            // it must NEVER call back into AggregateStore's mutating Record* methods.
            gtl::flat_hash_map<uint64_t, ConditionAggregateData> local;
            for (const auto& r : g_metrics_store.SnapshotConditions())
            {
                ConditionAggregateKey key{};
                key.situation        = r.situation;
                key.applier_vocation = r.applier_vocation;
                key.target_vocation  = r.target_vocation;
                key.condition_type   = r.condition_type;
                key.combat_type      = r.combat_type;
                key.source_combat_id = ClampToInt16(r.source_combat_id);

                auto& data = local[key.Pack()];
                data.key = key;

                switch (static_cast<ConditionRecordType>(r.record_type))
                {
                    case ConditionRecordType::Application:
                        ++data.total_applications;
                        data.sum_expected_dmg += r.total_expected_dmg;
                        break;

                    case ConditionRecordType::Tick:
                        ++data.total_ticks;
                        data.total_damage += r.tick_damage;
                        data.min_tick_damage = std::min(data.min_tick_damage, r.tick_damage);
                        data.max_tick_damage = std::max(data.max_tick_damage, r.tick_damage);
                        if (r.was_fatal) ++data.total_fatal_ticks;
                        break;

                    case ConditionRecordType::Expiry:
                        ++data.total_completed;
                        break;

                    case ConditionRecordType::Cancelled:
                        ++data.total_cancelled;
                        break;
                }
            }

            for (auto& [packed, data] : local)
                source.push_back(data);
        }

        gtl::flat_hash_map<std::string, ConditionAggregateData> buckets;

        for (const auto& d : source)
        {
            if (options.filter.situation and d.key.situation != *options.filter.situation) continue;
            if (options.filter.condition_type and d.key.condition_type != *options.filter.condition_type) continue;

            std::string groupKeyStr;
            ConditionAggregateKey projected{};

            for (auto dim : options.group_by)
            {
                switch (dim)
                {
                    case ConditionGroupDim::ConditionType:    groupKeyStr += fmt::format("c{}|", d.key.condition_type); projected.condition_type = d.key.condition_type; break;
                    case ConditionGroupDim::ApplierVocation:  groupKeyStr += fmt::format("a{}|", d.key.applier_vocation); projected.applier_vocation = d.key.applier_vocation; break;
                    case ConditionGroupDim::TargetVocation:   groupKeyStr += fmt::format("t{}|", d.key.target_vocation); projected.target_vocation = d.key.target_vocation; break;
                    case ConditionGroupDim::Situation:        groupKeyStr += fmt::format("s{}|", d.key.situation); projected.situation = d.key.situation; break;
                }
            }

            auto& bucket = buckets[groupKeyStr];

            if (bucket.total_applications == 0 and bucket.total_ticks == 0)
                bucket.key = projected;

            bucket.total_applications += d.total_applications;
            bucket.total_ticks        += d.total_ticks;
            bucket.total_damage       += d.total_damage;
            bucket.total_fatal_ticks  += d.total_fatal_ticks;
            bucket.total_completed    += d.total_completed;
            bucket.total_cancelled    += d.total_cancelled;
            bucket.min_tick_damage = std::min(bucket.min_tick_damage, d.min_tick_damage);
            bucket.max_tick_damage = std::max(bucket.max_tick_damage, d.max_tick_damage);
            bucket.sum_expected_dmg += d.sum_expected_dmg;
        }

        std::vector<ConditionQueryRow> rows;

        for (const auto& [groupKeyStr, d] : buckets)
        {
            ConditionQueryRow row{};
            row.condition_type   = d.key.condition_type;
            row.applier_vocation = d.key.applier_vocation;
            row.target_vocation  = d.key.target_vocation;
            row.situation        = d.key.situation;
            row.total_applications = d.total_applications;
            row.total_damage = d.total_damage;

            const double apps  = static_cast<double>(std::max<uint64_t>(d.total_applications, 1));
            const double ticks = static_cast<double>(std::max<uint64_t>(d.total_ticks, 1));

            row.avg_damage_per_app  = static_cast<double>(d.total_damage) / apps;
            row.avg_damage_per_tick = static_cast<double>(d.total_damage) / ticks;
            row.avg_ticks_per_app   = static_cast<double>(d.total_ticks) / apps;
            row.completion_rate     = static_cast<double>(d.total_completed) / apps * 100.0;
            row.realised_pct        = static_cast<double>(d.total_damage) / static_cast<double>(std::max<uint64_t>(d.sum_expected_dmg, 1)) * 100.0;
            row.fatal_tick_rate     = static_cast<double>(d.total_fatal_ticks) / ticks * 100.0;

            rows.push_back(row);
        }

        std::ranges::sort(rows, [&](const ConditionQueryRow& a, const ConditionQueryRow& b)
        {
            if (options.sort_by == "avg_damage_per_app") return a.avg_damage_per_app > b.avg_damage_per_app;
            if (options.sort_by == "completion_rate") return a.completion_rate > b.completion_rate;
            return a.total_damage > b.total_damage;
        });

        if (rows.size() > options.limit)
            rows.resize(options.limit);

        return rows;
    }

    std::vector<ConditionRecord> QueryConditionEvents(const ConditionEventFilter& filter, uint32_t limit)
    {
        auto events = g_metrics_store.SnapshotConditions();
        std::vector<ConditionRecord> results;

        for (auto it = events.rbegin(); it != events.rend() and results.size() < limit; ++it)
        {
            const auto& conditionEvent = *it;
            if (filter.target_id and conditionEvent.target_id != *filter.target_id) continue;
            if (filter.condition_type and conditionEvent.condition_type != *filter.condition_type) continue;
            if (filter.record_type and conditionEvent.record_type != *filter.record_type) continue;
            results.push_back(conditionEvent);
        }
        return results;
    }

    std::vector<FormulaUsageRow> QueryFormulaUsage(uint32_t limit)
    {
        FormulaSignature defaultSignature{};
        bool haveDefault = false;

        std::vector<FormulaUsageRow> rows;

        for (const auto& signature : g_metrics_store.SnapshotFormulaUsage())
        {
            FormulaUsageRow row{};
            row.signature = signature;
            rows.push_back(row);

            if (not haveDefault)
            {
                defaultSignature = signature;
                haveDefault = true;
            }
        }

        for (auto& row : rows)
        {
            row.is_anomaly = row.signature.inconsistency_count > 0
                or row.signature.output_preset != defaultSignature.output_preset
                or row.signature.resolution_preset != defaultSignature.resolution_preset;
        }

        std::ranges::sort(rows, [](const FormulaUsageRow& a, const FormulaUsageRow& b)
        {
            return a.signature.total_strikes > b.signature.total_strikes;
        });

        if (rows.size() > limit)
            rows.resize(limit);

        return rows;
    }

    void Reset(ResetScope scope)
    {
        g_metrics_store.Reset(scope);
    }

    bool Export(const std::string& path, const std::string& format, bool include_ring)
    {
        std::ofstream file(path, std::ios::out | std::ios::trunc);
        if (not file.is_open())
            return false;

        if (format == "csv")
        {
            file << "situation,attacker_vocation,defender_vocation,origin,damage_type,combat_id,"
                    "total_strikes,avg_damage,avg_raw,defense_mitigation_pct,armor_mitigation_pct,"
                    "modifier_mitigation_pct,overall_mitigation_pct,block_rate,crit_rate,kill_rate,"
                    "leech_efficiency,min_damage,max_damage\n";

            for (const auto& d : g_metrics_store.SnapshotStrikeAggregates())
            {
                const auto row = ToStrikeRow(d);
                file << fmt::format("{},{},{},{},{},{},{},{:.2f},{:.2f},{:.2f},{:.2f},{:.2f},{:.2f},{:.2f},{:.2f},{:.2f},{:.2f},{},{}\n",
                    SituationName(row.situation), row.attacker_vocation, row.defender_vocation, OriginName(row.origin),
                    row.damage_type, row.combat_id, row.total_strikes, row.avg_damage, row.avg_raw,
                    row.defense_mitigation_pct, row.armor_mitigation_pct, row.modifier_mitigation_pct,
                    row.overall_mitigation_pct, row.block_rate, row.crit_rate, row.kill_rate,
                    row.leech_efficiency, row.min_damage, row.max_damage);
            }
            return true;
        }

        // JSON
        file << "{\n  \"strike_aggregates\": [\n";
        const auto strikeAggs = g_metrics_store.SnapshotStrikeAggregates();
        for (size_t strikeIndex = 0; strikeIndex < strikeAggs.size(); ++strikeIndex)
        {
            const auto row = ToStrikeRow(strikeAggs[strikeIndex]);
            file << fmt::format("    {{\"situation\":\"{}\",\"attacker_vocation\":{},\"defender_vocation\":{},"
                "\"origin\":\"{}\",\"damage_type\":{},\"combat_id\":{},\"total_strikes\":{},\"avg_damage\":{:.2f},"
                "\"overall_mitigation_pct\":{:.2f},\"crit_rate\":{:.2f},\"block_rate\":{:.2f}}}{}\n",
                SituationName(row.situation), row.attacker_vocation, row.defender_vocation, OriginName(row.origin),
                row.damage_type, row.combat_id, row.total_strikes, row.avg_damage, row.overall_mitigation_pct,
                row.crit_rate, row.block_rate, (strikeIndex + 1 < strikeAggs.size()) ? "," : "");
        }
        file << "  ]";

        if (include_ring)
        {
            file << ",\n  \"ring_sample\": {\n    \"strikes\": [\n";
            const auto strikes = g_metrics_store.SnapshotStrikes();
            for (size_t strikeIndex = 0; strikeIndex < strikes.size(); ++strikeIndex)
                file << "      " << FormatStrikeJsonl(strikes[strikeIndex], "", "") << (strikeIndex + 1 < strikes.size() ? "," : "") << "\n";
            file << "    ]\n  }";
        }

        file << "\n}\n";
        return true;
    }

    void RunRollingDump()
    {
        if (g_metrics_runtime.rolling_dump.enabled)
        {
            const std::time_t dumpTime = static_cast<std::time_t>(OTSYS_TIME() / 1000);
            const std::string timestamp = fmt::format("{:%Y-%m-%d_%H-%M-%S}", *std::localtime(&dumpTime));
            std::filesystem::create_directories(g_metrics_runtime.rolling_dump.directory);
            const std::string path = g_metrics_runtime.rolling_dump.directory + "/dump_" + timestamp + ".json";

            std::ofstream file(path, std::ios::out | std::ios::trunc);
            if (file.is_open())
            {
                file << "{\n  \"meta\": {\"dump_time\": \"" << timestamp << "\"},\n  \"window\": {\n    \"strikes\": [\n";
                const auto strikes = g_metrics_store.SnapshotStrikes();
                for (size_t strikeIndex = 0; strikeIndex < strikes.size(); ++strikeIndex)
                    file << "      " << FormatStrikeJsonl(strikes[strikeIndex], "", "") << (strikeIndex + 1 < strikes.size() ? "," : "") << "\n";
                file << "    ]\n  }\n}\n";
            }

            if (g_metrics_runtime.rolling_dump.reset_after_dump)
                g_metrics_store.Reset(ResetScope::Window);

            if (g_metrics_runtime.rolling_dump.keep_dump_count > 0)
            {
                std::vector<std::filesystem::path> dumps;
                std::error_code ec;
                for (const auto& entry : std::filesystem::directory_iterator(g_metrics_runtime.rolling_dump.directory, ec))
                {
                    if (entry.is_regular_file() and entry.path().filename().string().starts_with("dump_"))
                        dumps.push_back(entry.path());
                }

                std::ranges::sort(dumps);
                while (dumps.size() > g_metrics_runtime.rolling_dump.keep_dump_count)
                {
                    std::filesystem::remove(dumps.front(), ec);
                    dumps.erase(dumps.begin());
                }
            }
        }

        if (g_metrics_runtime.rolling_dump.enabled and g_metrics_runtime.rolling_dump.interval_hours > 0.0)
        {
            const auto delayMs = static_cast<uint32_t>(g_metrics_runtime.rolling_dump.interval_hours * 3600.0 * 1000.0);
            g_scheduler.addEvent(createSchedulerTask(delayMs, []() { RunRollingDump(); }));
        }
    }
}
