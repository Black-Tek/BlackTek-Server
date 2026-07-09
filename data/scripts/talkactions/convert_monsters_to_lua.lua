-- Converts all XML monster definitions to Lua files using the
-- MonsterType::register API (data/scripts/lib/register_monster_type.lua).
-- Usage: /convertlua
-- Output: data/scripts/monsters/monsters/<stem>.lua
--
-- XMLNode API (from src/luascript.cpp):
--   XMLDocument(path)         -> XMLDocument or nil
--   doc:child(tagName)        -> XMLNode or nil  (first child with that tag)
--   node:firstChild()         -> XMLNode or nil
--   node:nextSibling()        -> XMLNode or nil
--   node:name()               -> string
--   node:attribute(key)       -> string or nil

-- ─────────────────────────────────────────────────────────────────────────────
-- Constant maps (sourced from src/tools.cpp and src/luascript.cpp)
-- ─────────────────────────────────────────────────────────────────────────────

local MAGIC_EFFECT_MAP = {
    redspark           = "CONST_ME_DRAWBLOOD",
    bluebubble         = "CONST_ME_LOSEENERGY",
    poff               = "CONST_ME_POFF",
    yellowspark        = "CONST_ME_BLOCKHIT",
    explosionarea      = "CONST_ME_EXPLOSIONAREA",
    explosion          = "CONST_ME_EXPLOSIONHIT",
    firearea           = "CONST_ME_FIREAREA",
    yellowbubble       = "CONST_ME_YELLOW_RINGS",
    greenbubble        = "CONST_ME_GREEN_RINGS",
    blackspark         = "CONST_ME_HITAREA",
    teleport           = "CONST_ME_TELEPORT",
    energy             = "CONST_ME_ENERGYHIT",
    blueshimmer        = "CONST_ME_MAGIC_BLUE",
    redshimmer         = "CONST_ME_MAGIC_RED",
    greenshimmer       = "CONST_ME_MAGIC_GREEN",
    fire               = "CONST_ME_HITBYFIRE",
    greenspark         = "CONST_ME_HITBYPOISON",
    mortarea           = "CONST_ME_MORTAREA",
    greennote          = "CONST_ME_SOUND_GREEN",
    rednote            = "CONST_ME_SOUND_RED",
    poison             = "CONST_ME_POISONAREA",
    yellownote         = "CONST_ME_SOUND_YELLOW",
    purplenote         = "CONST_ME_SOUND_PURPLE",
    bluenote           = "CONST_ME_SOUND_BLUE",
    whitenote          = "CONST_ME_SOUND_WHITE",
    bubbles            = "CONST_ME_BUBBLES",
    dice               = "CONST_ME_CRAPS",
    giftwraps          = "CONST_ME_GIFT_WRAPS",
    yellowfirework     = "CONST_ME_FIREWORK_YELLOW",
    redfirework        = "CONST_ME_FIREWORK_RED",
    bluefirework       = "CONST_ME_FIREWORK_BLUE",
    stun               = "CONST_ME_STUN",
    sleep              = "CONST_ME_SLEEP",
    watercreature      = "CONST_ME_WATERCREATURE",
    groundshaker       = "CONST_ME_GROUNDSHAKER",
    hearts             = "CONST_ME_HEARTS",
    fireattack         = "CONST_ME_FIREATTACK",
    energyarea         = "CONST_ME_ENERGYAREA",
    smallclouds        = "CONST_ME_SMALLCLOUDS",
    holydamage         = "CONST_ME_HOLYDAMAGE",
    bigclouds          = "CONST_ME_BIGCLOUDS",
    icearea            = "CONST_ME_ICEAREA",
    icetornado         = "CONST_ME_ICETORNADO",
    iceattack          = "CONST_ME_ICEATTACK",
    stones             = "CONST_ME_STONES",
    smallplants        = "CONST_ME_SMALLPLANTS",
    carniphila         = "CONST_ME_CARNIPHILA",
    purpleenergy       = "CONST_ME_PURPLEENERGY",
    yellowenergy       = "CONST_ME_YELLOWENERGY",
    holyarea           = "CONST_ME_HOLYAREA",
    bigplants          = "CONST_ME_BIGPLANTS",
    cake               = "CONST_ME_CAKE",
    giantice           = "CONST_ME_GIANTICE",
    watersplash        = "CONST_ME_WATERSPLASH",
    plantattack        = "CONST_ME_PLANTATTACK",
    tutorialarrow      = "CONST_ME_TUTORIALARROW",
    tutorialsquare     = "CONST_ME_TUTORIALSQUARE",
    mirrorhorizontal   = "CONST_ME_MIRRORHORIZONTAL",
    mirrorvertical     = "CONST_ME_MIRRORVERTICAL",
    skullhorizontal    = "CONST_ME_SKULLHORIZONTAL",
    skullvertical      = "CONST_ME_SKULLVERTICAL",
    assassin           = "CONST_ME_ASSASSIN",
    stepshorizontal    = "CONST_ME_STEPSHORIZONTAL",
    bloodysteps        = "CONST_ME_BLOODYSTEPS",
    stepsvertical      = "CONST_ME_STEPSVERTICAL",
    yalaharighost      = "CONST_ME_YALAHARIGHOST",
    bats               = "CONST_ME_BATS",
    smoke              = "CONST_ME_SMOKE",
    insects            = "CONST_ME_INSECTS",
    dragonhead         = "CONST_ME_DRAGONHEAD",
    orcshaman          = "CONST_ME_ORCSHAMAN",
    orcshamanfire      = "CONST_ME_ORCSHAMAN_FIRE",
    thunder            = "CONST_ME_THUNDER",
    ferumbras          = "CONST_ME_FERUMBRAS",
    confettihorizontal = "CONST_ME_CONFETTI_HORIZONTAL",
    confettivertical   = "CONST_ME_CONFETTI_VERTICAL",
    blacksmoke         = "CONST_ME_BLACKSMOKE",
    redsmoke           = "CONST_ME_REDSMOKE",
    yellowsmoke        = "CONST_ME_YELLOWSMOKE",
    greensmoke         = "CONST_ME_GREENSMOKE",
    purplesmoke        = "CONST_ME_PURPLESMOKE",
}

local SHOOT_TYPE_MAP = {
    spear            = "CONST_ANI_SPEAR",
    bolt             = "CONST_ANI_BOLT",
    arrow            = "CONST_ANI_ARROW",
    fire             = "CONST_ANI_FIRE",
    energy           = "CONST_ANI_ENERGY",
    poisonarrow      = "CONST_ANI_POISONARROW",
    burstarrow       = "CONST_ANI_BURSTARROW",
    throwingstar     = "CONST_ANI_THROWINGSTAR",
    throwingknife    = "CONST_ANI_THROWINGKNIFE",
    smallstone       = "CONST_ANI_SMALLSTONE",
    death            = "CONST_ANI_DEATH",
    largerock        = "CONST_ANI_LARGEROCK",
    snowball         = "CONST_ANI_SNOWBALL",
    powerbolt        = "CONST_ANI_POWERBOLT",
    poison           = "CONST_ANI_POISON",
    infernalbolt     = "CONST_ANI_INFERNALBOLT",
    huntingspear     = "CONST_ANI_HUNTINGSPEAR",
    enchantedspear   = "CONST_ANI_ENCHANTEDSPEAR",
    redstar          = "CONST_ANI_REDSTAR",
    greenstar        = "CONST_ANI_GREENSTAR",
    royalspear       = "CONST_ANI_ROYALSPEAR",
    sniperarrow      = "CONST_ANI_SNIPERARROW",
    onyxarrow        = "CONST_ANI_ONYXARROW",
    piercingbolt     = "CONST_ANI_PIERCINGBOLT",
    whirlwindsword   = "CONST_ANI_WHIRLWINDSWORD",
    whirlwindaxe     = "CONST_ANI_WHIRLWINDAXE",
    whirlwindclub    = "CONST_ANI_WHIRLWINDCLUB",
    etherealspear    = "CONST_ANI_ETHEREALSPEAR",
    ice              = "CONST_ANI_ICE",
    earth            = "CONST_ANI_EARTH",
    holy             = "CONST_ANI_HOLY",
    suddendeath      = "CONST_ANI_SUDDENDEATH",
    flasharrow       = "CONST_ANI_FLASHARROW",
    flammingarrow    = "CONST_ANI_FLAMMINGARROW",
    shiverarrow      = "CONST_ANI_SHIVERARROW",
    energyball       = "CONST_ANI_ENERGYBALL",
    smallice         = "CONST_ANI_SMALLICE",
    smallholy        = "CONST_ANI_SMALLHOLY",
    smallearth       = "CONST_ANI_SMALLEARTH",
    eartharrow       = "CONST_ANI_EARTHARROW",
    explosion        = "CONST_ANI_EXPLOSION",
    cake             = "CONST_ANI_CAKE",
    tarsalarrow      = "CONST_ANI_TARSALARROW",
    vortexbolt       = "CONST_ANI_VORTEXBOLT",
    prismaticbolt    = "CONST_ANI_PRISMATICBOLT",
    crystallinearrow = "CONST_ANI_CRYSTALLINEARROW",
    drillbolt        = "CONST_ANI_DRILLBOLT",
    envenomedarrow   = "CONST_ANI_ENVENOMEDARROW",
    gloothspear      = "CONST_ANI_GLOOTHSPEAR",
    simplearrow      = "CONST_ANI_SIMPLEARROW",
}

-- XML <element XxxPercent="N" /> attribute (exact casing as in XML) → COMBAT_*
local ELEMENT_ATTR_MAP = {
    physicalPercent  = "COMBAT_PHYSICALDAMAGE",
    energyPercent    = "COMBAT_ENERGYDAMAGE",
    firePercent      = "COMBAT_FIREDAMAGE",
    earthPercent     = "COMBAT_EARTHDAMAGE",
    icePercent       = "COMBAT_ICEDAMAGE",
    holyPercent      = "COMBAT_HOLYDAMAGE",
    deathPercent     = "COMBAT_DEATHDAMAGE",
    drownPercent     = "COMBAT_DROWNDAMAGE",
    lifedrainPercent = "COMBAT_LIFEDRAIN",
    manadrainPercent = "COMBAT_MANADRAIN",
    undefinedPercent = "COMBAT_UNDEFINEDDAMAGE",
}

-- {combat_immune, condition_immune}
local IMMUNITY_FLAGS = {
    physical     = {true,  true},
    energy       = {true,  true},
    fire         = {true,  true},
    poison       = {true,  true},
    earth        = {true,  true},
    drown        = {true,  true},
    ice          = {true,  true},
    holy         = {true,  true},
    death        = {true,  true},
    lifedrain    = {true,  false},
    manadrain    = {true,  false},
    paralyze     = {false, true},
    outfit       = {false, true},
    drunk        = {false, true},
    invisible    = {false, true},
    invisibility = {false, true},
    bleed        = {false, true},
}

local MELEE_CONDITION_ATTRS = {
    poison   = "CONDITION_POISON",
    fire     = "CONDITION_FIRE",
    energy   = "CONDITION_ENERGY",
    bleed    = "CONDITION_BLEEDING",
    drown    = "CONDITION_DROWN",
    freezing = "CONDITION_FREEZING",
    cursed   = "CONDITION_CURSED",
    dazzled  = "CONDITION_DAZZLED",
}

-- ─────────────────────────────────────────────────────────────────────────────
-- XMLNode traversal helpers
-- (XMLNode only exposes: attribute, name, firstChild, nextSibling)
-- ─────────────────────────────────────────────────────────────────────────────

-- Returns the integer value of an attribute, or default if absent/non-numeric.
local function intAttr(node, key, default)
    local v = node:attribute(key)
    if v == nil then return default end
    return tonumber(v) or default
end

-- Returns the string value of an attribute, or default if absent or empty.
local function strAttr(node, key, default)
    local v = node:attribute(key)
    if v == nil or v == "" then return default end
    return v
end

-- Finds the first child element whose tag matches tagName.
local function findChild(node, tagName)
    local child = node:firstChild()
    while child do
        if child:name() == tagName then return child end
        child = child:nextSibling()
    end
    return nil
end

-- Collects all child elements whose tag matches tagName into an array.
local function findChildren(node, tagName)
    local result = {}
    local child = node:firstChild()
    while child do
        if child:name() == tagName then
            result[#result + 1] = child
        end
        child = child:nextSibling()
    end
    return result
end

-- Finds the value of <attribute key="K" value="V" /> child element.
local function getAttrChild(node, key)
    local child = node:firstChild()
    while child do
        if child:name() == "attribute" then
            local k = child:attribute("key")
            if k == key then
                return child:attribute("value") or ""
            end
        end
        child = child:nextSibling()
    end
    return ""
end

-- ─────────────────────────────────────────────────────────────────────────────
-- Lua output helpers
-- ─────────────────────────────────────────────────────────────────────────────

local function luaStr(s)
    return '"' .. tostring(s):gsub('\\', '\\\\'):gsub('"', '\\"') .. '"'
end

-- ─────────────────────────────────────────────────────────────────────────────
-- Spell (attack / defense) renderer
-- ─────────────────────────────────────────────────────────────────────────────

local function renderSpell(node, baseIndent)
    local bi = string.rep("    ", baseIndent)
    local ii = string.rep("    ", baseIndent + 1)
    local L  = {"{"}

    local function field(key, val)
        L[#L + 1] = ii .. key .. " = " .. tostring(val) .. ","
    end

    local name = strAttr(node, "name", "")
    if name ~= "" then field("name", luaStr(name)) end

    local interval = intAttr(node, "interval", nil)
    if interval then field("interval", interval) end

    local chance = intAttr(node, "chance", nil)
    if chance then field("chance", chance) end

    local range = intAttr(node, "range", 0)
    if range ~= 0 then field("range", range) end

    local minV = node:attribute("min")
    local maxV = node:attribute("max")
    if minV and maxV then
        field("minDamage", tonumber(minV) or 0)
        field("maxDamage", tonumber(maxV) or 0)
    end

    local radius = intAttr(node, "radius", 0)
    if radius ~= 0 then field("radius", radius) end

    local length = intAttr(node, "length", 0)
    if length ~= 0 then field("length", length) end

    local spread = intAttr(node, "spread", 0)
    if spread ~= 0 then field("spread", spread) end

    local ring = intAttr(node, "ring", 0)
    if ring ~= 0 then field("ring", ring) end

    local targetRaw = node:attribute("target")
    if targetRaw then
        field("target", (targetRaw ~= "0") and "true" or "false")
    end

    local speedChange = intAttr(node, "speedchange", 0)
    if speedChange ~= 0 then field("speed", speedChange) end

    local duration = intAttr(node, "duration", 0)
    if duration ~= 0 then field("duration", duration) end

    local attack = intAttr(node, "attack", 0)
    if attack ~= 0 then field("attack", attack) end

    local skill = intAttr(node, "skill", 0)
    if skill ~= 0 then field("skill", skill) end

    local shootRaw = getAttrChild(node, "shootEffect")
    if shootRaw ~= "" then
        local c = SHOOT_TYPE_MAP[shootRaw:lower()]
        field("shootEffect", c or luaStr(shootRaw))
        if not c then L[#L + 1] = ii .. "-- WARN: unknown shootEffect '" .. shootRaw .. "'" end
    end

    local areaRaw = getAttrChild(node, "areaEffect")
    if areaRaw ~= "" then
        local c = MAGIC_EFFECT_MAP[areaRaw:lower()]
        field("effect", c or luaStr(areaRaw))
        if not c then L[#L + 1] = ii .. "-- WARN: unknown areaEffect '" .. areaRaw .. "'" end
    end

    -- Melee condition shorthands: poison="N", fire="N", etc.
    if name == "melee" then
        for condKey, condType in pairs(MELEE_CONDITION_ATTRS) do
            local condVal = intAttr(node, condKey, 0)
            if condVal ~= 0 then
                L[#L + 1] = ii .. "-- NOTE: melee condition (" .. condKey .. "=" .. condVal .. "); verify damage/duration"
                L[#L + 1] = ii .. "condition = {"
                L[#L + 1] = ii .. "    type = " .. condType .. ","
                L[#L + 1] = ii .. "    interval = " .. (condVal * 1000) .. ","
                L[#L + 1] = ii .. "},"
                break
            end
        end
    end

    L[#L + 1] = bi .. "}"
    return table.concat(L, "\n")
end

-- ─────────────────────────────────────────────────────────────────────────────
-- Loot item renderer (recursive for container children)
-- ─────────────────────────────────────────────────────────────────────────────

local function renderLoot(node, baseIndent)
    local bi = string.rep("    ", baseIndent)
    local ii = string.rep("    ", baseIndent + 1)
    local L  = {"{"}

    local itemId   = node:attribute("id")
    local itemName = strAttr(node, "name", "")

    if itemId and tonumber(itemId) then
        L[#L + 1] = ii .. "id = " .. tonumber(itemId) .. ","
    elseif itemName ~= "" then
        L[#L + 1] = ii .. "id = " .. luaStr(itemName) .. ","
    end

    local chance = intAttr(node, "chance", nil)
    if chance then L[#L + 1] = ii .. "chance = " .. chance .. "," end

    local countmax = intAttr(node, "countmax", 1)
    if countmax > 1 then L[#L + 1] = ii .. "maxCount = " .. countmax .. "," end

    local countmin = intAttr(node, "countmin", 1)
    if countmin > 1 then L[#L + 1] = ii .. "minCount = " .. countmin .. "," end

    local subtype = intAttr(node, "subtype", 0)
    if subtype ~= 0 then L[#L + 1] = ii .. "subType = " .. subtype .. "," end

    local actionid = intAttr(node, "actionid", 0)
    if actionid ~= 0 then L[#L + 1] = ii .. "actionId = " .. actionid .. "," end

    local text = strAttr(node, "text", "") ~= "" and strAttr(node, "text", "")
              or strAttr(node, "description", "")
    if text and text ~= "" then L[#L + 1] = ii .. "text = " .. luaStr(text) .. "," end

    local children = findChildren(node, "item")
    if #children > 0 then
        local ci = string.rep("    ", baseIndent + 2)
        L[#L + 1] = ii .. "child = {"
        for _, child in ipairs(children) do
            L[#L + 1] = ci .. renderLoot(child, baseIndent + 2) .. ","
        end
        L[#L + 1] = ii .. "},"
    end

    L[#L + 1] = bi .. "}"
    return table.concat(L, "\n")
end

-- ─────────────────────────────────────────────────────────────────────────────
-- Core converter: one XMLDocument → Lua source string written to luaPath
-- ─────────────────────────────────────────────────────────────────────────────

local function convertMonster(xmlPath, luaPath)
    local doc = XMLDocument(xmlPath)
    if not doc then
        return false, "Cannot open " .. xmlPath
    end

    -- doc:child() is the only navigation method on XMLDocument;
    -- it returns an XMLNode for the first child with that tag name.
    local root = doc:child("monster")
    if not root then
        return false, "No <monster> root in " .. xmlPath
    end

    local lines = {}
    local function w(s) lines[#lines + 1] = s end

    -- Top-level attributes
    local monName  = strAttr(root, "name",            "Unknown")
    local monDesc  = strAttr(root, "nameDescription", "")
    local monRace  = strAttr(root, "race",            "blood")
    local monExp   = intAttr(root, "experience",      0)
    local monSpeed = intAttr(root, "speed",           200)
    local monMana  = intAttr(root, "manacost",        0)

    w('local mtype = Game.createMonsterType(' .. luaStr(monName) .. ')')
    w('local monster = {}')
    w('')
    w('monster.name = '       .. luaStr(monName))
    if monDesc ~= "" then
        w('monster.description = ' .. luaStr(monDesc))
    end
    w('monster.experience = ' .. monExp)
    w('monster.race = '       .. luaStr(monRace))

    -- Health
    local healthNode = findChild(root, "health")
    if healthNode then
        local hMax = intAttr(healthNode, "max", intAttr(healthNode, "now", 100))
        local hNow = intAttr(healthNode, "now", hMax)
        w('monster.maxHealth = ' .. hMax)
        w('monster.health = '    .. hNow)
    end

    w('monster.speed = '    .. monSpeed)
    w('monster.manaCost = ' .. monMana)

    -- Look / Outfit
    local lookNode = findChild(root, "look")
    if lookNode then
        local corpse = intAttr(lookNode, "corpse", 0)
        if corpse ~= 0 then w('monster.corpse = ' .. corpse) end

        local oParts = {}
        local function oPush(luaKey, xmlKey)
            local v = lookNode:attribute(xmlKey)
            if v then oParts[#oParts + 1] = luaKey .. " = " .. (tonumber(v) or 0) end
        end
        oPush("lookType",   "type")
        oPush("lookTypeEx", "typeex")
        oPush("lookHead",   "head")
        oPush("lookBody",   "body")
        oPush("lookLegs",   "legs")
        oPush("lookFeet",   "feet")
        oPush("lookAddons", "addons")
        oPush("lookMount",  "mount")

        if #oParts == 1 then
            w('monster.outfit = { ' .. oParts[1] .. ' }')
        elseif #oParts > 1 then
            w('monster.outfit = {')
            for _, p in ipairs(oParts) do w('    ' .. p .. ',') end
            w('}')
        end
    end

    -- Light
    local lightNode = findChild(root, "light")
    if lightNode then
        w('monster.light = { level = ' .. intAttr(lightNode, "level", 0)
          .. ', color = ' .. intAttr(lightNode, "color", 0) .. ' }')
    end

    -- Flags (runonhealth / targetdistance / staticattack live as flags in XML)
    local runHealth  = 0
    local targetDist = 1
    local staticAtk  = 100
    local flagOut    = {}

    local flagsNode = findChild(root, "flags")
    if flagsNode then
        for _, flagNode in ipairs(findChildren(flagsNode, "flag")) do
            local function bF(xmlK, luaK)
                local v = flagNode:attribute(xmlK)
                if v then flagOut[luaK] = (v ~= "0" and v ~= "false") end
            end
            local roh = flagNode:attribute("runonhealth")
            if roh then runHealth  = tonumber(roh) or 0  end
            local td  = flagNode:attribute("targetdistance")
            if td  then targetDist = tonumber(td)  or 1  end
            local sa  = flagNode:attribute("staticattack")
            if sa  then staticAtk  = tonumber(sa)  or 100 end

            bF("summonable",       "summonable")
            bF("attackable",       "attackable")
            bF("hostile",          "hostile")
            bF("convinceable",     "convinceable")
            bF("challengeable",    "challengeable")
            bF("illusionable",     "illusionable")
            bF("pushable",         "pushable")
            bF("canpushitems",     "canPushItems")
            bF("canpushcreatures", "canPushCreatures")
            bF("canwalkonenergy",  "canWalkOnEnergy")
            bF("canwalkonfire",    "canWalkOnFire")
            bF("canwalkonpoison",  "canWalkOnPoison")
            bF("isboss",           "boss")
            bF("healthhidden",     "healthHidden")
            bF("ignorespawnblock", "ignoreSpawnBlock")
        end
    end

    if runHealth > 0 then w('monster.runHealth = ' .. runHealth) end

    -- Target change
    local tcNode = findChild(root, "targetchange")
    if tcNode then
        w('monster.changeTarget = {')
        w('    interval = ' .. intAttr(tcNode, "interval", 4000) .. ',')
        w('    chance = '   .. intAttr(tcNode, "chance",   0)    .. ',')
        w('}')
    end

    w('monster.targetDistance = '     .. targetDist)
    w('monster.staticAttackChance = ' .. staticAtk)

    -- Flags block
    local FLAG_ORDER = {
        "summonable", "attackable", "hostile", "illusionable", "convinceable",
        "challengeable", "pushable", "canPushItems", "canPushCreatures",
        "canWalkOnEnergy", "canWalkOnFire", "canWalkOnPoison",
        "boss", "healthHidden", "ignoreSpawnBlock",
    }
    local hasFlags = false
    for _, k in ipairs(FLAG_ORDER) do if flagOut[k] ~= nil then hasFlags = true; break end end
    if hasFlags then
        w('monster.flags = {')
        for _, k in ipairs(FLAG_ORDER) do
            if flagOut[k] ~= nil then
                w('    ' .. k .. ' = ' .. (flagOut[k] and "true" or "false") .. ',')
            end
        end
        w('}')
    end

    -- Attacks
    local attacksNode = findChild(root, "attacks")
    if attacksNode then
        local atks = findChildren(attacksNode, "attack")
        if #atks > 0 then
            w('monster.attacks = {')
            for _, atk in ipairs(atks) do
                w('    ' .. renderSpell(atk, 1) .. ',')
            end
            w('}')
        end
    end

    -- Defenses
    local defensesNode = findChild(root, "defenses")
    if defensesNode then
        local defVal   = defensesNode:attribute("defense")
        local armorVal = defensesNode:attribute("armor")
        local defSpells = findChildren(defensesNode, "defense")

        if defVal or armorVal or #defSpells > 0 then
            w('monster.defenses = {')
            if defVal   then w('    defense = ' .. (tonumber(defVal)   or 0) .. ',') end
            if armorVal then w('    armor = '   .. (tonumber(armorVal) or 0) .. ',') end
            for _, def in ipairs(defSpells) do
                w('    ' .. renderSpell(def, 1) .. ',')
            end
            w('}')
        end
    end

    -- Elements
    local elementsNode = findChild(root, "elements")
    if elementsNode then
        local elems = {}
        for _, elemNode in ipairs(findChildren(elementsNode, "element")) do
            for xmlAttr, combatConst in pairs(ELEMENT_ATTR_MAP) do
                local val = elemNode:attribute(xmlAttr)
                if val then
                    elems[#elems + 1] = {combatConst, tonumber(val) or 0}
                    break
                end
            end
        end
        if #elems > 0 then
            w('monster.elements = {')
            for _, el in ipairs(elems) do
                w('    {type = ' .. el[1] .. ', percent = ' .. el[2] .. '},')
            end
            w('}')
        end
    end

    -- Immunities
    local immunitiesNode = findChild(root, "immunities")
    if immunitiesNode then
        local imms = {}
        for _, immNode in ipairs(findChildren(immunitiesNode, "immunity")) do
            for immName, fl in pairs(IMMUNITY_FLAGS) do
                local v = immNode:attribute(immName)
                if v == "1" or v == "true" then
                    imms[#imms + 1] = {immName, fl[1], fl[2]}
                end
            end
        end
        if #imms > 0 then
            w('monster.immunities = {')
            for _, imm in ipairs(imms) do
                local parts = {'type = ' .. luaStr(imm[1])}
                if imm[2] then parts[#parts + 1] = 'combat = true'    end
                if imm[3] then parts[#parts + 1] = 'condition = true' end
                w('    {' .. table.concat(parts, ', ') .. '},')
            end
            w('}')
        end
    end

    -- Summons
    local summonsNode = findChild(root, "summons")
    if summonsNode then
        local maxS = intAttr(summonsNode, "maxSummons", 0)
        if maxS ~= 0 then w('monster.maxSummons = ' .. maxS) end

        local summonList = findChildren(summonsNode, "summon")
        if #summonList > 0 then
            w('monster.summons = {')
            for _, s in ipairs(summonList) do
                w(string.format('    {name = %s, interval = %d, chance = %d, max = %d},',
                    luaStr(strAttr(s, "name", "")),
                    intAttr(s, "interval", 2000),
                    intAttr(s, "chance",   100),
                    intAttr(s, "max",      0)))
            end
            w('}')
        end
    end

    -- Voices
    local voicesNode = findChild(root, "voices")
    if voicesNode then
        local voiceList = findChildren(voicesNode, "voice")
        if #voiceList > 0 then
            w('monster.voices = {')
            w('    interval = ' .. intAttr(voicesNode, "interval", 5000) .. ',')
            w('    chance = '   .. intAttr(voicesNode, "chance",   10)   .. ',')
            for _, v in ipairs(voiceList) do
                local sentence = strAttr(v, "sentence", "")
                local yell     = (v:attribute("yell") == "1") and "true" or "false"
                w('    {text = ' .. luaStr(sentence) .. ', yell = ' .. yell .. '},')
            end
            w('}')
        end
    end

    -- Loot
    local lootNode = findChild(root, "loot")
    if lootNode then
        local lootItems = findChildren(lootNode, "item")
        if #lootItems > 0 then
            w('monster.loot = {')
            for _, item in ipairs(lootItems) do
                w('    ' .. renderLoot(item, 1) .. ',')
            end
            w('}')
        end
    end

    w('')
    w('mtype:register(monster)')

    local f = io.open(luaPath, "w")
    if not f then return false, "Cannot write " .. luaPath end
    f:write(table.concat(lines, "\n") .. "\n")
    f:close()
    return true, nil
end

-- ─────────────────────────────────────────────────────────────────────────────
-- Talkaction handler
-- ─────────────────────────────────────────────────────────────────────────────

local function runConversion(player)
    local outDir = "data/scripts/monsters/monsters"
    os.execute('mkdir -p "' .. outDir .. '" 2>/dev/null')
    os.execute('mkdir "' .. outDir:gsub("/", "\\") .. '" 2>nul')

    -- Read master list line-by-line (avoids XMLDocument on the index file)
    local mf = io.open("data/monster/monsters.xml", "r")
    if not mf then
        player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "[convertlua] Cannot open data/monster/monsters.xml")
        return
    end
    local files = {}
    for line in mf:lines() do
        local relFile = line:match('file="([^"]+)"')
        if relFile and relFile:match("%.xml$") then
            files[#files + 1] = relFile
        end
    end
    mf:close()

    local converted, skipped = 0, 0
    for _, relFile in ipairs(files) do
        local stem = relFile:match("([^/\\]+)%.xml$")
        if stem then
            local ok, err = convertMonster(
                "data/monster/" .. relFile,
                outDir .. "/" .. stem .. ".lua"
            )
            if ok then
                converted = converted + 1
            else
                skipped = skipped + 1
                print("[convertlua] " .. relFile .. ": " .. (err or "?"))
            end
        end
    end

    local msg = string.format("[convertlua] Done: %d converted, %d skipped -> %s",
        converted, skipped, outDir)
    player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, msg)
    print(msg)
end

local function onSay(player, words, param)
    if not player:getGroup():getAccess() then
        return true
    end
    if player:getAccountType() < ACCOUNT_TYPE_GOD then
        player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, "[convertlua] Requires GOD account.")
        return false
    end
    local ok, err = pcall(runConversion, player)
    if not ok then
        local msg = "[convertlua] Lua error: " .. tostring(err)
        player:sendTextMessage(MESSAGE_STATUS_CONSOLE_BLUE, msg)
        print(msg)
    end
    return false
end

local convertLua = TalkAction("/convertlua")
function convertLua.onSay(player, words, param)
    return onSay(player, words, param)
end
convertLua:separator(" ")
convertLua:register()
