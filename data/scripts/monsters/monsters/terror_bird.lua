local mtype = Game.createMonsterType("Terror Bird")
local monster = {}

monster.name = "Terror Bird"
monster.description = "a terror bird"
monster.experience = 150
monster.race = "blood"
monster.maxHealth = 300
monster.health = 300
monster.speed = 212
monster.manaCost = 490
monster.corpse = 6057
monster.outfit = { lookType = 218 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -90,
    },
}
monster.defenses = {
    defense = 13,
    armor = 13,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "CRAAAHHH!", yell = false},
    {text = "Gruuuh Gruuuh.", yell = false},
    {text = "Carrah Carrah!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 82500,
        maxCount = 30,
    },
    {
        id = "meat",
        chance = 48000,
        maxCount = 3,
    },
    {
        id = "terrorbird beak",
        chance = 10000,
    },
    {
        id = "worm",
        chance = 9500,
    },
    {
        id = "colourful feather",
        chance = 3000,
    },
    {
        id = "health potion",
        chance = 690,
    },
    {
        id = "seeds",
        chance = 230,
    },
    {
        id = "feather headdress",
        chance = 100,
    },
}

mtype:register(monster)
