local mtype = Game.createMonsterType("Lost Soul")
local monster = {}

monster.name = "Lost Soul"
monster.description = "a lost soul"
monster.experience = 4000
monster.race = "undead"
monster.maxHealth = 5800
monster.health = 5800
monster.speed = 380
monster.manaCost = 0
monster.corpse = 6310
monster.outfit = { lookType = 232 }
monster.runHealth = 450
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
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -420,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        minDamage = -40,
        maxDamage = -210,
        length = 3,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        radius = 6,
        target = false,
        speed = -800,
        duration = 4000,
        effect = CONST_ME_SMALLCLOUDS,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 50},
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Forgive Meeeee!", yell = false},
    {text = "Mouuuurn meeee!", yell = false},
    {text = "Help meeee!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 196,
    },
    {
        id = 2152,
        chance = 100000,
        maxCount = 7,
    },
    {
        id = 2260,
        chance = 36842,
        maxCount = 3,
    },
    {
        id = 11233,
        chance = 33684,
    },
    {
        id = 7590,
        chance = 17193,
        maxCount = 2,
    },
    {
        id = 2144,
        chance = 15439,
        maxCount = 3,
    },
    {
        id = 5944,
        chance = 15439,
    },
    {
        id = 2143,
        chance = 14035,
        maxCount = 3,
    },
    {
        id = 7591,
        chance = 8421,
        maxCount = 2,
    },
    {
        id = 6500,
        chance = 5614,
    },
    {
        id = 5806,
        chance = 3860,
    },
    {
        id = 9810,
        chance = 3158,
    },
    {
        id = 2197,
        chance = 2807,
    },
    {
        id = 6300,
        chance = 2456,
    },
    {
        id = 2133,
        chance = 1404,
    },
    {
        id = 6526,
        chance = 1404,
    },
    {
        id = 2156,
        chance = 1053,
    },
    {
        id = 7407,
        chance = 702,
    },
    {
        id = 2436,
        chance = 702,
    },
    {
        id = 7413,
        chance = 351,
    },
}

mtype:register(monster)
