local mtype = Game.createMonsterType("Dipthrah")
local monster = {}

monster.name = "Dipthrah"
monster.description = "Dipthrah"
monster.experience = 2900
monster.race = "undead"
monster.maxHealth = 4200
monster.health = 4200
monster.speed = 320
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
        maxDamage = -200,
        -- NOTE: melee condition (poison=65); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 65000,
        },
    },
    {
        name = "lifedrain",
        interval = 4000,
        chance = 20,
        range = 1,
        minDamage = -100,
        maxDamage = -800,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -100,
        maxDamage = -500,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 15,
        range = 7,
        speed = -650,
        duration = 50000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "drunk",
        interval = 1000,
        chance = 12,
        radius = 7,
        target = false,
        effect = CONST_ME_LOSEENERGY,
    },
    {
        name = "melee",
        interval = 3000,
        chance = 34,
        minDamage = -50,
        maxDamage = -600,
        radius = 3,
        target = false,
        effect = CONST_ME_LOSEENERGY,
    },
    {
        name = "dipthrah skill reducer",
        interval = 2000,
        chance = 10,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "healing",
        interval = 1000,
        chance = 25,
        minDamage = 100,
        maxDamage = 200,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "physical", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 4
monster.summons = {
    {name = "Priestess", interval = 2000, chance = 15, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Come closer to learn the final lesson.", yell = false},
    {text = "You can't escape death forever.", yell = false},
    {text = "Undeath will shatter my shackles.", yell = false},
    {text = "You don't need this magic anymore.", yell = false},
}
monster.loot = {
    {
        id = 3241,
        chance = 100000,
    },
    {
        id = 2148,
        chance = 90981,
        maxCount = 226,
    },
    {
        id = 2146,
        chance = 9806,
        maxCount = 3,
    },
    {
        id = 7590,
        chance = 8927,
    },
    {
        id = 2167,
        chance = 4857,
    },
    {
        id = 2178,
        chance = 925,
    },
    {
        id = 2158,
        chance = 833,
    },
    {
        id = 2193,
        chance = 601,
    },
    {
        id = 2436,
        chance = 555,
    },
    {
        id = 11207,
        chance = 93,
    },
    {
        id = 2446,
        chance = 93,
    },
}

mtype:register(monster)
