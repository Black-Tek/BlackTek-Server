-- Run from the repository root: lua tests/data/content_spells.lua
-- Exercises script behavior with game API stubs; no running server is required.

SKULL_BLACK = 99
PlayerFlag_CanSummonAll = 1
PlayerFlag_HasInfiniteMana = 2
CONST_ME_POFF = 1
CONST_ME_MAGIC_BLUE = 2
CONST_ME_TELEPORT = 3
RETURNVALUE_NOTPOSSIBLE = 1
RETURNVALUE_NOTENOUGHMANA = 2
RETURNVALUE_NOTENOUGHROOM = 3

local noOp = function() end
local lastSpell
function Spell()
    lastSpell = setmetatable({}, {__index = function() return noOp end})
    return lastSpell
end

local function loadSpell(path)
    dofile("data/scripts/spells/" .. path)
    return lastSpell
end

local position = {sendMagicEffect = noOp}
local player
local creates
local removals
local allowCreation
local corpse = {
    getType = function()
        return {isCorpse = function() return true end, isMovable = function() return true end}
    end,
    remove = function() removals = removals + 1 end,
}
function Tile()
    return {getTopDownItem = function() return corpse end}
end
Game = {createMonster = function()
    creates = creates + 1
    if allowCreation then
        return {getPosition = function() return position end}
    end
end}

local function resetPlayer(count, privileged)
    creates, removals, allowCreation = 0, 0, true
    player = {
        summons = {}, mana = 150, manaSpent = 0, messages = {},
        getSkull = function() return 0 end,
        getPosition = function() return position end,
        hasFlag = function(_, flag) return privileged and flag == PlayerFlag_CanSummonAll end,
        getSummons = function(self) return self.summons end,
        addSummon = function(self, summon) self.summons[#self.summons + 1] = summon end,
        getMana = function(self) return self.mana end,
        addMana = function(self, amount) self.mana = self.mana + amount end,
        addManaSpent = function(self, amount) self.manaSpent = self.manaSpent + amount end,
        sendCancelMessage = function(self, message) self.messages[#self.messages + 1] = message end,
    }
    for _ = 1, count do player.summons[#player.summons + 1] = {} end
end
local variant = {
    getPosition = function() return position end,
    getString = function() return "Skeleton" end,
}

local animate = loadSpell("runes/support/animate_dead_rune.lua")
resetPlayer(2, false)
assert(animate.onCastSpell(player, variant, false) == false)
assert(creates == 0 and removals == 0 and #player.summons == 2,
    "A capped cast must not create a monster or consume its corpse")
resetPlayer(1, false)
assert(animate.onCastSpell(player, variant, false) == true)
assert(creates == 1 and removals == 1 and #player.summons == 2)
resetPlayer(2, true)
assert(animate.onCastSpell(player, variant, false) == true)
assert(#player.summons == 3, "CanSummonAll must retain its privileged bypass")
resetPlayer(1, false)
allowCreation = false
assert(animate.onCastSpell(player, variant, false) == false)
assert(removals == 0 and #player.summons == 1)

function MonsterType()
    return {isSummonable = function() return true end, manaCost = function() return 100 end}
end
local summon = loadSpell("support/summon_creature.lua")
resetPlayer(0, false)
assert(summon.onCastSpell(player, variant) == true)
assert(#player.summons == 1 and player.mana == 50 and player.manaSpent == 100)
resetPlayer(0, false)
player.mana = 99
assert(summon.onCastSpell(player, variant) == false)
assert(creates == 0 and player.mana == 99 and player.manaSpent == 0)

print("PASS: summon limits, placement failures, privilege bypass, and summon mana")
