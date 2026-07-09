local mtype = Game.createMonsterType("Enslaved Dwarf")
local monster = {}

monster.name = "Enslaved Dwarf"
monster.description = "an enslaved dwarf"
monster.experience = 2700
monster.race = "blood"
monster.maxHealth = 3800
monster.health = 3800
monster.speed = 270
monster.manaCost = 0
monster.corpse = 17408
monster.outfit = { lookType = 494 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 70
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
    canWalkOnPoison = false,
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
        maxDamage = -340,
        shootEffect = CONST_ANI_LARGEROCK,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -250,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_EXPLOSION,
        effect = CONST_ME_EXPLOSIONHIT,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 20,
        radius = 5,
        target = false,
        duration = 6000,
        effect = CONST_ME_BLOCKHIT,
    },
    {
        name = "enslaved dwarf skill reducer 1",
        interval = 2000,
        chance = 5,
    },
    {
        name = "enslaved dwarf skill reducer 2",
        interval = 2000,
        chance = 5,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 396,
        maxDamage = 478,
        effect = CONST_ME_MAGIC_GREEN,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = -3},
    {type = COMBAT_ENERGYDAMAGE, percent = 5},
    {type = COMBAT_EARTHDAMAGE, percent = 30},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = 15},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Bark!", yell = false},
    {text = "Blood!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 149,
    },
    {
        id = 2152,
        chance = 100000,
        maxCount = 6,
    },
    {
        id = 2789,
        chance = 14724,
        maxCount = 2,
    },
    {
        id = 5880,
        chance = 10357,
    },
    {
        id = 2150,
        chance = 10328,
        maxCount = 2,
    },
    {
        id = 2149,
        chance = 9826,
        maxCount = 2,
    },
    {
        id = 7590,
        chance = 6019,
    },
    {
        id = 7591,
        chance = 5754,
    },
    {
        id = 11227,
        chance = 5341,
    },
    {
        id = 18436,
        chance = 5193,
        maxCount = 5,
    },
    {
        id = 18417,
        chance = 4721,
        maxCount = 2,
    },
    {
        id = 2391,
        chance = 3924,
    },
    {
        id = 18420,
        chance = 3747,
    },
    {
        id = 2532,
        chance = 3305,
    },
    {
        id = 18416,
        chance = 3275,
    },
    {
        id = 2515,
        chance = 2567,
    },
    {
        id = 2208,
        chance = 2006,
    },
    {
        id = 18415,
        chance = 1977,
    },
    {
        id = 7454,
        chance = 1564,
    },
    {
        id = 13757,
        chance = 1121,
    },
    {
        id = 7413,
        chance = 1121,
    },
    {
        id = 2528,
        chance = 472,
    },
    {
        id = 7437,
        chance = 384,
    },
    {
        id = 2475,
        chance = 325,
    },
}

mtype:register(monster)
