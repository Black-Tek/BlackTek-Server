local mtype = Game.createMonsterType("Minotaur Hunter")
local monster = {}

monster.name = "Minotaur Hunter"
monster.description = "a minotaur hunter"
monster.experience = 1700
monster.race = "blood"
monster.maxHealth = 1400
monster.health = 1400
monster.speed = 230
monster.manaCost = 0
monster.corpse = 23466
monster.outfit = { lookType = 612 }
monster.runHealth = 300
monster.changeTarget = {
    interval = 2000,
    chance = 11,
}
monster.targetDistance = 4
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
        maxDamage = -150,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 25,
        range = 7,
        minDamage = 0,
        maxDamage = -150,
        shootEffect = CONST_ANI_SPEAR,
        effect = CONST_ME_EXPLOSIONAREA,
    },
    {
        name = "bleedcondition",
        interval = 2000,
        chance = 35,
        range = 7,
        minDamage = -300,
        maxDamage = -400,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_THROWINGKNIFE,
        effect = CONST_ME_HITAREA,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 21,
        range = 7,
        minDamage = -160,
        maxDamage = -260,
        radius = 2,
        target = true,
        shootEffect = CONST_ANI_BURSTARROW,
        effect = CONST_ME_EXPLOSIONHIT,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 11,
        minDamage = -35,
        maxDamage = -150,
        radius = 4,
        target = false,
        effect = CONST_ME_EXPLOSIONAREA,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 13,
        minDamage = 95,
        maxDamage = 180,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 11,
        speed = 520,
        duration = 5000,
        effect = CONST_ME_POFF,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "You are marked for death!", yell = false},
    {text = "This time the prey is you!", yell = false},
    {text = "You are hunted!", yell = false},
    {text = "Bullseye!", yell = false},
    {text = "You'll make a fine trophy!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 99464,
        maxCount = 200,
    },
    {
        id = 2152,
        chance = 35026,
        maxCount = 3,
    },
    {
        id = 3965,
        chance = 15007,
        maxCount = 5,
    },
    {
        id = 2260,
        chance = 14990,
        maxCount = 2,
    },
    {
        id = 7378,
        chance = 11751,
        maxCount = 5,
    },
    {
        id = 7589,
        chance = 10135,
        maxCount = 2,
    },
    {
        id = 7588,
        chance = 9908,
        maxCount = 2,
    },
    {
        id = 5944,
        chance = 7011,
    },
    {
        id = 12428,
        chance = 5046,
        maxCount = 2,
    },
    {
        id = 5878,
        chance = 4850,
    },
    {
        id = 2147,
        chance = 2550,
        maxCount = 3,
    },
    {
        id = 2150,
        chance = 2454,
        maxCount = 3,
    },
    {
        id = 2165,
        chance = 2400,
    },
    {
        id = 5910,
        chance = 1537,
    },
    {
        id = 5912,
        chance = 1506,
    },
    {
        id = 5911,
        chance = 1484,
    },
    {
        id = 23546,
        chance = 746,
    },
    {
        id = 2154,
        chance = 556,
    },
    {
        id = 2156,
        chance = 535,
    },
    {
        id = 7401,
        chance = 162,
    },
    {
        id = 23537,
        chance = 104,
    },
}

mtype:register(monster)
