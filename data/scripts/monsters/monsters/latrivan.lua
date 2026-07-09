local mtype = Game.createMonsterType("Latrivan")
local monster = {}

monster.name = "Latrivan"
monster.description = "Latrivan"
monster.experience = 10000
monster.race = "fire"
monster.maxHealth = 25000
monster.health = 25000
monster.speed = 390
monster.manaCost = 0
monster.corpse = 8721
monster.outfit = {
    lookType = 12,
    lookHead = 119,
    lookBody = 71,
    lookLegs = 118,
    lookFeet = 34,
}
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 1
monster.staticAttackChance = 85
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -878,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 10,
        minDamage = -50,
        maxDamage = -850,
        length = 7,
        effect = CONST_ME_EXPLOSIONHIT,
    },
}
monster.defenses = {
    defense = 45,
    armor = 35,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = 35},
    {type = COMBAT_DEATHDAMAGE, percent = -12},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "I might reward you for killing my brother ~ with a swift dead!", yell = true},
    {text = "Colateral damage is so fun!", yell = false},
    {text = "Golgordan you fool!", yell = false},
    {text = "We are the brothers of fear!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 273,
    },
    {
        id = 7591,
        chance = 37674,
        maxCount = 2,
    },
    {
        id = 2214,
        chance = 26977,
    },
    {
        id = 2387,
        chance = 19535,
    },
    {
        id = 2165,
        chance = 19535,
    },
    {
        id = 2162,
        chance = 17674,
    },
    {
        id = 2150,
        chance = 16744,
        maxCount = 19,
    },
    {
        id = 2144,
        chance = 15814,
        maxCount = 15,
    },
    {
        id = 2143,
        chance = 15814,
        maxCount = 13,
    },
    {
        id = 2402,
        chance = 14419,
    },
    {
        id = 2170,
        chance = 13953,
    },
    {
        id = 6300,
        chance = 13488,
    },
    {
        id = 2146,
        chance = 13488,
        maxCount = 10,
    },
    {
        id = 2197,
        chance = 13488,
    },
    {
        id = 2149,
        chance = 12558,
        maxCount = 10,
    },
    {
        id = 2520,
        chance = 11628,
    },
    {
        id = 2432,
        chance = 10698,
    },
    {
        id = 9971,
        chance = 10698,
    },
    {
        id = 6500,
        chance = 10233,
    },
    {
        id = 2167,
        chance = 9767,
    },
    {
        id = 2145,
        chance = 8372,
        maxCount = 5,
    },
    {
        id = 2393,
        chance = 6977,
    },
    {
        id = 2396,
        chance = 6977,
    },
    {
        id = 2179,
        chance = 6512,
    },
    {
        id = 2462,
        chance = 5581,
    },
    {
        id = 2164,
        chance = 5581,
    },
    {
        id = 7365,
        chance = 4186,
        maxCount = 8,
    },
    {
        id = 2200,
        chance = 4186,
    },
    {
        id = 2182,
        chance = 4186,
    },
    {
        id = 2436,
        chance = 3256,
    },
    {
        id = 2192,
        chance = 2791,
    },
    {
        id = 2178,
        chance = 2791,
    },
    {
        id = 2186,
        chance = 2791,
    },
    {
        id = 2195,
        chance = 2326,
    },
    {
        id = 2155,
        chance = 2326,
    },
    {
        id = 2188,
        chance = 2326,
    },
    {
        id = 2158,
        chance = 1395,
    },
    {
        id = 2185,
        chance = 1395,
    },
    {
        id = 2470,
        chance = 930,
    },
    {
        id = 2418,
        chance = 930,
    },
    {
        id = 2514,
        chance = 930,
    },
    {
        id = 7368,
        chance = 465,
    },
}

mtype:register(monster)
