local mtype = Game.createMonsterType("Dwarf Guard")
local monster = {}

monster.name = "Dwarf Guard"
monster.description = "a dwarf guard"
monster.experience = 165
monster.race = "blood"
monster.maxHealth = 245
monster.health = 245
monster.speed = 206
monster.manaCost = 650
monster.corpse = 6013
monster.outfit = { lookType = 70 }
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
        maxDamage = -140,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_PHYSICALDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Hail Durin!", yell = false},
}
monster.loot = {
    {
        id = 2787,
        chance = 55243,
        maxCount = 2,
    },
    {
        id = 2643,
        chance = 40075,
    },
    {
        id = 2148,
        chance = 39985,
        maxCount = 30,
    },
    {
        id = 2483,
        chance = 9009,
    },
    {
        id = 2513,
        chance = 6014,
    },
    {
        id = 2417,
        chance = 3849,
    },
    {
        id = 3351,
        chance = 1549,
    },
    {
        id = 2387,
        chance = 592,
    },
    {
        id = 5880,
        chance = 481,
    },
    {
        id = 7618,
        chance = 394,
    },
    {
        id = 2208,
        chance = 198,
    },
    {
        id = 13757,
        chance = 195,
    },
    {
        id = 2150,
        chance = 120,
    },
}

mtype:register(monster)
