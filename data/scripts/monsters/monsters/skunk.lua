local mtype = Game.createMonsterType("Skunk")
local monster = {}

monster.name = "Skunk"
monster.description = "a skunk"
monster.experience = 3
monster.race = "blood"
monster.maxHealth = 20
monster.health = 20
monster.speed = 136
monster.manaCost = 200
monster.corpse = 6035
monster.outfit = { lookType = 106 }
monster.runHealth = 8
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -5,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        range = 1,
        minDamage = -1,
        maxDamage = -3,
        target = true,
    },
}
monster.defenses = {
    defense = 1,
    armor = 1,
}
monster.loot = {
    {
        id = 9114,
        chance = 4472,
    },
    {
        id = 11191,
        chance = 884,
    },
}

mtype:register(monster)
