-- Lua twins of data/world/forgotten-zones/example_zones.toml, "classic" revscript style:
-- Zones.create(name) builds an unregistered Zone, every property is set through its own
-- method call, and zone:register() commits it to ZoneManager last - same shape as
-- TalkAction/CreatureEvent revscripts elsewhere in data/scripts/.
--
-- See zone_new_revscripts.lua for the same ten zones built from a single table passed to
-- the Zone(...) constructor instead.

-- ---------------------------------------------------------------------------------------------
-- Flag-only zones - no spawnType/policy, so nothing here ever activates a spawn coroutine.
-- ---------------------------------------------------------------------------------------------

local entranceHall = Zones.create("EntranceHall")
entranceHall:setPositions({
    Position(95, 138, 7),
    Position(95, 139, 7)
})
entranceHall:setFlags({ "protection", "nologout" })
entranceHall:register()

-- A no-loot, no-experience training arena: monsters can be fought here but never drop anything,
-- and fighting them grants no experience.
local trainingArena = Zones.create("TrainingArena")
trainingArena:setPositions({
    Position(100, 138, 7),
    Position(101, 138, 7)
})
trainingArena:setFlags({ "no_drop", "no_experience", "no_party", "no_summons" })
trainingArena:register()

-- ---------------------------------------------------------------------------------------------
-- FixedSpawn - monsters respawn at fixed positions within setRange() after a delay. "passive"
-- only (re)spawns while no player is present in range; "forced" makes a passive zone keep
-- spawning on a cooldown cadence instead of pausing entirely.
-- ---------------------------------------------------------------------------------------------

local ratCellar = Zones.create("RatCellar")
ratCellar:setSpawnType("monster")
ratCellar:setPolicy("fixed")
ratCellar:setRange(Position(990, 990, 7), Position(1010, 1010, 7))
ratCellar:passive(false)
ratCellar:forced(false)

-- A weighted list draws one entry per spawn instead of always spawning the same monster.
ratCellar:addMonster({
    { name = "Rat",      chance = 60 },
    { name = "Cave Rat", chance = 40 }
}, Position(1000, 1000, 7), { interval = 60 })

-- A plain string is shorthand for a single, unweighted monster instead of a weighted list.
ratCellar:addMonster("Rotworm", Position(1002, 1000, 7), { interval = 30 })

ratCellar:register()

-- ---------------------------------------------------------------------------------------------
-- RelativeSpawn - spawns relative to (and rotated to face) a nearby player instead of a fixed
-- map position. The monster's position is an offset applied in front of the player.
-- ---------------------------------------------------------------------------------------------

local ghostAmbush = Zones.create("GhostAmbush")
ghostAmbush:setSpawnType("monster")
ghostAmbush:setPolicy("relative")
ghostAmbush:setRange(Position(990, 990, 7), Position(1010, 1010, 7))
ghostAmbush:addMonster("Ghost", Position(0, 2, 0), { interval = 45, chance = 25 })
ghostAmbush:register()

-- ---------------------------------------------------------------------------------------------
-- StagedSpawn - wave-based encounter. Each monster entry belongs to a wave; the next wave
-- starts once the previous wave's monsters are all dead.
--   rebootable: wave progress always resets to wave 1 once all players leave.
--   resumable:  wave progress pauses (rather than resetting) while the area is empty.
--   degradable: progress decays over time (reserved for future use by the scheduler).
--   timed:      waves advance on a timer instead of only on full-wave death.
-- ---------------------------------------------------------------------------------------------

local waveRoom = Zones.create("WaveRoom")
waveRoom:setSpawnType("monster")
waveRoom:setPolicy("staged")
waveRoom:setRange(Position(990, 990, 7), Position(1010, 1010, 7))
waveRoom:rebootable(true)
waveRoom:resumable(false)
waveRoom:degradable(false)
waveRoom:timed(false)

waveRoom:addMonster("Rat", Position(1000, 1000, 7), { wave = 1 })
waveRoom:addMonster("Rat", Position(1001, 1000, 7), { wave = 1 })
waveRoom:addMonster("Cave Rat", Position(1000, 1000, 7), { wave = 2 })

waveRoom:register()

-- ---------------------------------------------------------------------------------------------
-- TriggeredSpawn - spawns only when the configured game event fires at a position covered by
-- this zone's range. See config/zones.toml's neighbor docs for which SpawnTrigger strings are
-- wired up to real game events this release.
-- ---------------------------------------------------------------------------------------------

local graveyardTrigger = Zones.create("GraveyardTrigger")
graveyardTrigger:setSpawnType("monster")
graveyardTrigger:setPolicy("triggered")
graveyardTrigger:setRange(Position(990, 990, 7), Position(1010, 1010, 7))
graveyardTrigger:trigger("Enter")
graveyardTrigger:addMonster("Ghost", Position(1000, 1000, 7))
graveyardTrigger:register()

-- ---------------------------------------------------------------------------------------------
-- BossSpawn - a master monster plus optional minion monsters chained off it. minionBehavior
-- controls what happens to surviving minions once the master dies:
--   despawn_on_master_death | finish_life_only | independent_respawn
-- Each minion's delay (ms) is how long after the master spawns before that minion spawns.
-- ---------------------------------------------------------------------------------------------

local demonLair = Zones.create("DemonLair")
demonLair:setSpawnType("boss")
demonLair:setRange(Position(990, 990, 7), Position(1010, 1010, 7))
demonLair:minionBehavior("despawn_on_master_death")

demonLair:setBossMaster("Demon", Position(1000, 1000, 7))
demonLair:addMinion("Fire Elemental", Position(1001, 1000, 7), { delay = 2000 })
demonLair:addMinion("Fire Elemental", Position(999, 1000, 7), { delay = 4000 })

demonLair:register()

-- ---------------------------------------------------------------------------------------------
-- NPC spawns - "fixed" places an NPC once at a fixed position (re-placed after options.interval
-- seconds if removed; omit or use 0 for an NPC that never comes back). "relative" is the same
-- idea but placed relative to a nearby player, same as RelativeSpawn above.
-- ---------------------------------------------------------------------------------------------

local oldManHut = Zones.create("OldManHut")
oldManHut:setSpawnType("npc")
oldManHut:setPolicy("fixed")
oldManHut:setRange(Position(990, 990, 7), Position(1010, 1010, 7))
oldManHut:addNpc("Old Man", Position(1005, 1000, 7))
oldManHut:register()

local wanderingMerchant = Zones.create("WanderingMerchant")
wanderingMerchant:setSpawnType("npc")
wanderingMerchant:setPolicy("relative")
wanderingMerchant:setRange(Position(990, 990, 7), Position(1010, 1010, 7))
wanderingMerchant:addNpc("Old Man", Position(0, 2, 0))
wanderingMerchant:register()

-- ---------------------------------------------------------------------------------------------
-- Combined: flags and spawn behavior set once on the same zone - a no-death-penalty, no-logout
-- trial room that also happens to be a triggered encounter. Only setRange() is called;
-- register() derives positions from it itself, same as the TOML loader does for a zone that
-- only states one of the two.
-- ---------------------------------------------------------------------------------------------

local trialOfTheGhost = Zones.create("TrialOfTheGhost")
trialOfTheGhost:setRange(Position(990, 990, 7), Position(1010, 1010, 7))
trialOfTheGhost:setFlags({ "no_death_penalty", "nologout" })

trialOfTheGhost:setSpawnType("monster")
trialOfTheGhost:setPolicy("triggered")
trialOfTheGhost:trigger("Enter")
trialOfTheGhost:addMonster("Ghost", Position(1000, 1000, 7))

trialOfTheGhost:register()
