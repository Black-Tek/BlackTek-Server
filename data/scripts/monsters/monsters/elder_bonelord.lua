local mtype = Game.createMonsterType("Elder Bonelord")
local monster = {}

monster.name = "Elder Bonelord"
monster.description = "an elder bonelord"
monster.experience = 280
monster.race = "blood"
monster.maxHealth = 500
monster.health = 500
monster.speed = 170
monster.manaCost = 0
monster.corpse = 6037
monster.outfit = { lookType = 108 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -55,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 5,
        range = 7,
        minDamage = -45,
        maxDamage = -60,
        shootEffect = CONST_ANI_ENERGY,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 5,
        range = 7,
        minDamage = -40,
        maxDamage = -80,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -45,
        maxDamage = -90,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -20,
        maxDamage = -40,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 5,
        range = 7,
        minDamage = -45,
        maxDamage = -85,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 5,
        range = 7,
        minDamage = 0,
        maxDamage = -40,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        range = 7,
        speed = -600,
        duration = 20000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 13,
    armor = 13,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 30},
    {type = COMBAT_DEATHDAMAGE, percent = 30},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 6
monster.summons = {
    {name = "Gazer", interval = 2000, chance = 10, max = 0},
    {name = "Crypt Shambler", interval = 2000, chance = 15, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Let me take a look at you!", yell = false},
    {text = "Inferior creatures, bow before my power!", yell = false},
    {text = "659978 54764!", yell = false},
    {text = "653768764!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 99298,
        maxCount = 90,
    },
    {
        id = 11193,
        chance = 20030,
    },
    {
        id = 12468,
        chance = 9698,
    },
    {
        id = 7364,
        chance = 8785,
        maxCount = 4,
    },
    {
        id = 2377,
        chance = 2968,
    },
    {
        id = 2509,
        chance = 2033,
    },
    {
        id = 5898,
        chance = 1927,
    },
    {
        id = 2175,
        chance = 1016,
    },
    {
        id = 7589,
        chance = 836,
    },
    {
        id = 11197,
        chance = 458,
    },
    {
        id = 2518,
        chance = 86,
    },
    {
        id = 3972,
        chance = 84,
    },
}

mtype:register(monster)
