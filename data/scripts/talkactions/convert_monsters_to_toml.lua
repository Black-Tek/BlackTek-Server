-- Converts monster XML files to TOML format.
-- Usage: /convertmonsters
-- Reads data/monster/monsters/*.xml and writes data/monster/monsters/*.toml
-- Run this once as a GM, then switch the server to TOML loading.

local function escapeTomlString(s)
    s = s:gsub("\\", "\\\\")
    s = s:gsub('"', '\\"')
    return s
end

local function tomlString(s)
    return '"' .. escapeTomlString(tostring(s)) .. '"'
end

local function tomlBool(v)
    return (v == "1" or v == "true" or v == "yes") and "true" or "false"
end

local function tomlBoolAttr(node, attrName, default)
    local v = node:attribute(attrName)
    if v == "" then return default end
    return tomlBool(v)
end

local function intAttr(node, attrName, default)
    local v = node:attribute(attrName)
    if v == "" then return default end
    return tonumber(v) or default
end

local function strAttr(node, attrName, default)
    local v = node:attribute(attrName)
    return (v ~= "") and v or default
end

-- Maps XML immunity/element names to our TOML names
local combatImmunityMap = {
    physical = "physical",
    energy = "energy",
    fire = "fire",
    earth = "earth",
    poison = "earth",
    drown = "drown",
    ice = "ice",
    holy = "holy",
    death = "death",
    lifedrain = "lifedrain",
    manadrain = "manadrain",
}

local conditionImmunityMap = {
    physical = "physical",
    bleed = "physical",
    energy = "energy",
    fire = "fire",
    earth = "earth",
    poison = "earth",
    drown = "drown",
    ice = "ice",
    holy = "holy",
    death = "death",
    paralyze = "paralyze",
    outfit = "outfit",
    drunk = "drunk",
    invisible = "invisible",
    invisibility = "invisible",
}

local elementNameMap = {
    physicalPercent = "physical",
    energyPercent = "energy",
    firePercent = "fire",
    earthPercent = "earth",
    poisonPercent = "earth",
    drowndPercent = "drown",
    drownPercent = "drown",
    icePercent = "ice",
    holyPercent = "holy",
    deathPercent = "death",
    lifedrainPercent = "lifedrain",
    manadrainPercent = "manadrain",
}

local function getAttributeValue(attackNode, key)
    for attrNode in attackNode:children() do
        if attrNode:name() == "attribute" and attrNode:attribute("key") == key then
            return attrNode:attribute("value")
        end
    end
    return ""
end

local function convertSpell(node)
    local parts = {}
    local name = strAttr(node, "name", "")
    local interval = intAttr(node, "interval", 2000)
    local chance = intAttr(node, "chance", 100)
    local range = intAttr(node, "range", 0)
    local min = intAttr(node, "min", 0)
    local max = intAttr(node, "max", 0)
    local radius = intAttr(node, "radius", 0)
    local length = intAttr(node, "length", 0)
    local spread = intAttr(node, "spread", 0)
    local duration = intAttr(node, "duration", 0)
    local targetVal = intAttr(node, "target", 0)
    local speedChange = intAttr(node, "speedchange", 0)
    local attack = intAttr(node, "attack", 0)
    local skill = intAttr(node, "skill", 0)
    local drunkenness = intAttr(node, "drunkenness", 0)

    local shootEffect = getAttributeValue(node, "shootEffect")
    local areaEffect = getAttributeValue(node, "areaEffect")

    table.insert(parts, string.format("name = %s", tomlString(name)))
    table.insert(parts, string.format("interval = %d", interval))
    if chance ~= 100 then
        table.insert(parts, string.format("chance = %d", chance))
    end
    if range ~= 0 then table.insert(parts, string.format("range = %d", range)) end
    if min ~= 0 then table.insert(parts, string.format("min = %d", min)) end
    if max ~= 0 then table.insert(parts, string.format("max = %d", max)) end
    if radius ~= 0 then table.insert(parts, string.format("radius = %d", radius)) end
    if length ~= 0 then table.insert(parts, string.format("length = %d", length)) end
    if spread ~= 0 then table.insert(parts, string.format("spread = %d", spread)) end
    if duration ~= 0 then table.insert(parts, string.format("duration = %d", duration)) end
    if targetVal ~= 0 then table.insert(parts, "target = true") end
    if speedChange ~= 0 then table.insert(parts, string.format("speedChange = %d", speedChange)) end
    if attack ~= 0 then table.insert(parts, string.format("attack = %d", attack)) end
    if skill ~= 0 then table.insert(parts, string.format("skill = %d", skill)) end
    if drunkenness ~= 0 then table.insert(parts, string.format("drunkenness = %d", drunkenness)) end
    if shootEffect ~= "" then table.insert(parts, string.format("shootEffect = %s", tomlString(shootEffect))) end
    if areaEffect ~= "" then table.insert(parts, string.format("areaEffect = %s", tomlString(areaEffect))) end

    -- Check for condition on poison/melee attack
    local poisonDmg = intAttr(node, "poison", 0)
    if poisonDmg ~= 0 then
        table.insert(parts, string.format('condition = {type = "earth", minDamage = %d, maxDamage = %d, interval = 2000}', -poisonDmg, -poisonDmg))
    end

    return "{" .. table.concat(parts, ", ") .. "}"
end

local function convertLootItem(node)
    local parts = {}
    local itemName = strAttr(node, "name", "")
    local chance = intAttr(node, "chance", 0)
    local countmin = intAttr(node, "countmin", 1)
    local countmax = intAttr(node, "countmax", 1)
    local subtype = intAttr(node, "subtype", 0)
    local actionid = intAttr(node, "actionid", 0)
    local text = strAttr(node, "text", "")
    local unique = tomlBoolAttr(node, "unique", "false")

    table.insert(parts, string.format("name = %s", tomlString(itemName)))
    table.insert(parts, string.format("chance = %d", chance))
    if countmin ~= 1 then table.insert(parts, string.format("countmin = %d", countmin)) end
    if countmax ~= 1 then table.insert(parts, string.format("countmax = %d", countmax)) end
    if subtype ~= 0 then table.insert(parts, string.format("subType = %d", subtype)) end
    if actionid ~= 0 then table.insert(parts, string.format("actionId = %d", actionid)) end
    if text ~= "" then table.insert(parts, string.format("text = %s", tomlString(text))) end
    if unique == "true" then table.insert(parts, "unique = true") end

    -- Check for child loot
    local children = {}
    for child in node:children() do
        if child:name() == "item" then
            table.insert(children, convertLootItem(child))
        end
    end

    local base = "{" .. table.concat(parts, ", ") .. "}"
    if #children > 0 then
        -- Rebuild with child array
        table.insert(parts, "child = [" .. table.concat(children, ", ") .. "]")
        base = "{" .. table.concat(parts, ", ") .. "}"
    end

    return base
end

local function convertMonsterXmlToToml(xmlPath, tomlPath)
    local doc = XMLDocument(xmlPath)
    if not doc then
        return false, "Could not open XML: " .. xmlPath
    end

    local root = doc:child("monster")
    if not root then
        return false, "No <monster> root in " .. xmlPath
    end

    local lines = {}

    local monName = strAttr(root, "name", "Unknown")
    local monKey = monName:lower():gsub("[^a-z0-9_]", "_")

    local function w(s) table.insert(lines, s) end

    w(string.format("[%s]", monKey))
    w(string.format("name = %s", tomlString(monName)))

    local nameDesc = strAttr(root, "nameDescription", "")
    if nameDesc ~= "" then
        w(string.format("nameDescription = %s", tomlString(nameDesc)))
    end

    w(string.format("race = %s", tomlString(strAttr(root, "race", "blood"))))
    w(string.format("experience = %d", intAttr(root, "experience", 0)))
    w(string.format("speed = %d", intAttr(root, "speed", 200)))

    -- health
    local healthNode = root:child("health")
    if healthNode then
        w(string.format("health = %d", intAttr(healthNode, "now", 100)))
        w(string.format("maxHealth = %d", intAttr(healthNode, "max", 100)))
    end

    -- look
    local lookNode = root:child("look")
    if lookNode then
        local lookType = intAttr(lookNode, "type", 0)
        local lookTypeEx = intAttr(lookNode, "typeex", 0)
        local corpse = intAttr(lookNode, "corpse", 0)
        if lookType ~= 0 then w(string.format("lookType = %d", lookType)) end
        if lookTypeEx ~= 0 then w(string.format("lookTypeEx = %d", lookTypeEx)) end
        if corpse ~= 0 then w(string.format("lookCorpse = %d", corpse)) end
        local head = intAttr(lookNode, "head", 0)
        local body = intAttr(lookNode, "body", 0)
        local legs = intAttr(lookNode, "legs", 0)
        local feet = intAttr(lookNode, "feet", 0)
        local addons = intAttr(lookNode, "addons", 0)
        if head ~= 0 then w(string.format("lookHead = %d", head)) end
        if body ~= 0 then w(string.format("lookBody = %d", body)) end
        if legs ~= 0 then w(string.format("lookLegs = %d", legs)) end
        if feet ~= 0 then w(string.format("lookFeet = %d", feet)) end
        if addons ~= 0 then w(string.format("lookAddons = %d", addons)) end
    end

    -- targetchange
    local targetNode = root:child("targetchange")
    if targetNode then
        local ti = intAttr(targetNode, "interval", 0)
        local tc = intAttr(targetNode, "chance", 0)
        if ti ~= 0 then w(string.format("targetInterval = %d", ti)) end
        if tc ~= 0 then w(string.format("targetChance = %d", tc)) end
    end

    -- defenses (armor/defense values)
    local defensesNode = root:child("defenses")
    local armorVal = 0
    local defenseVal = 0
    if defensesNode then
        armorVal = intAttr(defensesNode, "armor", 0)
        defenseVal = intAttr(defensesNode, "defense", 0)
    end
    if armorVal ~= 0 then w(string.format("armor = %d", armorVal)) end
    if defenseVal ~= 0 then w(string.format("defense = %d", defenseVal)) end

    -- flags
    local flagsNode = root:child("flags")
    if flagsNode then
        local flags = {}
        for flagNode in flagsNode:children() do
            if flagNode:name() == "flag" then
                for k, v in pairs({
                    summonable = "summonable",
                    attackable = "attackable",
                    hostile = "hostile",
                    illusionable = "illusionable",
                    convinceable = "convinceable",
                    pushable = "pushable",
                    canpushitems = "canPushItems",
                    canpushcreatures = "canPushCreatures",
                    isboss = "isBoss",
                    isrewardboss = "isRewardBoss",
                    ischallengeable = "isChallengeable",
                    ignorespawnblock = "ignoreSpawnBlock",
                    hiddenhealth = "hiddenHealth",
                    canwalkonenergy = "canWalkOnEnergy",
                    canwalkonfire = "canWalkOnFire",
                    canwalkonpoison = "canWalkOnPoison",
                }) do
                    local val = flagNode:attribute(k)
                    if val ~= "" then
                        flags[v] = tomlBool(val)
                    end
                end
                -- targetdistance and staticattack are numeric
                local td = flagNode:attribute("targetdistance")
                if td ~= "" then flags["targetDistance"] = tonumber(td) or 1 end
                local sa = flagNode:attribute("staticattack")
                if sa ~= "" then flags["staticAttack"] = tonumber(sa) or 95 end
                local roh = flagNode:attribute("runonhealth")
                if roh ~= "" then flags["runOnHealth"] = tonumber(roh) or 0 end
            end
        end
        for k, v in pairs(flags) do
            if type(v) == "boolean" or type(v) == "string" then
                w(string.format("%s = %s", k, v))
            else
                w(string.format("%s = %d", k, v))
            end
        end
    end

    -- voices
    local voicesNode = root:child("voices")
    if voicesNode then
        local yellInterval = intAttr(voicesNode, "interval", 0)
        local yellChance = intAttr(voicesNode, "chance", 0)
        if yellInterval ~= 0 then w(string.format("yellInterval = %d", yellInterval)) end
        if yellChance ~= 0 then w(string.format("yellChance = %d", yellChance)) end

        local sentences = {}
        for voiceNode in voicesNode:children() do
            if voiceNode:name() == "voice" then
                local sentence = strAttr(voiceNode, "sentence", "")
                if sentence ~= "" then
                    table.insert(sentences, tomlString(sentence))
                end
            end
        end
        if #sentences > 0 then
            w("voices = [" .. table.concat(sentences, ", ") .. "]")
        end
    end

    -- immunities
    local combatImms = {}
    local condImms = {}
    local immNode = root:child("immunities")
    if immNode then
        for imm in immNode:children() do
            if imm:name() == "immunity" then
                for xmlKey, tomlKey in pairs(combatImmunityMap) do
                    local val = imm:attribute(xmlKey)
                    if val == "1" then
                        combatImms[tomlKey] = true
                    end
                end
                for xmlKey, tomlKey in pairs(conditionImmunityMap) do
                    local val = imm:attribute(xmlKey)
                    if val == "1" then
                        condImms[tomlKey] = true
                    end
                end
                -- Special: paralyze and outfit and drunk and invisible are usually condition-only
                if imm:attribute("paralyze") == "1" then condImms["paralyze"] = true end
                if imm:attribute("outfit") == "1" then condImms["outfit"] = true end
                if imm:attribute("drunk") == "1" then condImms["drunk"] = true end
                if imm:attribute("invisible") == "1" then condImms["invisible"] = true end
                if imm:attribute("drown") == "1" then combatImms["drown"] = true end
            end
        end
    end
    if next(combatImms) then
        local arr = {}
        for k in pairs(combatImms) do table.insert(arr, tomlString(k)) end
        w("combatImmunities = [" .. table.concat(arr, ", ") .. "]")
    end
    if next(condImms) then
        local arr = {}
        for k in pairs(condImms) do table.insert(arr, tomlString(k)) end
        w("conditionImmunities = [" .. table.concat(arr, ", ") .. "]")
    end

    -- elements
    local elementParts = {}
    local elemNode = root:child("elements")
    if elemNode then
        for elem in elemNode:children() do
            if elem:name() == "element" then
                for xmlAttr, tomlName in pairs(elementNameMap) do
                    local val = elem:attribute(xmlAttr)
                    if val ~= "" then
                        table.insert(elementParts,
                            string.format('{type = %s, percent = %s}', tomlString(tomlName), val))
                    end
                end
            end
        end
    end
    if #elementParts > 0 then
        w("elements = [")
        for _, ep in ipairs(elementParts) do
            w("    " .. ep .. ",")
        end
        w("]")
    end

    -- attacks
    local attackParts = {}
    local attacksNode = root:child("attacks")
    if attacksNode then
        for atk in attacksNode:children() do
            if atk:name() == "attack" then
                table.insert(attackParts, "    " .. convertSpell(atk) .. ",")
            end
        end
    end
    if #attackParts > 0 then
        w("attacks = [")
        for _, ap in ipairs(attackParts) do w(ap) end
        w("]")
    end

    -- defense spells (from <defenses> child nodes, NOT armor/defense values)
    local defSpellParts = {}
    if defensesNode then
        for defSpell in defensesNode:children() do
            if defSpell:name() == "defense" then
                table.insert(defSpellParts, "    " .. convertSpell(defSpell) .. ",")
            end
        end
    end
    if #defSpellParts > 0 then
        w("defenseSpells = [")
        for _, dp in ipairs(defSpellParts) do w(dp) end
        w("]")
    end

    -- summons
    local summonParts = {}
    local summonsNode = root:child("summons")
    local maxSummons = 0
    if summonsNode then
        maxSummons = intAttr(summonsNode, "maxSummons", 0)
        for summon in summonsNode:children() do
            if summon:name() == "summon" then
                local parts = {}
                local sname = strAttr(summon, "name", "")
                local schance = intAttr(summon, "chance", 100)
                local sinterval = intAttr(summon, "interval", 2000)
                local smax = intAttr(summon, "max", 1)
                local sforce = tomlBoolAttr(summon, "force", "false")
                table.insert(parts, string.format("name = %s", tomlString(sname)))
                table.insert(parts, string.format("chance = %d", schance))
                table.insert(parts, string.format("interval = %d", sinterval))
                if smax ~= 1 then table.insert(parts, string.format("max = %d", smax)) end
                if sforce == "true" then table.insert(parts, "force = true") end
                table.insert(summonParts, "    {" .. table.concat(parts, ", ") .. "},")
            end
        end
    end
    if maxSummons ~= 0 then w(string.format("maxSummons = %d", maxSummons)) end
    if #summonParts > 0 then
        w("summons = [")
        for _, sp in ipairs(summonParts) do w(sp) end
        w("]")
    end

    -- loot
    local lootParts = {}
    local lootNode = root:child("loot")
    if lootNode then
        for item in lootNode:children() do
            if item:name() == "item" then
                table.insert(lootParts, "    " .. convertLootItem(item) .. ",")
            end
        end
    end
    if #lootParts > 0 then
        w("loot = [")
        for _, lp in ipairs(lootParts) do w(lp) end
        w("]")
    end

    -- Write file
    local f = io.open(tomlPath, "w")
    if not f then
        return false, "Could not write: " .. tomlPath
    end
    f:write(table.concat(lines, "\n"))
    f:write("\n")
    f:close()

    return true, nil
end

local function onSay(player, words, param)
    if not player:hasFlag(PlayerFlag_GamemasterPrivileges) then
        return true
    end

    local monsterDir = "data/monster/monsters"
    local count = 0
    local errors = 0

    -- Iterate all .xml files in the monsters directory
    -- We use io.popen for directory listing since Lua doesn't have built-in dir traversal
    local pipe = io.popen('find "' .. monsterDir .. '" -name "*.xml" 2>/dev/null || dir /b /s "' .. monsterDir .. '\\*.xml" 2>nul')
    if not pipe then
        player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "Could not list monster directory.")
        return false
    end

    for xmlPath in pipe:lines() do
        xmlPath = xmlPath:gsub("\\", "/"):gsub("%s+$", "")
        if xmlPath:match("%.xml$") and not xmlPath:match("monsters%.xml$") then
            local tomlPath = xmlPath:gsub("%.xml$", ".toml")
            local ok, err = convertMonsterXmlToToml(xmlPath, tomlPath)
            if ok then
                count = count + 1
            else
                errors = errors + 1
                print("[convert_monsters] Error: " .. (err or "unknown") .. " for " .. xmlPath)
            end
        end
    end
    pipe:close()

    local msg = string.format("Converted %d monster XML files to TOML. Errors: %d", count, errors)
    player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, msg)
    print("[convert_monsters] " .. msg)
    return false
end

local convertMonsters = TalkAction("/convertmonsters")
function convertMonsters.onSay(player, words, param)
    return onSay(player, words, param)
end
convertMonsters:separator(" ")
convertMonsters:register()
