local mtype = Game.createMonsterType("Phantasm")
local monster = {}

monster.name = "Phantasm"
monster.description = "a phantasm"
monster.experience = 4400
monster.race = "undead"
monster.maxHealth = 3950
monster.health = 3950
monster.speed = 340
monster.manaCost = 0
monster.corpse = 6344
monster.outfit = { lookType = 241 }
monster.runHealth = 350
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
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
        maxDamage = -475,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -250,
        maxDamage = -610,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 20,
        minDamage = -5,
        maxDamage = -80,
        radius = 3,
        target = false,
        effect = CONST_ME_YELLOW_RINGS,
    },
    {
        name = "phantasm drown",
        interval = 2000,
        chance = 15,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 15,
        radius = 5,
        target = false,
        duration = 6000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 80,
    armor = 80,
    {
        name = "healing",
        interval = 2000,
        chance = 30,
        minDamage = 228,
        maxDamage = 449,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        speed = 500,
        duration = 6000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 25,
        duration = 8000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "death", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "physical", combat = true, condition = true},
}
monster.maxSummons = 4
monster.summons = {
    {name = "Phantasm Summon", interval = 2000, chance = 20, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Oh my, you forgot to put your pants on!", yell = false},
    {text = "Weeheeheeheehee!", yell = false},
    {text = "Its nothing but a dream!", yell = false},
    {text = "Give in!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 87839,
        maxCount = 238,
    },
    {
        id = 2152,
        chance = 87839,
        maxCount = 4,
    },
    {
        id = 7590,
        chance = 30634,
        maxCount = 2,
    },
    {
        id = 2804,
        chance = 25739,
    },
    {
        id = 2260,
        chance = 21552,
        maxCount = 2,
    },
    {
        id = 8473,
        chance = 17518,
    },
    {
        id = 6500,
        chance = 16964,
    },
    {
        id = 2150,
        chance = 14286,
        maxCount = 3,
    },
    {
        id = 9970,
        chance = 13147,
        maxCount = 3,
    },
    {
        id = 2147,
        chance = 11576,
        maxCount = 3,
    },
    {
        id = 2149,
        chance = 11115,
        maxCount = 3,
    },
    {
        id = 7451,
        chance = 862,
    },
    {
        id = 2165,
        chance = 800,
    },
    {
        id = 2487,
        chance = 708,
    },
    {
        id = 7414,
        chance = 493,
    },
    {
        id = 6300,
        chance = 369,
    },
}

mtype:register(monster)
