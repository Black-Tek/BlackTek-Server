local mtype = Game.createMonsterType("The Horned Fox")
local monster = {}

monster.name = "The Horned Fox"
monster.description = "The Horned Fox"
monster.experience = 300
monster.race = "blood"
monster.maxHealth = 265
monster.health = 265
monster.speed = 210
monster.manaCost = 0
monster.corpse = 5983
monster.outfit = { lookType = 202 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -122,
    },
    {
        name = "physical",
        interval = 1000,
        chance = 25,
        range = 7,
        minDamage = -50,
        maxDamage = -120,
        shootEffect = CONST_ANI_BOLT,
    },
    {
        name = "poisoncondition",
        interval = 1000,
        chance = 17,
        range = 7,
        minDamage = -50,
        maxDamage = -120,
        shootEffect = CONST_ANI_BOLT,
    },
}
monster.defenses = {
    defense = 16,
    armor = 16,
    {
        name = "healing",
        interval = 1000,
        chance = 15,
        minDamage = 25,
        maxDamage = 75,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "invisible",
        interval = 1000,
        chance = 10,
        duration = 2000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = -1},
    {type = COMBAT_ICEDAMAGE, percent = -1},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.maxSummons = 6
monster.summons = {
    {name = "minotaur archer", interval = 1000, chance = 13, max = 2},
    {name = "minotaur guard", interval = 1000, chance = 13, max = 2},
    {name = "minotaur mage", interval = 1000, chance = 13, max = 2},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "You will never get me!", yell = false},
    {text = "I'll be back!", yell = false},
    {text = "Catch me, if you can!", yell = false},
    {text = "Help me, boys!", yell = false},
}
monster.loot = {
    {
        id = 5804,
        chance = 100000,
    },
    {
        id = 2148,
        chance = 96875,
        maxCount = 99,
    },
    {
        id = 5878,
        chance = 96875,
    },
    {
        id = 12428,
        chance = 93750,
        maxCount = 2,
    },
    {
        id = 12438,
        chance = 81250,
    },
    {
        id = 7363,
        chance = 46875,
        maxCount = 14,
    },
    {
        id = 2465,
        chance = 28125,
    },
    {
        id = 2666,
        chance = 15625,
    },
    {
        id = 2513,
        chance = 12500,
    },
    {
        id = 2502,
        chance = 9375,
    },
    {
        id = 7588,
        chance = 9375,
    },
    {
        id = 2580,
        chance = 6250,
    },
    {
        id = 2387,
        chance = 3125,
    },
}

mtype:register(monster)
