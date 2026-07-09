local mtype = Game.createMonsterType("Dragon Lord")
local monster = {}

monster.name = "Dragon Lord"
monster.description = "a dragon lord"
monster.experience = 2100
monster.race = "blood"
monster.maxHealth = 1900
monster.health = 1900
monster.speed = 200
monster.manaCost = 0
monster.corpse = 5984
monster.outfit = { lookType = 39 }
monster.runHealth = 300
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
    illusionable = true,
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
        maxDamage = -230,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -100,
        maxDamage = -200,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "firefield",
        interval = 2000,
        chance = 10,
        range = 7,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        minDamage = -150,
        maxDamage = -230,
        length = 8,
        spread = 3,
        effect = CONST_ME_FIREAREA,
    },
}
monster.defenses = {
    defense = 35,
    armor = 35,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 57,
        maxDamage = 93,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 80},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "fire", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "ZCHHHHH", yell = true},
    {text = "YOU WILL BURN!", yell = true},
}
monster.loot = {
    {
        id = 2148,
        chance = 95258,
        maxCount = 246,
    },
    {
        id = 2672,
        chance = 79757,
    },
    {
        id = 2796,
        chance = 12120,
    },
    {
        id = 7378,
        chance = 9139,
        maxCount = 3,
    },
    {
        id = 1976,
        chance = 9090,
    },
    {
        id = 2547,
        chance = 6565,
        maxCount = 7,
    },
    {
        id = 2167,
        chance = 5072,
    },
    {
        id = 2146,
        chance = 4968,
    },
    {
        id = 2033,
        chance = 3072,
    },
    {
        id = 5882,
        chance = 1963,
    },
    {
        id = 5948,
        chance = 1022,
    },
    {
        id = 7588,
        chance = 971,
    },
    {
        id = 2177,
        chance = 629,
    },
    {
        id = 2479,
        chance = 382,
    },
    {
        id = 2392,
        chance = 286,
    },
    {
        id = 2528,
        chance = 268,
    },
    {
        id = 2498,
        chance = 233,
    },
    {
        id = 2492,
        chance = 142,
    },
    {
        id = 7402,
        chance = 109,
    },
    {
        id = 7399,
        chance = 93,
    },
}

mtype:register(monster)
