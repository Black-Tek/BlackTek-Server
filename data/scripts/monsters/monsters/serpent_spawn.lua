local mtype = Game.createMonsterType("Serpent Spawn")
local monster = {}

monster.name = "Serpent Spawn"
monster.description = "a serpent spawn"
monster.experience = 3050
monster.race = "blood"
monster.maxHealth = 3000
monster.health = 3000
monster.speed = 234
monster.manaCost = 0
monster.corpse = 6061
monster.outfit = { lookType = 220 }
monster.runHealth = 275
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -250,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -80,
        maxDamage = -300,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 1,
        range = 7,
        duration = 3000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 25,
        range = 7,
        radius = 4,
        target = true,
        speed = -850,
        duration = 12000,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -200,
        maxDamage = -500,
        length = 8,
        effect = CONST_ME_SOUND_RED,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        minDamage = -200,
        maxDamage = -500,
        length = 8,
        spread = 3,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 35,
    armor = 35,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 250,
        maxDamage = 500,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 340,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Sssssouls for the one", yell = false},
    {text = "HISSSS", yell = true},
    {text = "Tsssse one will risssse again", yell = false},
    {text = "I bring your deathhh, mortalssss", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 97236,
        maxCount = 245,
    },
    {
        id = 2796,
        chance = 18300,
    },
    {
        id = 10611,
        chance = 14967,
    },
    {
        id = 2146,
        chance = 12132,
    },
    {
        id = 2547,
        chance = 6107,
    },
    {
        id = 2168,
        chance = 6058,
    },
    {
        id = 2167,
        chance = 5968,
    },
    {
        id = 2033,
        chance = 2958,
    },
    {
        id = 7386,
        chance = 2030,
    },
    {
        id = 7590,
        chance = 2014,
    },
    {
        id = 2182,
        chance = 986,
    },
    {
        id = 11230,
        chance = 941,
    },
    {
        id = 2528,
        chance = 855,
    },
    {
        id = 7456,
        chance = 807,
    },
    {
        id = 2177,
        chance = 789,
    },
    {
        id = 2479,
        chance = 633,
    },
    {
        id = 5956,
        chance = 569,
    },
    {
        id = 2475,
        chance = 564,
    },
    {
        id = 2487,
        chance = 504,
    },
    {
        id = 3971,
        chance = 184,
    },
    {
        id = 2498,
        chance = 124,
    },
    {
        id = 8902,
        chance = 98,
    },
    {
        id = 8880,
        chance = 84,
    },
}

mtype:register(monster)
