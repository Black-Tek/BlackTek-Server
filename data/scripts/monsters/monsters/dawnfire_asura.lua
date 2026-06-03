local mtype = Game.createMonsterType("Dawnfire Asura")
local monster = {}

monster.name = "Dawnfire Asura"
monster.description = "a dawnfire asura"
monster.experience = 4100
monster.race = "blood"
monster.maxHealth = 2900
monster.health = 2900
monster.speed = 210
monster.manaCost = 0
monster.corpse = 24643
monster.outfit = {
    lookType = 150,
    lookHead = 114,
    lookBody = 94,
    lookLegs = 78,
    lookFeet = 79,
    lookAddons = 1,
}
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -269,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -70,
    },
    {
        name = "firefield",
        interval = 2000,
        chance = 10,
        range = 7,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -50,
        maxDamage = -200,
        length = 8,
        effect = CONST_ME_PURPLEENERGY,
    },
    {
        name = "energy strike",
        interval = 2000,
        chance = 10,
        range = 1,
        minDamage = -10,
        maxDamage = -100,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        radius = 1,
        target = true,
        speed = -100,
        duration = 30000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 55,
    armor = 55,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 50,
        maxDamage = 100,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 320,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_EARTHDAMAGE, percent = -15},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 7,
    {text = "Encounter the flames of destiny!", yell = false},
    {text = "Fire and destruction!", yell = true},
    {text = "I will end your torment. Do not run, little mortal.", yell = false},
    {text = "Join me, brothers, for a BLOODY FRENZY!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 100,
    },
    {
        id = 2148,
        chance = 80000,
        maxCount = 100,
    },
    {
        id = 2148,
        chance = 60000,
        maxCount = 42,
    },
    {
        id = 2152,
        chance = 18200,
        maxCount = 6,
    },
    {
        id = 6558,
        chance = 2000,
    },
    {
        id = 7590,
        chance = 2000,
    },
    {
        id = 2150,
        chance = 210,
        maxCount = 3,
    },
    {
        id = 2145,
        chance = 300,
        maxCount = 3,
    },
    {
        id = 2149,
        chance = 300,
        maxCount = 3,
    },
    {
        id = 2147,
        chance = 350,
        maxCount = 3,
    },
    {
        id = 9970,
        chance = 280,
        maxCount = 3,
    },
    {
        id = 2158,
        chance = 300,
    },
    {
        id = 6300,
        chance = 260,
    },
    {
        id = 6500,
        chance = 330,
    },
    {
        id = 8871,
        chance = 220,
    },
    {
        id = 24630,
        chance = 500,
    },
    {
        id = 7899,
        chance = 100,
    },
    {
        id = 2194,
        chance = 400,
    },
    {
        id = 2663,
        chance = 300,
    },
    {
        id = 24637,
        chance = 300,
    },
    {
        id = 24631,
        chance = 500,
    },
    {
        id = 5911,
        chance = 300,
    },
    {
        id = 2133,
        chance = 400,
    },
    {
        id = 2134,
        chance = 900,
    },
    {
        id = 5944,
        chance = 300,
    },
    {
        id = 8902,
        chance = 100,
    },
    {
        id = 2187,
        chance = 200,
    },
}

mtype:register(monster)
