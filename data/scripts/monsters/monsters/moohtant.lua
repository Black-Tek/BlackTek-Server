local mtype = Game.createMonsterType("Moohtant")
local monster = {}

monster.name = "Moohtant"
monster.description = "a moohtant"
monster.experience = 2600
monster.race = "blood"
monster.maxHealth = 3200
monster.health = 3200
monster.speed = 260
monster.manaCost = 0
monster.corpse = 23367
monster.outfit = { lookType = 607 }
monster.changeTarget = {
    interval = 2000,
    chance = 3,
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
        attack = 50,
        skill = 110,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 13,
        minDamage = -100,
        maxDamage = -230,
        length = 3,
        effect = CONST_ME_GROUNDSHAKER,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 12,
        minDamage = -100,
        maxDamage = -200,
        radius = 3,
        target = false,
        effect = CONST_ME_GROUNDSHAKER,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 19,
        minDamage = -50,
        maxDamage = -225,
        radius = 5,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -150,
        maxDamage = -235,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_LARGEROCK,
        effect = CONST_ME_EXPLOSIONAREA,
    },
}
monster.defenses = {
    defense = 45,
    armor = 40,
    {
        name = "healing",
        interval = 2000,
        chance = 9,
        minDamage = 50,
        maxDamage = 150,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 1},
    {type = COMBAT_ICEDAMAGE, percent = 15},
    {type = COMBAT_FIREDAMAGE, percent = 1},
    {type = COMBAT_DEATHDAMAGE, percent = 1},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "MOOOOH!", yell = true},
    {text = "Grrrr.", yell = false},
    {text = "Raaaargh!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 100,
    },
    {
        id = 2148,
        chance = 60000,
        maxCount = 95,
    },
    {
        id = 2152,
        chance = 58160,
        maxCount = 2,
    },
    {
        id = 23554,
        chance = 15740,
        maxCount = 2,
    },
    {
        id = 23570,
        chance = 11770,
    },
    {
        id = 7591,
        chance = 7380,
        maxCount = 3,
    },
    {
        id = 7590,
        chance = 7230,
        maxCount = 3,
    },
    {
        id = 2666,
        chance = 6520,
    },
    {
        id = 2147,
        chance = 4680,
        maxCount = 2,
    },
    {
        id = 5878,
        chance = 4110,
    },
    {
        id = 2145,
        chance = 4400,
        maxCount = 2,
    },
    {
        id = 2214,
        chance = 2410,
    },
    {
        id = 5911,
        chance = 1700,
    },
    {
        id = 23544,
        chance = 1560,
    },
    {
        id = 2156,
        chance = 850,
    },
    {
        id = 2154,
        chance = 710,
    },
    {
        id = 7452,
        chance = 430,
    },
    {
        id = 7427,
        chance = 280,
    },
    {
        id = 9971,
        chance = 280,
    },
    {
        id = 7401,
        chance = 280,
    },
}

mtype:register(monster)
