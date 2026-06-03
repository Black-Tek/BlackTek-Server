local mtype = Game.createMonsterType("Blood Beast")
local monster = {}

monster.name = "Blood Beast"
monster.description = "a blood beast"
monster.experience = 1000
monster.race = "venom"
monster.maxHealth = 1600
monster.health = 1600
monster.speed = 220
monster.manaCost = 0
monster.corpse = 23351
monster.outfit = { lookType = 602 }
monster.changeTarget = {
    interval = 2000,
    chance = 4,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        attack = 50,
        skill = 82,
        -- NOTE: melee condition (poison=260); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 260000,
        },
    },
    {
        name = "poison",
        interval = 2000,
        chance = 13,
        range = 5,
        minDamage = -65,
        maxDamage = -105,
        target = true,
        shootEffect = CONST_ANI_GREENSTAR,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 17,
        minDamage = -300,
        maxDamage = -400,
        radius = 4,
        target = false,
        effect = CONST_ME_MAGIC_GREEN,
    },
}
monster.defenses = {
    defense = 28,
    armor = 19,
    {
        name = "speed",
        interval = 2000,
        chance = 8,
        speed = 314,
        duration = 6000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 17,
        minDamage = 80,
        maxDamage = 120,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "lifedrain", combat = true},
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "Rawrrr!", yell = false},
    {text = "*grlb*", yell = false},
    {text = "Roarr!", yell = false},
}
monster.loot = {
    {
        id = 23549,
        chance = 16010,
    },
    {
        id = 10557,
        chance = 5080,
    },
    {
        id = 23517,
        chance = 3720,
    },
    {
        id = 2148,
        chance = 100000,
        maxCount = 100,
    },
    {
        id = 2148,
        chance = 60000,
        maxCount = 50,
    },
    {
        id = 23565,
        chance = 20040,
    },
    {
        id = 7588,
        chance = 7710,
    },
    {
        id = 7366,
        chance = 8290,
        maxCount = 5,
    },
    {
        id = 23554,
        chance = 850,
    },
    {
        id = 23550,
        chance = 810,
    },
    {
        id = 23549,
        chance = 810,
    },
    {
        id = 23551,
        chance = 1050,
    },
    {
        id = 23529,
        chance = 780,
    },
}

mtype:register(monster)
