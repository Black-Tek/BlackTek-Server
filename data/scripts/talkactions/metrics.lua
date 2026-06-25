-- /metrics — combat metrics operator console. See combat-metrics-proposal.md §8.
-- /metrics augment <name> is not implemented: modifier_guid is always 0 in this
-- implementation (see proposal §0.2), so there is no per-item key to filter on.

local metricsTalk = TalkAction("/metrics")

local function sendInfo(player, lines)
    player:sendTextMessage(MESSAGE_INFO_DESCR, table.concat(lines, "\n"))
end

local function pct(value)
    return string.format("%.1f%%", value or 0)
end

local function summarizeEvents(events)
    local count, dmg, raw, crits, blocked, kills = 0, 0, 0, 0, 0, 0
    for _, e in ipairs(events) do
        count = count + 1
        dmg = dmg + (e.final_damage or 0)
        raw = raw + (e.raw_output or 0)
        if e.was_critical then crits = crits + 1 end
        if e.block_type_name and e.block_type_name ~= "NoBlock" then blocked = blocked + 1 end
        if e.was_fatal then kills = kills + 1 end
    end
    return { count = count, dmg = dmg, raw = raw, crits = crits, blocked = blocked, kills = kills }
end

local function cmdPlayer(player, args)
    local name = args[1]
    if not name then
        player:sendCancelMessage("Usage: /metrics player <name>")
        return
    end

    local asAttacker = summarizeEvents(CombatMetrics.query_events({ caster_name = name }, 2000))
    local asDefender = summarizeEvents(CombatMetrics.query_events({ target_name = name }, 2000))

    local lines = { "=== Combat Metrics: " .. name .. " (recent window, up to 2000 events) ===" }

    table.insert(lines, string.format("[As Attacker - %d strikes]", asAttacker.count))
    if asAttacker.count > 0 then
        table.insert(lines, string.format("  Avg Damage: %.1f (raw: %.1f)", asAttacker.dmg / asAttacker.count, asAttacker.raw / asAttacker.count))
        table.insert(lines, string.format("  Crit Rate: %s  Block Rate: %s  Kill Rate: %s",
            pct(100 * asAttacker.crits / asAttacker.count), pct(100 * asAttacker.blocked / asAttacker.count), pct(100 * asAttacker.kills / asAttacker.count)))
    end

    table.insert(lines, string.format("[As Defender - %d strikes received]", asDefender.count))
    if asDefender.count > 0 then
        table.insert(lines, string.format("  Avg Damage Taken: %.1f (raw incoming: %.1f)", asDefender.dmg / asDefender.count, asDefender.raw / asDefender.count))
        table.insert(lines, string.format("  Block Rate: %s", pct(100 * asDefender.blocked / asDefender.count)))
    end

    sendInfo(player, lines)
end

local function vocationId(name)
    local voc = Vocation(name)
    if not voc or voc:getId() == 0 then
        return nil
    end
    return voc:getId()
end

local function cmdCompare(player, args)
    local voc1Name, voc2Name, situation = args[1], args[2], args[3]
    if not voc1Name or not voc2Name then
        player:sendCancelMessage("Usage: /metrics compare <vocation1> <vocation2> [situation]")
        return
    end

    local voc1, voc2 = vocationId(voc1Name), vocationId(voc2Name)
    if not voc1 or not voc2 then
        player:sendCancelMessage("Unknown vocation name.")
        return
    end

    local function statsFor(vocId)
        local rows = CombatMetrics.query({
            situation = situation,
            attacker_vocation = vocId,
            group_by = { "attacker_vocation" },
            sort_by = "total_strikes",
            limit = 1,
        })
        return rows[1]
    end

    local row1, row2 = statsFor(voc1), statsFor(voc2)

    local lines = { string.format("=== %s Comparison: %s vs %s (session) ===", situation or "All", voc1Name, voc2Name) }
    local function line(label, a, b)
        table.insert(lines, string.format("%-22s %-15s %-15s", label, tostring(a or "-"), tostring(b or "-")))
    end

    line("", voc1Name, voc2Name)
    if row1 then line("Avg Damage", string.format("%.1f", row1.avg_damage), row2 and string.format("%.1f", row2.avg_damage)) end
    if row1 then line("Overall Mitigation", pct(row1.overall_mitigation_pct), row2 and pct(row2.overall_mitigation_pct)) end
    if row1 then line("Crit Rate", pct(row1.crit_rate), row2 and pct(row2.crit_rate)) end
    if row1 then line("Block Rate", pct(row1.block_rate), row2 and pct(row2.block_rate)) end
    if row1 then line("Kill Rate", pct(row1.kill_rate), row2 and pct(row2.kill_rate)) end
    if row1 then line("Total Strikes", row1.total_strikes, row2 and row2.total_strikes) end

    sendInfo(player, lines)
end

local function cmdSpell(player, args)
    local combatId = tonumber(args[1])
    if not combatId then
        player:sendCancelMessage("Usage: /metrics spell <combat_id> (name lookup is not implemented)")
        return
    end

    local usage = CombatMetrics.query_formula_usage({ limit = 1000 })
    local signature
    for _, row in ipairs(usage) do
        if row.combat_id == combatId then
            signature = row
            break
        end
    end

    local rows = CombatMetrics.query({ combat_id = combatId, group_by = { "situation" }, sort_by = "total_strikes", limit = 10 })

    local lines = { string.format("=== Spell Metrics: combat_id %d ===", combatId) }
    if signature then
        table.insert(lines, string.format("Formula: output=%s defense=%s armor=%s resolution=%s bind=%s/%s%s",
            signature.output_preset, signature.defense_preset, signature.armor_preset, signature.resolution_preset,
            signature.bind_key, signature.bind_source, signature.is_anomaly and "  *** ANOMALY ***" or ""))
    else
        table.insert(lines, "No formula signature recorded yet for this combat_id.")
    end

    table.insert(lines, "")
    table.insert(lines, string.format("%-8s %-8s %-8s %-10s", "Situation", "Strikes", "AvgDmg", "Mitigation"))
    for _, row in ipairs(rows) do
        table.insert(lines, string.format("%-8s %-8d %-8.1f %-10s", row.situation, row.total_strikes, row.avg_damage, pct(row.overall_mitigation_pct)))
    end

    sendInfo(player, lines)
end

local function cmdFormulas(player, args)
    local situation = args[1]
    local rows = CombatMetrics.query_formula_usage({ situation = situation, limit = 200 })

    local lines = { "=== Formula Usage by Combat ID (session) ===" }
    table.insert(lines, string.format("%-8s %-12s %-12s %-12s %-12s %-8s %-5s", "ID", "Output", "Defense", "Armor", "Resolution", "Strikes", "Anom"))

    local anomalies = 0
    for _, row in ipairs(rows) do
        if row.is_anomaly then anomalies = anomalies + 1 end
        table.insert(lines, string.format("%-8d %-12s %-12s %-12s %-12s %-8d %s",
            row.combat_id, row.output_preset, row.defense_preset, row.armor_preset, row.resolution_preset,
            row.total_strikes, row.is_anomaly and "***" or "-"))
    end

    table.insert(lines, "")
    table.insert(lines, anomalies .. " combat IDs have non-default formula configurations.")

    sendInfo(player, lines)
end

local function cmdConditions(player, args)
    local situation
    if args[1] == "situation" then
        situation = args[2]
    end

    local rows = CombatMetrics.query_conditions({
        situation = situation,
        group_by = { "condition_type", "applier_vocation" },
        sort_by = "total_damage",
        limit = 30,
    })

    local lines = { "=== Condition Stats" .. (situation and (": " .. situation) or "") .. " (session) ===" }
    table.insert(lines, string.format("%-10s %-8s %-8s %-10s %-10s %-8s", "Condition", "Voc", "Apps", "AvgDmg", "Completion", "Realised"))
    for _, row in ipairs(rows) do
        table.insert(lines, string.format("%-10s %-8d %-8d %-10.1f %-10s %-8s",
            row.condition_type, row.applier_vocation, row.total_applications, row.avg_damage_per_app,
            pct(row.completion_rate), pct(row.realised_pct)))
    end

    sendInfo(player, lines)
end

local function cmdWindow(player, args)
    -- /metrics window is informational only: query/query_conditions accept
    -- source="session"|"window" per-call (see CombatMetrics.query in metrics.h);
    -- there is no persistent "current source" switch to flip globally.
    player:sendCancelMessage("Pass source = \"session\" or \"window\" directly in CombatMetrics.query{}/query_conditions{} calls.")
end

local function cmdExport(player, args)
    local format = args[1] or "csv"
    local timestamp = os.date("%Y-%m-%d_%H-%M-%S")
    local path = "metrics/exports/session_" .. timestamp .. "." .. format

    if CombatMetrics.export(path, format) then
        player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "[metrics] Exported session aggregates to " .. path)
    else
        player:sendCancelMessage("Export failed — check the server console log.")
    end
end

local function cmdReset(player, args)
    local scope = args[1] or "all"
    CombatMetrics.reset(scope)
    player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "[metrics] Reset scope: " .. scope)
end

function metricsTalk.onSay(player, words, param)
    if not player:getGroup():getAccess() then
        return true
    end

    local parts = param:splitTrimmed(" ")
    local sub = table.remove(parts, 1)

    if sub == "player" then
        cmdPlayer(player, parts)
    elseif sub == "compare" then
        cmdCompare(player, parts)
    elseif sub == "spell" then
        cmdSpell(player, parts)
    elseif sub == "formulas" then
        cmdFormulas(player, parts)
    elseif sub == "conditions" then
        cmdConditions(player, parts)
    elseif sub == "window" then
        cmdWindow(player, parts)
    elseif sub == "export" then
        cmdExport(player, parts)
    elseif sub == "reset" then
        cmdReset(player, parts)
    else
        player:sendTextMessage(MESSAGE_INFO_DESCR,
            "Usage: /metrics player|compare|spell|formulas|conditions|export|reset ...")
    end

    return false
end

metricsTalk:separator(" ")
metricsTalk:register()
