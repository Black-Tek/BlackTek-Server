local mtype = Game.createMonsterType("Elf Arcanist")
local monster = {}

monster.name = "Elf Arcanist"
monster.description = "an elf arcanist"
monster.experience = 175
monster.race = "blood"
monster.maxHealth = 220
monster.health = 220
monster.speed = 220
monster.manaCost = 0
monster.corpse = 6011
monster.outfit = { lookType = 63 }
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
        maxDamage = -35,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -70,
        shootEffect = CONST_ANI_ARROW,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -30,
        maxDamage = -50,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -70,
        maxDamage = -85,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 40,
        maxDamage = 60,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 50},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Feel my wrath!", yell = false},
    {text = "For the Daughter of the Stars!", yell = false},
    {text = "I'll bring balance upon you!", yell = false},
    {text = "Tha'shi Cenath!", yell = false},
    {text = "Vihil Ealuel!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 37127,
        maxCount = 49,
    },
    {
        id = 1949,
        chance = 30500,
    },
    {
        id = 2682,
        chance = 21814,
    },
    {
        id = 2260,
        chance = 17953,
    },
    {
        id = 2689,
        chance = 13899,
    },
    {
        id = 10552,
        chance = 10070,
    },
    {
        id = 12421,
        chance = 7786,
    },
    {
        id = 2652,
        chance = 7294,
    },
    {
        id = 2544,
        chance = 6127,
        maxCount = 3,
    },
    {
        id = 2802,
        chance = 4846,
    },
    {
        id = 7618,
        chance = 4050,
    },
    {
        id = 7589,
        chance = 2991,
    },
    {
        id = 2047,
        chance = 2192,
    },
    {
        id = 2198,
        chance = 2047,
    },
    {
        id = 5922,
        chance = 2047,
    },
    {
        id = 2189,
        chance = 1126,
    },
    {
        id = 2177,
        chance = 1059,
    },
    {
        id = 2642,
        chance = 974,
    },
    {
        id = 2600,
        chance = 951,
    },
    {
        id = 2747,
        chance = 803,
    },
    {
        id = 2154,
        chance = 48,
    },
}

mtype:register(monster)
