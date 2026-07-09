local mtype = Game.createMonsterType("Omruc")
local monster = {}

monster.name = "Omruc"
monster.description = "Omruc"
monster.experience = 2950
monster.race = "undead"
monster.maxHealth = 4300
monster.health = 4300
monster.speed = 370
monster.manaCost = 0
monster.corpse = 6025
monster.outfit = { lookType = 90 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 4
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -120,
        -- NOTE: melee condition (poison=116); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 116000,
        },
    },
    {
        name = "lifedrain",
        interval = 5000,
        chance = 20,
        range = 1,
        minDamage = -100,
        maxDamage = -250,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        minDamage = -200,
        maxDamage = -500,
        shootEffect = CONST_ANI_POISONARROW,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 20,
        range = 3,
        minDamage = -120,
        maxDamage = -450,
        shootEffect = CONST_ANI_BURSTARROW,
        effect = CONST_ME_EXPLOSIONAREA,
    },
    {
        name = "melee",
        interval = 3000,
        chance = 20,
        minDamage = -150,
        maxDamage = -500,
        shootEffect = CONST_ANI_ARROW,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 25,
        range = 7,
        speed = -900,
        duration = 50000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 60,
    armor = 60,
    {
        name = "healing",
        interval = 1000,
        chance = 17,
        minDamage = 100,
        maxDamage = 200,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 14,
        duration = 8000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 5},
    {type = COMBAT_ICEDAMAGE, percent = 5},
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 4
monster.summons = {
    {name = "Stalker", interval = 2000, chance = 100, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Now chhhou shhhee me ... Now chhhou don't.", yell = false},
    {text = "Chhhhou are marked ashhh my prey.", yell = false},
    {text = "Psssst, I am over chhhere.", yell = false},
    {text = "Catchhhh me if chhhou can.", yell = false},
    {text = "Die!", yell = false},
}
monster.loot = {
    {
        id = 2352,
        chance = 100000,
    },
    {
        id = 2148,
        chance = 85417,
        maxCount = 211,
    },
    {
        id = 2674,
        chance = 78125,
        maxCount = 2,
    },
    {
        id = 2545,
        chance = 57292,
        maxCount = 19,
    },
    {
        id = 2546,
        chance = 47917,
        maxCount = 15,
    },
    {
        id = 7365,
        chance = 33333,
        maxCount = 6,
    },
    {
        id = 2544,
        chance = 22917,
        maxCount = 25,
    },
    {
        id = 2547,
        chance = 17708,
        maxCount = 5,
    },
    {
        id = 7591,
        chance = 11458,
    },
    {
        id = 2145,
        chance = 9375,
        maxCount = 3,
    },
    {
        id = 2165,
        chance = 3125,
    },
    {
        id = 11207,
        chance = 2083,
    },
    {
        id = 2195,
        chance = 1042,
    },
}

mtype:register(monster)
