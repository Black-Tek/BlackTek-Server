-- Lua twins of data/world/forgotten-zones/example_zones.toml, "table constructor" revscript
-- style: every property is passed as one table straight into Zone(...), the same way the
-- TOML loader itself reads one [Label] table per zone. Zone(table) still only builds an
-- unregistered Zone - zone:register() commits it to ZoneManager, same last step as the
-- classic style.
--
-- See zone_classic_revscript.lua for the same ten zones built through individual setter calls
-- instead of one table literal.
--
-- Table keys mirror the TOML schema almost exactly (positions/range/flags/monsters/npcs/...),
-- with two adjustments the Zone binding makes for every table-driven call - monster/master/
-- minion entries use "spec" rather than TOML's "name"/"list" (spec accepts either a plain
-- name string or a weighted { {name=, chance=}, ... } list), and camelCase keys
-- (spawnType, minionBehavior, ...) rather than TOML's snake_case.

-- ---------------------------------------------------------------------------------------------
-- Flag-only zones - no spawnType/policy, so nothing here ever activates a spawn coroutine.
-- ---------------------------------------------------------------------------------------------

local entranceHall = Zone({
    name = "EntranceHall",
    positions = {
        Position(95, 138, 7),
        Position(95, 139, 7)
    },
    flags = { "protection", "nologout" }
})
entranceHall:register()

-- A no-loot, no-experience training arena: monsters can be fought here but never drop anything,
-- and fighting them grants no experience.
local trainingArena = Zone({
    name = "TrainingArena",
    positions = {
        Position(100, 138, 7),
        Position(101, 138, 7)
    },
    flags = { "no_drop", "no_experience", "no_party", "no_summons" }
})
trainingArena:register()

-- ---------------------------------------------------------------------------------------------
-- FixedSpawn - monsters respawn at fixed positions within "range" after a delay. "passive" only
-- (re)spawns while no player is present in "range"; "forced" makes a passive zone keep spawning
-- on a "cooldown" cadence instead of pausing entirely.
-- ---------------------------------------------------------------------------------------------

local ratCellar = Zone({
    name = "RatCellar",
    range = { Position(990, 990, 7), Position(1010, 1010, 7) },
    spawnType = "monster",
    policy = "fixed",
    passive = false,
    forced = false,
    monsters = {
        -- A weighted "spec" list draws one entry per spawn instead of always spawning the
        -- same monster.
        { spec = { { name = "Rat", chance = 60 }, { name = "Cave Rat", chance = 40 } },
          position = Position(1000, 1000, 7), interval = 60 },
        -- A plain string "spec" is shorthand for a single, unweighted monster.
        { spec = "Rotworm", position = Position(1002, 1000, 7), interval = 30 }
    }
})
ratCellar:register()

-- ---------------------------------------------------------------------------------------------
-- RelativeSpawn - spawns relative to (and rotated to face) a nearby player instead of a fixed
-- map position. The monster's "position" is an offset applied in front of the player.
-- ---------------------------------------------------------------------------------------------

local ghostAmbush = Zone({
    name = "GhostAmbush",
    range = { Position(990, 990, 7), Position(1010, 1010, 7) },
    spawnType = "monster",
    policy = "relative",
    monsters = {
        { spec = "Ghost", position = Position(0, 2, 0), interval = 45, chance = 25 }
    }
})
ghostAmbush:register()

-- ---------------------------------------------------------------------------------------------
-- StagedSpawn - wave-based encounter. Each monster entry's "wave" groups it; the next wave
-- starts once the previous wave's monsters are all dead.
--   rebootable: wave progress always resets to wave 1 once all players leave.
--   resumable:  wave progress pauses (rather than resetting) while the area is empty.
--   degradable: progress decays over time (reserved for future use by the scheduler).
--   timed:      waves advance on a timer instead of only on full-wave death.
-- ---------------------------------------------------------------------------------------------

local waveRoom = Zone({
    name = "WaveRoom",
    range = { Position(990, 990, 7), Position(1010, 1010, 7) },
    spawnType = "monster",
    policy = "staged",
    rebootable = true,
    resumable = false,
    degradable = false,
    timed = false,
    monsters = {
        { spec = "Rat",      position = Position(1000, 1000, 7), wave = 1 },
        { spec = "Rat",      position = Position(1001, 1000, 7), wave = 1 },
        { spec = "Cave Rat", position = Position(1000, 1000, 7), wave = 2 }
    }
})
waveRoom:register()

-- ---------------------------------------------------------------------------------------------
-- TriggeredSpawn - spawns only when the configured game event fires at a position covered by
-- this zone's "range". See config/zones.toml's neighbor docs for which SpawnTrigger strings are
-- wired up to real game events this release.
-- ---------------------------------------------------------------------------------------------

local graveyardTrigger = Zone({
    name = "GraveyardTrigger",
    range = { Position(990, 990, 7), Position(1010, 1010, 7) },
    spawnType = "monster",
    policy = "triggered",
    trigger = "Enter",
    monsters = {
        { spec = "Ghost", position = Position(1000, 1000, 7) }
    }
})
graveyardTrigger:register()

-- ---------------------------------------------------------------------------------------------
-- BossSpawn - a "master" monster plus optional "minions" chained off it. "minionBehavior"
-- controls what happens to surviving minions once the master dies:
--   despawn_on_master_death | finish_life_only | independent_respawn
-- Each minion's "delay" (ms) is how long after the master spawns before that minion spawns.
-- ---------------------------------------------------------------------------------------------

local demonLair = Zone({
    name = "DemonLair",
    range = { Position(990, 990, 7), Position(1010, 1010, 7) },
    spawnType = "boss",
    minionBehavior = "despawn_on_master_death",
    master = { spec = "Demon", position = Position(1000, 1000, 7) },
    minions = {
        { spec = "Fire Elemental", position = Position(1001, 1000, 7), delay = 2000 },
        { spec = "Fire Elemental", position = Position(999, 1000, 7), delay = 4000 }
    }
})
demonLair:register()

-- ---------------------------------------------------------------------------------------------
-- NPC spawns - "fixed" places an NPC once at a fixed position (re-placed after "interval"
-- seconds if removed; omit or use 0 for an NPC that never comes back). "relative" is the same
-- idea but placed relative to a nearby player, same as RelativeSpawn above.
-- ---------------------------------------------------------------------------------------------

local oldManHut = Zone({
    name = "OldManHut",
    range = { Position(990, 990, 7), Position(1010, 1010, 7) },
    spawnType = "npc",
    policy = "fixed",
    npcs = {
        { name = "Old Man", position = Position(1005, 1000, 7) }
    }
})
oldManHut:register()

local wanderingMerchant = Zone({
    name = "WanderingMerchant",
    range = { Position(990, 990, 7), Position(1010, 1010, 7) },
    spawnType = "npc",
    policy = "relative",
    npcs = {
        { name = "Old Man", position = Position(0, 2, 0) }
    }
})
wanderingMerchant:register()

-- ---------------------------------------------------------------------------------------------
-- Combined: flags and spawn behavior defined once on the same zone - a no-death-penalty,
-- no-logout trial room that also happens to be a triggered encounter. Only "range" is given;
-- register() derives "positions" from it itself, same as the TOML loader does for a zone that
-- only states one of the two.
-- ---------------------------------------------------------------------------------------------

local trialOfTheGhost = Zone({
    name = "TrialOfTheGhost",
    range = { Position(990, 990, 7), Position(1010, 1010, 7) },
    flags = { "no_death_penalty", "nologout" },
    spawnType = "monster",
    policy = "triggered",
    trigger = "Enter",
    monsters = {
        { spec = "Ghost", position = Position(1000, 1000, 7) }
    }
})
trialOfTheGhost:register()
