local mtype = Game.createMonsterType("Green Djinn")
local monster = {}

monster.name = "Green Djinn"
monster.description = "a green djinn"
monster.experience = 215
monster.race = "blood"
monster.maxHealth = 330
monster.health = 330
monster.speed = 220
monster.manaCost = 0
monster.corpse = 6016
monster.outfit = { lookType = 51 }
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
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -110,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -45,
        maxDamage = -80,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -50,
        maxDamage = -105,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        range = 7,
        duration = 5000,
        shootEffect = CONST_ANI_ENERGY,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 1,
        range = 7,
        duration = 4000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "djinn electrify",
        interval = 2000,
        chance = 15,
        range = 5,
    },
    {
        name = "djinn cancel invisibility",
        interval = 2000,
        chance = 10,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 50},
    {type = COMBAT_FIREDAMAGE, percent = 80},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -13},
    {type = COMBAT_DEATHDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 1000,
    chance = 5,
    {text = "Good wishes are for fairytales", yell = false},
    {text = "I wish you a merry trip to hell!", yell = false},
    {text = "Muhahahaha!", yell = false},
    {text = "I grant you a deathwish!", yell = false},
    {text = "Tell me your last wish!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 88234,
        maxCount = 115,
    },
    {
        id = 3607,
        chance = 24327,
    },
    {
        id = 7378,
        chance = 5046,
        maxCount = 2,
    },
    {
        id = 2149,
        chance = 2933,
        maxCount = 4,
    },
    {
        id = 1965,
        chance = 2295,
    },
    {
        id = 12412,
        chance = 2121,
    },
    {
        id = 5910,
        chance = 2005,
    },
    {
        id = 2747,
        chance = 953,
    },
    {
        id = 2063,
        chance = 870,
    },
    {
        id = 7620,
        chance = 481,
    },
    {
        id = 2663,
        chance = 116,
    },
}

mtype:register(monster)
