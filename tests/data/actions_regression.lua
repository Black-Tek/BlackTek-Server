-- Run from the repository root: lua tests/data/actions_regression.lua
-- These tests execute the production callbacks with isolated game API fakes.
local passed, failed = 0, 0
local function expect(value, message)
    assert(value, message or "expectation failed")
end
local function equal(actual, expected)
    assert(actual == expected, tostring(actual) .. " ~= " .. tostring(expected))
end
local function test(name, fn)
    local ok, err = pcall(fn)
    if ok then
        passed = passed + 1
        print("PASS " .. name)
    else
        failed = failed + 1
        print("FAIL " .. name .. ": " .. tostring(err))
    end
end
local function noop() end
local function position()
    return {x = 100, y = 100, z = 7, sendMagicEffect = noop,
        moveUpstairs = function(self) self.y = self.y + 1; self.z = self.z - 1; return self end}
end
local function item(id)
    local obj = {itemid = id, removed = 0, pos = position()}
    function obj:getId() return self.itemid end
    function obj:getPosition() return self.pos end
    function obj:isItem() return true end
    function obj:isPlayer() return false end
    function obj:isContainer() return false end
    function obj:transform(newId) self.itemid = newId; return true end
    function obj:remove(count) self.removed = self.removed + (count or 1); return true end
    function obj:decay() self.decayed = true end
    function obj:setAttribute(key, value) self[key] = value end
    function obj:hasAttribute() return false end
    function obj:getType() return {usesSlot = function() return false end, getDecayId = function() return 0 end} end
    return obj
end
local function player()
    local obj = {storage = {}, rewards = {}, achievements = {}, level = 200, vocation = 4, premium = 0, access = true,
        pos = position(), cancellations = 0, paid = 0, messages = {}, mounts = {}}
    function obj:isPlayer() return true end
    function obj:isContainer() return false end
    function obj:isPremium() return true end
    function obj:getPosition() return self.pos end
    function obj:getLevel() return self.level end
    function obj:getName() return "Test Player" end
    function obj:getVocation() return {getId = function() return self.vocation end} end
    function obj:getStorageValue(key) expect(type(key) == "number", "invalid storage key"); local v = self.storage[key]; return v == nil and -1 or v end
    function obj:setStorageValue(key, value) expect(type(key) == "number", "invalid storage key"); self.storage[key] = value end
    function obj:addItem(id, count) local reward = item(id); self.rewards[#self.rewards + 1] = {id = id, count = count or 1, item = reward}; return reward end
    function obj:addAchievement(name) self.achievements[name] = true end
    function obj:addAchievementProgress() end
    function obj:say() end
    function obj:sendCancelMessage() self.cancellations = self.cancellations + 1 end
    function obj:sendTextMessage(...) self.messages[#self.messages + 1] = {...} end
    function obj:addMount(id) self.mounts[id] = true end
    function obj:hasMount(id) return self.mounts[id] == true end
    function obj:getGroup() return {getAccess = function() return self.access end} end
    function obj:getPremiumDays() return self.premium end
    function obj:addPremiumDays(days) self.premium = self.premium + days end
    function obj:removeTotalMoney(amount) self.paid = self.paid + amount; return true end
    return obj
end
local function loadScript(env, path)
    local chunk, err
    if setfenv then
        chunk, err = loadfile(path)
        if chunk then setfenv(chunk, env) end
    else
        chunk, err = loadfile(path, "t", env)
    end
    assert(chunk, err)
    chunk()
end
local function environment()
    local env = setmetatable({actions = {}, talks = {}}, {__index = _G})
    env._G = env
    env.math = setmetatable({random = function() return 1 end}, {__index = math})
    env.table = setmetatable({contains = function(values, value)
        for _, v in pairs(values) do if v == value then return true end end
        return false
    end}, {__index = table})
    local constants = {
        "CONST_ME_GREEN_RINGS", "CONST_ME_POFF", "CONST_ME_HITAREA", "CONST_ME_MAGIC_GREEN", "CONST_ME_GIFT_WRAPS",
        "CONST_ME_MAGIC_BLUE", "CONST_ME_MAGIC_RED", "CONST_ME_BLOCKHIT", "CONST_ME_DRAWBLOOD", "CONST_ME_THUNDER",
        "CONST_ME_SOUND_GREEN", "CONST_ME_SOUND_YELLOW", "CONST_ME_SOUND_PURPLE", "CONST_ME_SOUND_BLUE", "CONST_ME_SOUND_RED", "CONST_ME_SOUND_WHITE",
        "TALKTYPE_MONSTER_SAY", "MESSAGE_EVENT_ADVANCE", "MESSAGE_INFO_DESCR", "MESSAGE_STATUS_CONSOLE_BLUE", "MESSAGE_EXPERIENCE", "TEXTCOLOR_WHITE_EXP",
        "CONDITION_ATTRIBUTES", "CONDITION_PARAM_TICKS", "CONDITION_PARAM_SKILL_MELEE", "CONDITION_PARAM_SKILL_SHIELD", "CONDITION_PARAM_BUFF_SPELL",
        "CONDITION_PARAM_STAT_MAGICPOINTS", "CONDITION_PARAM_SKILL_DISTANCE", "COMBAT_HEALING", "COMBAT_MANADRAIN", "COMBAT_FIREDAMAGE", "COMBAT_ICEDAMAGE",
        "COMBAT_EARTHDAMAGE", "COMBAT_ENERGYDAMAGE", "COMBAT_NONE", "CONST_SLOT_FEET", "CONST_SLOT_NECKLACE", "CONST_SLOT_HEAD", "ITEM_ATTRIBUTE_DESCRIPTION",
        "ITEM_ATTRIBUTE_DURATION", "ITEM_ATTRIBUTE_CHARGES", "TILESTATE_PROTECTIONZONE", "TILESTATE_IMMOVABLEBLOCKSOLID", "TILESTATE_BLOCKSOLID",
        "RETURNVALUE_PLAYERISPZLOCKED", "RETURNVALUE_NOTPOSSIBLE", "RETURNVALUE_NOTENOUGHROOM", "SKILL_CLUB", "SKILL_SWORD", "SKILL_AXE", "SKILL_DISTANCE", "SKILL_MAGLEVEL"
    }
    for i, name in ipairs(constants) do env[name] = i end
    env.Combat = {DamageType = {None = 0, Fire = 1, Ice = 2, Earth = 3, Energy = 4, Healing = 5, ManaDrain = 6}}
    env.PLAYERSEX_FEMALE = 0
    env.RETURNVALUE_NOERROR = 0
    env.CONTAINER_POSITION = 65535
    env.Condition = function() return {setParameter = noop} end
    env.ItemEvent = function()
        local obj = {ids = {}}
        function obj:id(...) for _, id in ipairs({...}) do self.ids[#self.ids + 1] = id end end
        function obj:register() for _, id in ipairs(self.ids) do env.actions[id] = self end end
        return obj
    end
    env.TalkAction = function(...)
        local obj = {words = {...}}
        function obj:separator(separator) self.sep = separator end
        function obj:register() for _, words in ipairs(self.words) do env.talks[words] = self end end
        return obj
    end
    env.configKeys = {REMOVE_POTION_CHARGES = 1, FREE_PREMIUM = 2}
    env.configManager = {getBoolean = function(key) return key == env.configKeys.REMOVE_POTION_CHARGES end}
    env.ItemType = function() return {getArticle = function() return "a" end, getPluralName = function() return "items" end, getName = function() return "item" end} end
    env.Game = {createItem = function(id) return item(id) end}
    env.Player = function(obj) return obj and obj.isPlayer and obj:isPlayer() and obj or nil end
    env.Position = function(x, y, z) return {x = x, y = y, z = z} end
    loadScript(env, "data/lib/core/storages.lua")
    return env
end

test("depleted roots cannot award loot and fresh roots can be harvested once", function()
    local e, p = environment(), player()
    loadScript(e, "data/scripts/itemevents/use/others/juicy_root.lua")
    for _, id in ipairs({23477, 23478}) do e.actions[id].onUse(p, item(id)) end
    equal(#p.rewards, 0)
    local root = item(23475)
    e.actions[23475].onUse(p, root)
    equal(root.itemid, 23477)
    e.actions[23477].onUse(p, root)
    equal(#p.rewards, 1)
end)

test("gravedigger taming requires five successful attempts from missing storage", function()
    local e, p = environment(), player()
    loadScript(e, "data/scripts/itemevents/use/others/nail_case.lua")
    local monster = {isCreature = function() return true end, isMonster = function() return true end, getMaster = function() end,
        getName = function() return "gravedigger" end, remove = noop}
    local tool = item(21452)
    for i = 1, 4 do e.actions[21452].onUse(p, tool, nil, monster); expect(not p.mounts[39]); equal(tool.removed, 0) end
    e.actions[21452].onUse(p, tool, nil, monster)
    expect(p.mounts[39]); equal(tool.removed, 1)
end)

local function ropeScenario(pz, locked, occupants, missing, nativeResult)
    local e, p, target = environment(), player(), player()
    e.ropeSpots = {}
    local moved, calls = false, 0
    target.isPzLocked = function() return locked end
    target.teleportTo = function() moved = true; return true end
    local tiles = {
        {getGround = function() end, getItemById = function() end},
        {getTopVisibleThing = function() return target end},
        not missing and {getGround = function() return {} end, getCreatureCount = function() return occupants end,
            queryAdd = function(_, creature) equal(creature, target); return nativeResult or e.RETURNVALUE_NOERROR end,
            hasFlag = function(_, flag) return pz and flag == e.TILESTATE_PROTECTIONZONE end} or nil
    }
    e.Tile = function() calls = calls + 1; return tiles[calls] end
    loadScript(e, "data/scripts/lib/actions.lua")
    e.onUseRope(p, {}, {}, {itemid = 294}, position())
    return moved
end
test("rope rejects PZ-locked recipients entering protection", function()
    expect(not ropeScenario(true, true, 0))
    expect(ropeScenario(true, false, 0))
    expect(ropeScenario(false, true, 0))
end)
test("rope rejects occupied and missing exit tiles", function()
    expect(not ropeScenario(false, false, 1))
    expect(not ropeScenario(false, false, 0, true))
    expect(ropeScenario(false, false, 0))
    expect(not ropeScenario(false, false, 0, false, 1))
end)

test("healing potion checks recipient level and vocation", function()
    local e, p, target = environment(), player(), player()
    local heals = 0
    e.doTargetCombat = function(_, recipient) equal(recipient, target); heals = heals + 1 end
    loadScript(e, "data/scripts/itemevents/use/other/potions.lua")
    e.onUse = assert(e.actions[8473]).onUse
    target.level = 1
    local potion = item(8473)
    e.onUse(p, potion, position(), target)
    equal(heals, 0); equal(potion.removed, 0)
    target.level = 200; target.vocation = 1
    e.onUse(p, potion, position(), target)
    equal(heals, 0); equal(potion.removed, 0)
    p.level = 1; p.vocation = 1; target.vocation = 4
    e.onUse(p, potion, position(), target)
    equal(heals, 1); equal(potion.removed, 1)
    equal(e.onUse(p, potion, position(), {}), false)
end)

test("demonic blood works in equipment and preserves input on a full container", function()
    local e, p = environment(), player()
    loadScript(e, "data/scripts/itemevents/use/other/potions.lua")
    e.onUse = assert(e.actions[8473]).onUse
    local blood = item(6558)
    blood.getParent = function() return p end
    e.onUse(p, blood, {x = e.CONTAINER_POSITION}, {})
    equal(blood.removed, 1); equal(#p.rewards, 1)
    equal(p.rewards[1].id, 7588)
    local full = item(6558)
    full.getParent = function() return {isContainer = function() return true end, addItem = function() end} end
    e.onUse(p, full, {x = e.CONTAINER_POSITION}, {})
    equal(full.removed, 0); equal(#p.rewards, 1)
    local failedRemoval = item(6558)
    failedRemoval.getParent = blood.getParent
    failedRemoval.remove = function() return false end
    e.onUse(p, failedRemoval, {x = e.CONTAINER_POSITION}, {})
    equal(p.rewards[2].item.removed, 1)
end)

test("all empty rust tables use the configured failure path without errors", function()
    local e, p = environment(), player()
    loadScript(e, "data/scripts/itemevents/use/tools/rust_remover.lua")
    for _, id in ipairs({9814, 9815, 9817, 9818, 9819, 9820}) do
        local target, remover = item(id), item(9930)
        e.actions[9930].onUse(p, remover, nil, target)
        equal(target.removed, 1); equal(remover.removed, 1)
    end
    local target, remover = item(9808), item(9930)
    e.actions[9930].onUse(p, remover, nil, target)
    equal(target.itemid, 2464); equal(target.removed, 0); equal(remover.removed, 1)
end)

test("marble sculpting supports success and failure with one consumption", function()
    local e, p = environment(), player()
    loadScript(e, "data/scripts/itemevents/use/tools/skinning.lua")
    local rock = item(11343)
    e.actions[5908].onUse(p, item(5908), nil, rock, position())
    equal(#p.rewards, 1); equal(p.rewards[1].id, 11346); equal(rock.removed, 1)
    e.math.random = function() return 100000 end
    rock = item(11343)
    e.actions[5908].onUse(p, item(5908), nil, rock, position())
    equal(#p.rewards, 1); equal(rock.removed, 1)
end)

test("werewolf helmet enchants without invented storage or promotion requirements", function()
    local e, p = environment(), player()
    loadScript(e, "data/scripts/itemevents/use/other/enchanting.lua")
    e.onUse = assert(e.actions[24739]).onUse
    local crystal, helmet = item(24739), item(24718)
    e.onUse(p, crystal, nil, helmet)
    equal(helmet.itemid, 24783); equal(crystal.removed, 1)
    helmet = item(24718)
    helmet.getType = function() return {usesSlot = function() return true end} end
    helmet.getParent = function() return p end
    e.onUse(p, crystal, nil, helmet)
    equal(helmet.itemid, 24718); equal(crystal.removed, 1)
end)

local function afflicted(sex)
    local e, p = environment(), player()
    p.storage[e.PlayerStorageKeys.afflictedOutfit] = 1
    p.getSex = function() return sex end
    local addons = {}
    p.addOutfitAddon = function(_, id, addon) addons[id] = (addons[id] or 0) + addon end
    p.hasOutfit = function(_, id, addon) expect(type(id) == "number", "outfit must be defined"); return addons[id] == addon end
    loadScript(e, "data/scripts/itemevents/use/others/afflicted_outfit.lua")
    return e, p
end
test("plague bell uses its dedicated storage and rejects repeat consumption", function()
    local e, p = afflicted(0)
    local bell = item(13926)
    e.actions[13926].onUse(p, bell)
    equal(p.storage[e.PlayerStorageKeys.afflictedPlagueBell], 1)
    equal(p.storage[0], nil)
    e.actions[13926].onUse(p, bell)
    equal(bell.removed, 1)
end)
test("both sexes receive Beak Doctor after completing the afflicted addons", function()
    for _, sex in ipairs({0, 1}) do
        local e, p = afflicted(sex)
        e.actions[13925].onUse(p, item(13925))
        expect(not p.achievements["Beak Doctor"])
        e.actions[13926].onUse(p, item(13926))
        expect(p.achievements["Beak Doctor"])
    end
end)

test("deathlist accepts a name and formats all ordinal day boundaries", function()
    local e, p = environment(), player()
    local days = {1,2,3,9,11,12,13,21,22,23,31}
    local index, freed, query = 1, 0, 0
    e.os = setmetatable({date = function(fmt)
        if fmt == "%B" then return "January" end
        return {day = days[index], month = 1, year = 2026, hour = 1, min = 2, sec = 3}
    end}, {__index = os})
    e.db = {escapeString = function(name) equal(name, "Player Name"); return "'Player Name'" end,
        storeQuery = function() query = query + 1; return query end}
    e.result = {getNumber = function(_, key) return key == "is_player" and 1 or 100 end,
        getString = function(_, key) return key == "name" and "Player Name" or "Killer" end,
        free = function() freed = freed + 1 end,
        next = function() index = index + 1; return index <= #days end}
    p.popupFYI = function(_, text) p.popup = text end
    loadScript(e, "data/scripts/talkactions/deathlist.lua")
    equal(e.talks["!deathlist"].sep, " ")
    e.talks["!deathlist"].onSay(p, "!deathlist", "Player Name")
    for _, day in ipairs({"01st", "02nd", "03rd", "09th", "11th", "12th", "13th", "21st", "22nd", "23rd", "31st"}) do
        expect(p.popup:find(day .. " January", 1, true), day)
    end
    equal(freed, 2)
end)

test("sendtext uses native serialization and excludes ordinary players", function()
    local e, p = environment(), player()
    e.NetworkMessage = function() error("raw packets are forbidden in this callback") end
    loadScript(e, "data/scripts/talkactions/sendtext.lua")
    p.access = false
    equal(e.talks["/sendtext"].onSay(p, "/sendtext", ""), true)
    equal(#p.messages, 0)
    p.access = true
    equal(e.talks["/sendtext"].onSay(p, "/sendtext", ""), false)
    equal(p.messages[1][1], e.MESSAGE_EXPERIENCE)
    equal(p.messages[1][3], p.pos); equal(p.messages[1][4], 100)
end)

test("premium purchases honor the cap before taking money", function()
    local e, p = environment(), player()
    loadScript(e, "data/scripts/talkactions/buyprem.lua")
    p.premium = 276
    e.talks["!buypremium"].onSay(p)
    equal(p.premium, 276); equal(p.paid, 0)
    p.premium = 275
    e.talks["!buypremium"].onSay(p)
    equal(p.premium, 365); equal(p.paid, 10000)
end)

test("looktype validates malformed and fractional arguments", function()
    local e, p = environment(), player()
    local changes = 0
    p.getOutfit = function() return {} end
    p.setOutfit = function() changes = changes + 1 end
    loadScript(e, "data/scripts/talkactions/looktype.lua")
    for _, value in ipairs({"", "abc", "128.5", "-1", "903"}) do e.talks["/looktype"].onSay(p, "/looktype", value) end
    equal(changes, 0)
    e.talks["/looktype"].onSay(p, "/looktype", "128")
    equal(changes, 1)
end)

test("spawn commands reject undefined range indexes and accept valid ranges", function()
    local e, p = environment(), player()
    local spawns = 0
    e.Game.createMonster = function() spawns = spawns + 1; return {} end
    p.addSummon = noop
    loadScript(e, "data/scripts/talkactions/multispawn.lua")
    local function args(values) return {splitTrimmed = function() return values end} end
    for _, command in ipairs({"/multispawn", "/multisummon"}) do
        for _, value in ipairs({"0", "-1", "1.5", "4", "abc"}) do
            equal(e.talks[command].onSay(p, command, args({"rat", value})), false)
        end
        equal(e.talks[command].onSay(p, command, args({"rat", "/random", "1.5"})), false)
        equal(spawns, 0)
        e.talks[command].onSay(p, command, args({"rat", "1"}))
        equal(spawns, 8)
        spawns = 0
    end
end)

test("registered post horn has a usable instrument effect", function()
    local e, p = environment(), player()
    loadScript(e, "data/scripts/itemevents/use/other/music.lua")
    e.onUse = assert(e.actions[2364]).onUse
    local horn, effect = item(2364), nil
    horn.pos.sendMagicEffect = function(_, value) effect = value end
    equal(e.onUse(p, horn), true)
    equal(effect, e.CONST_ME_SOUND_GREEN)
end)

test("glooth foods are registered and can be eaten", function()
    local e, p = environment(), player()
    e.CONDITION_REGENERATION = 1
    e.CONDITIONID_DEFAULT = 0
    p.getCondition = function() end
    local fed = 0
    p.feed = function(_, amount) fed = fed + amount end
    loadScript(e, "data/scripts/itemevents/use/other/food.lua")
    for id = 23514, 23517 do
        local food = item(id)
        expect(e.actions[id], "unregistered food " .. id)
        equal(e.actions[id].onUse(p, food), true)
        equal(food.removed, 1)
    end
    equal(fed, (15 + 7 + 6 + 25) * 12)
end)

test("empty crate rolls consume the search cooldown", function()
    local e, p = environment(), player()
    local now, rolls = 100000, 0
    e.os = {time = function() return now end}
    e.math.random = function() rolls = rolls + 1; return rolls == 1 and 1 or 40 end
    loadScript(e, "data/scripts/itemevents/use/others/crate_usable.lua")
    e.actions[9661].onUse(p, item(9661))
    equal(p.storage[e.PlayerStorageKeys.crateUsable], now + 3600)
    e.actions[9661].onUse(p, item(9661))
    equal(rolls, 1); equal(#p.rewards, 0)
    now = now + 3600
    e.actions[9661].onUse(p, item(9661))
    equal(rolls, 2); equal(#p.rewards, 1)
end)

print(string.format("Action regression tests: %d passed, %d failed", passed, failed))
if failed > 0 then os.exit(1) end
