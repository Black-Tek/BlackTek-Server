local mtype = Game.createMonsterType("Terrorsleep")
local monster = {}

monster.name = "Terrorsleep"
monster.description = "a terrorsleep"
monster.experience = 5900
monster.race = "venom"
monster.maxHealth = 7200
monster.health = 7200
monster.speed = 360
monster.manaCost = 0
monster.corpse = 22497
monster.outfit = { lookType = 593 }
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
        maxDamage = -450,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 20,
        minDamage = -1000,
        maxDamage = -1500,
        radius = 7,
        target = false,
        effect = CONST_ME_YELLOW_RINGS,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        minDamage = -100,
        maxDamage = -300,
        radius = 5,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "feversleep skill reducer",
        interval = 2000,
        chance = 10,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -350,
        maxDamage = -500,
        length = 6,
        target = true,
        effect = CONST_ME_YELLOWENERGY,
    },
    {
        name = "death",
        interval = 2000,
        chance = 20,
        minDamage = -200,
        maxDamage = -450,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_SMALLCLOUDS,
    },
}
monster.defenses = {
    defense = 80,
    armor = 80,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 350,
        maxDamage = 600,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 15,
        duration = 5000,
        effect = CONST_ME_HITAREA,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 300,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = 5},
    {type = COMBAT_FIREDAMAGE, percent = 5},
    {type = COMBAT_ENERGYDAMAGE, percent = 5},
    {type = COMBAT_ICEDAMAGE, percent = 5},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Aktat Roshok! Marruk!", yell = false},
    {text = "I will eat you in your sleep.", yell = false},
    {text = "I am the darkness around you...", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 99960,
        maxCount = 197,
    },
    {
        id = 2152,
        chance = 99960,
        maxCount = 9,
    },
    {
        id = 7590,
        chance = 34849,
        maxCount = 2,
    },
    {
        id = 8473,
        chance = 21811,
    },
    {
        id = 18419,
        chance = 16700,
    },
    {
        id = 22538,
        chance = 16660,
    },
    {
        id = 2150,
        chance = 14889,
        maxCount = 3,
    },
    {
        id = 9970,
        chance = 14447,
        maxCount = 3,
    },
    {
        id = 2149,
        chance = 13561,
        maxCount = 3,
    },
    {
        id = 2147,
        chance = 12797,
        maxCount = 3,
    },
    {
        id = 18418,
        chance = 12274,
    },
    {
        id = 22537,
        chance = 12233,
    },
    {
        id = 18413,
        chance = 8974,
    },
    {
        id = 5909,
        chance = 4024,
    },
    {
        id = 5912,
        chance = 3461,
    },
    {
        id = 2476,
        chance = 2294,
    },
    {
        id = 5895,
        chance = 2173,
    },
    {
        id = 22363,
        chance = 2093,
    },
    {
        id = 2393,
        chance = 1650,
    },
    {
        id = 2475,
        chance = 1650,
    },
    {
        id = 2656,
        chance = 1207,
    },
    {
        id = 5911,
        chance = 1127,
    },
    {
        id = 22396,
        chance = 805,
    },
}

mtype:register(monster)
