local mtype = Game.createMonsterType("Rabbit")
local monster = {}

monster.name = "Rabbit"
monster.description = "a rabbit"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 15
monster.health = 15
monster.speed = 180
monster.manaCost = 220
monster.corpse = 6017
monster.outfit = { lookType = 74 }
monster.runHealth = 15
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = false,
    illusionable = true,
    convinceable = true,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.defenses = {
    defense = 1,
    armor = 1,
}
monster.loot = {
    {
        id = "meat",
        chance = 82000,
        maxCount = 2,
    },
    {
        id = 2684,
        chance = 10000,
        maxCount = 2,
    },
}

mtype:register(monster)
