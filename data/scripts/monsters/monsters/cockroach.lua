local mtype = Game.createMonsterType("Cockroach")
local monster = {}

monster.name = "Cockroach"
monster.description = "a cockroach"
monster.experience = 0
monster.race = "venom"
monster.maxHealth = 1
monster.health = 1
monster.speed = 180
monster.manaCost = 0
monster.corpse = 8593
monster.outfit = { lookType = 284 }
monster.runHealth = 1
monster.changeTarget = {
    interval = 60000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 50
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = false,
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.loot = {
    {
        id = "cockroach leg",
        chance = 100000,
    },
}

mtype:register(monster)
