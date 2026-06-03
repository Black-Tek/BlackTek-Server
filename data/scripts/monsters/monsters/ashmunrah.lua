local mtype = Game.createMonsterType("Ashmunrah")
local monster = {}

monster.name = "Ashmunrah"
monster.description = "Ashmunrah"
monster.experience = 3100
monster.race = "undead"
monster.maxHealth = 5000
monster.health = 5000
monster.speed = 430
monster.manaCost = 0
monster.corpse = 6031
monster.outfit = { lookType = 87 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -1000,
        -- NOTE: melee condition (poison=55); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 55000,
        },
    },
    {
        name = "lifedrain",
        interval = 3000,
        chance = 7,
        range = 1,
        minDamage = -100,
        maxDamage = -700,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 12,
        range = 7,
        minDamage = -100,
        maxDamage = -500,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "physical",
        interval = 3000,
        chance = 12,
        range = 7,
        minDamage = -120,
        maxDamage = -750,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "speed",
        interval = 3000,
        chance = 25,
        range = 7,
        speed = -650,
        duration = 50000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 18,
        minDamage = -50,
        maxDamage = -550,
        length = 8,
        spread = 3,
        effect = CONST_ME_YELLOW_RINGS,
    },
}
monster.defenses = {
    defense = 30,
    armor = 25,
    {
        name = "healing",
        interval = 1000,
        chance = 20,
        minDamage = 200,
        maxDamage = 400,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 1000,
        chance = 7,
        duration = 2000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "outfit",
        interval = 1000,
        chance = 3,
        duration = 6000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = 25},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 4
monster.summons = {
    {name = "Ancient Scarab", interval = 1000, chance = 100, max = 2},
    {name = "Green Djinn", interval = 1000, chance = 100, max = 4},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "No mortal or undead will steal my secrets!", yell = false},
    {text = "Ahhhh all those long years.", yell = false},
    {text = "My traitorous son has thee.", yell = false},
    {text = "Come to me, my allys and underlings.", yell = false},
    {text = "I might be trapped but not without power.", yell = false},
    {text = "Ages come, ages go. Ashmunrah remains.", yell = false},
    {text = "You will be history soon.", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 33000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 33000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 33000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 4,
    },
    {
        id = 7590,
        chance = 12005,
    },
    {
        id = 2164,
        chance = 4662,
    },
    {
        id = 2134,
        chance = 4196,
    },
    {
        id = 2487,
        chance = 466,
    },
    {
        id = 11207,
        chance = 175,
    },
    {
        id = 2444,
        chance = 117,
    },
    {
        id = 2140,
        chance = 117,
    },
}

mtype:register(monster)
