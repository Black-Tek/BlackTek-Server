local mtype = Game.createMonsterType("Lost Berserker")
local monster = {}

monster.name = "Lost Berserker"
monster.description = "a lost berserker"
monster.experience = 4400
monster.race = "blood"
monster.maxHealth = 5900
monster.health = 5900
monster.speed = 300
monster.manaCost = 0
monster.corpse = 17416
monster.outfit = { lookType = 496 }
monster.changeTarget = {
    interval = 4000,
    chance = 15,
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -501,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -300,
        shootEffect = CONST_ANI_WHIRLWINDAXE,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -250,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_EXPLOSION,
        effect = CONST_ME_EXPLOSIONAREA,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        minDamage = -150,
        maxDamage = -250,
        radius = 5,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        radius = 2,
        target = false,
        speed = -800,
        duration = 20000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        radius = 4,
        target = true,
        duration = 6000,
        effect = CONST_ME_STUN,
    },
}
monster.defenses = {
    defense = 80,
    armor = 80,
    {
        name = "invisible",
        interval = 2000,
        chance = 5,
        duration = 3000,
        effect = CONST_ME_TELEPORT,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 17},
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = 40},
    {type = COMBAT_DEATHDAMAGE, percent = 15},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Kill! Kill! Kill!", yell = false},
    {text = "Death! Death! Death!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 200,
    },
    {
        id = 2152,
        chance = 99965,
        maxCount = 9,
    },
    {
        id = 2789,
        chance = 15482,
        maxCount = 2,
    },
    {
        id = 7590,
        chance = 14232,
    },
    {
        id = 7591,
        chance = 13497,
    },
    {
        id = 5880,
        chance = 8663,
    },
    {
        id = 9970,
        chance = 8033,
        maxCount = 2,
    },
    {
        id = 18436,
        chance = 7974,
        maxCount = 10,
    },
    {
        id = 18417,
        chance = 7542,
        maxCount = 2,
    },
    {
        id = 18421,
        chance = 6667,
    },
    {
        id = 18418,
        chance = 4752,
    },
    {
        id = 2114,
        chance = 3783,
    },
    {
        id = 18414,
        chance = 3596,
    },
    {
        id = 2213,
        chance = 2347,
    },
    {
        id = 2430,
        chance = 2113,
    },
    {
        id = 13757,
        chance = 1880,
    },
    {
        id = 2515,
        chance = 1296,
    },
    {
        id = 11339,
        chance = 992,
    },
    {
        id = 2528,
        chance = 899,
    },
    {
        id = 7452,
        chance = 794,
    },
    {
        id = 5904,
        chance = 724,
    },
    {
        id = 2529,
        chance = 630,
    },
    {
        id = 7886,
        chance = 619,
    },
    {
        id = 7427,
        chance = 479,
    },
    {
        id = 2432,
        chance = 409,
    },
    {
        id = 2498,
        chance = 198,
    },
}

mtype:register(monster)
