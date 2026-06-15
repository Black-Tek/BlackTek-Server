local mtype = Game.createMonsterType("Chicken")
local monster = {}

monster.name = "Chicken"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 15
monster.health = 15
monster.speed = 128
monster.manaCost = 220
monster.corpse = 6042
monster.outfit = { lookType = 111 }
monster.runHealth = 15
monster.changeTarget = {
    interval = 4000,
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
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Gokgoooook", yell = false},
    {text = "Cluck Cluck", yell = false},
}
monster.loot = {
    {
        id = "chicken feather",
        chance = 20000,
    },
    {
        id = "worm",
        chance = 10000,
    },
    {
        id = "meat",
        chance = 2120,
        maxCount = 2,
    },
    {
        id = 2695,
        chance = 1000,
    },
}

mtype:register(monster)
