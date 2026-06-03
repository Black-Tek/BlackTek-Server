local mtype = Game.createMonsterType("Carrion Worm")
local monster = {}

monster.name = "Carrion Worm"
monster.description = "a carrion worm"
monster.experience = 70
monster.race = "blood"
monster.maxHealth = 145
monster.health = 145
monster.speed = 160
monster.manaCost = 380
monster.corpse = 6069
monster.outfit = { lookType = 192 }
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = true,
    pushable = false,
    canPushItems = true,
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
        maxDamage = -45,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 5},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 45,
    },
    {
        id = "meat",
        chance = 9460,
        maxCount = 2,
    },
    {
        id = "worm",
        chance = 2100,
        maxCount = 2,
    },
    {
        id = "carrion worm fang",
        chance = 10000,
    },
    {
        id = 13757,
        chance = 210,
    },
}

mtype:register(monster)
