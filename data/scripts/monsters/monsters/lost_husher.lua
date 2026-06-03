local mtype = Game.createMonsterType("Lost Husher")
local monster = {}

monster.name = "Lost Husher"
monster.description = "a lost husher"
monster.experience = 1800
monster.race = "blood"
monster.maxHealth = 1600
monster.health = 1600
monster.speed = 250
monster.manaCost = 0
monster.corpse = 19964
monster.outfit = { lookType = 537 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
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
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "death",
        interval = 2000,
        chance = 10,
        minDamage = -150,
        maxDamage = -300,
        length = 6,
        effect = CONST_ME_BLACKSMOKE,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        minDamage = -150,
        maxDamage = -250,
        radius = 5,
        target = false,
        effect = CONST_ME_BLACKSMOKE,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -150,
        maxDamage = -200,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_SMALLCLOUDS,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -150,
        maxDamage = -250,
        radius = 2,
        target = true,
        shootEffect = CONST_ANI_SMALLEARTH,
        effect = CONST_ME_MAGIC_GREEN,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        radius = 4,
        target = false,
        duration = 6000,
        effect = CONST_ME_SOUND_RED,
    },
}
monster.defenses = {
    defense = 55,
    armor = 55,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 75,
        maxDamage = 92,
        effect = CONST_ME_YELLOWENERGY,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 15,
        duration = 5000,
        effect = CONST_ME_TELEPORT,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = 25},
    {type = COMBAT_ICEDAMAGE, percent = 15},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Arr far zwar!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 100,
    },
    {
        id = 2152,
        chance = 59669,
        maxCount = 2,
    },
    {
        id = 20135,
        chance = 15336,
    },
    {
        id = 2789,
        chance = 15105,
        maxCount = 2,
    },
    {
        id = 20127,
        chance = 12042,
    },
    {
        id = 13757,
        chance = 11812,
    },
    {
        id = 20130,
        chance = 11608,
    },
    {
        id = 7588,
        chance = 10466,
        maxCount = 3,
    },
    {
        id = 7590,
        chance = 10377,
        maxCount = 2,
    },
    {
        id = 9970,
        chance = 9695,
    },
    {
        id = 20128,
        chance = 9217,
    },
    {
        id = 20129,
        chance = 8996,
    },
    {
        id = 20110,
        chance = 8465,
    },
    {
        id = 20137,
        chance = 7739,
    },
    {
        id = 20111,
        chance = 7677,
    },
    {
        id = 20136,
        chance = 5932,
    },
    {
        id = 2213,
        chance = 2656,
    },
    {
        id = 2515,
        chance = 965,
    },
    {
        id = 11339,
        chance = 885,
    },
    {
        id = 2430,
        chance = 744,
    },
    {
        id = 20109,
        chance = 726,
    },
    {
        id = 7886,
        chance = 620,
    },
    {
        id = 2436,
        chance = 469,
    },
    {
        id = 7452,
        chance = 283,
    },
    {
        id = 2432,
        chance = 266,
    },
    {
        id = 7885,
        chance = 221,
    },
    {
        id = 2528,
        chance = 195,
    },
}

mtype:register(monster)
