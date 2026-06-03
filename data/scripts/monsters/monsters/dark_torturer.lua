local mtype = Game.createMonsterType("Dark Torturer")
local monster = {}

monster.name = "Dark Torturer"
monster.description = "a dark torturer"
monster.experience = 4650
monster.race = "blood"
monster.maxHealth = 7350
monster.health = 7350
monster.speed = 370
monster.manaCost = 0
monster.corpse = 6328
monster.outfit = { lookType = 234 }
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
    illusionable = false,
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
        maxDamage = -500,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -781,
        shootEffect = CONST_ANI_THROWINGKNIFE,
    },
    {
        name = "dark torturer skill reducer",
        interval = 2000,
        chance = 5,
    },
}
monster.defenses = {
    defense = 45,
    armor = 45,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 200,
        maxDamage = 250,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 30},
    {type = COMBAT_EARTHDAMAGE, percent = 90},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "You like it, don't you?", yell = false},
    {text = "IahaEhheAie!", yell = false},
    {text = "It's party time!", yell = false},
    {text = "Harrr, Harrr!", yell = false},
    {text = "The torturer is in!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 199,
    },
    {
        id = 2671,
        chance = 65733,
    },
    {
        id = 2152,
        chance = 57202,
        maxCount = 8,
    },
    {
        id = 6558,
        chance = 35009,
    },
    {
        id = 5944,
        chance = 24088,
    },
    {
        id = 7590,
        chance = 15083,
        maxCount = 2,
    },
    {
        id = 7591,
        chance = 9602,
        maxCount = 2,
    },
    {
        id = 6500,
        chance = 8860,
    },
    {
        id = 2558,
        chance = 5316,
    },
    {
        id = 2645,
        chance = 5007,
    },
    {
        id = 9971,
        chance = 3132,
    },
    {
        id = 5022,
        chance = 2823,
        maxCount = 2,
    },
    {
        id = 5480,
        chance = 2061,
    },
    {
        id = 6300,
        chance = 1958,
    },
    {
        id = 7368,
        chance = 1772,
        maxCount = 5,
    },
    {
        id = 5801,
        chance = 845,
    },
    {
        id = 7412,
        chance = 762,
    },
    {
        id = 7388,
        chance = 536,
    },
    {
        id = 2470,
        chance = 82,
    },
}

mtype:register(monster)
