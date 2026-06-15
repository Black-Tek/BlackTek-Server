local mtype = Game.createMonsterType("General Murius")
local monster = {}

monster.name = "General Murius"
monster.description = "General Murius"
monster.experience = 450
monster.race = "blood"
monster.maxHealth = 550
monster.health = 550
monster.speed = 240
monster.manaCost = 0
monster.corpse = 23462
monster.outfit = { lookType = 611 }
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
        maxDamage = -220,
    },
    {
        name = "physical",
        interval = 1000,
        chance = 12,
        range = 7,
        minDamage = -50,
        maxDamage = -120,
        shootEffect = CONST_ANI_BOLT,
    },
    {
        name = "physical",
        interval = 1000,
        chance = 10,
        minDamage = -50,
        maxDamage = -80,
        radius = 3,
        target = false,
        effect = CONST_ME_HITAREA,
    },
}
monster.defenses = {
    defense = 22,
    armor = 16,
    {
        name = "healing",
        interval = 1000,
        chance = 15,
        minDamage = 50,
        maxDamage = 100,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.maxSummons = 4
monster.summons = {
    {name = "Minotaur Archer", interval = 1000, chance = 15, max = 2},
    {name = "Minotaur Guard", interval = 1000, chance = 12, max = 2},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Feel the power of the Mooh'Tah!", yell = false},
    {text = "You will get what you deserve!", yell = false},
    {text = "For the king!", yell = false},
    {text = "Guards!", yell = false},
}
monster.loot = {
    {
        id = 12428,
        chance = 100000,
        maxCount = 2,
    },
    {
        id = 5878,
        chance = 100000,
    },
    {
        id = 2148,
        chance = 93333,
        maxCount = 97,
    },
    {
        id = 2152,
        chance = 93333,
        maxCount = 3,
    },
    {
        id = 2387,
        chance = 80000,
    },
    {
        id = 2465,
        chance = 66667,
    },
    {
        id = 2513,
        chance = 33333,
    },
    {
        id = 7363,
        chance = 33333,
        maxCount = 11,
    },
    {
        id = 2666,
        chance = 26667,
    },
    {
        id = 2648,
        chance = 20000,
    },
    {
        id = 7401,
        chance = 20000,
    },
    {
        id = 2547,
        chance = 20000,
        maxCount = 7,
    },
    {
        id = 2580,
        chance = 6667,
    },
    {
        id = 7588,
        chance = 6667,
    },
}

mtype:register(monster)
