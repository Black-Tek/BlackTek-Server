local mtype = Game.createMonsterType("Minishabaal")
local monster = {}

monster.name = "Minishabaal"
monster.description = "Minishabaal"
monster.experience = 4000
monster.race = "blood"
monster.maxHealth = 6000
monster.health = 6000
monster.speed = 700
monster.manaCost = 0
monster.corpse = 6364
monster.outfit = { lookType = 237 }
monster.runHealth = 350
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    attackable = true,
    hostile = true,
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
        maxDamage = -380,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -80,
        maxDamage = -350,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "fire",
        interval = 3000,
        chance = 34,
        range = 7,
        minDamage = -120,
        maxDamage = -500,
        radius = 2,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "diabolic imp skill reducer",
        interval = 2000,
        chance = 5,
        range = 5,
    },
}
monster.defenses = {
    defense = 29,
    armor = 29,
    {
        name = "healing",
        interval = 1000,
        chance = 50,
        minDamage = 155,
        maxDamage = 255,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 12,
        speed = 800,
        duration = 4000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "invisible",
        interval = 4000,
        chance = 50,
        duration = 4000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 50},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 3
monster.summons = {
    {name = "Diabolic Imp", interval = 2000, chance = 40, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "I had Princess Lumelia as breakfast!", yell = false},
    {text = "Naaa-Nana-Naaa-Naaa!", yell = false},
    {text = "My brother will come and get you for this!", yell = false},
    {text = "Get them Fluffy!", yell = false},
    {text = "He He He!", yell = false},
    {text = "Pftt, Ferumbras such an upstart!", yell = false},
    {text = "My dragon is not that old, it's just second hand!", yell = false},
    {text = "My other dragon is a red one!", yell = false},
    {text = "When I am big I want to become the ruthless eighth!", yell = false},
    {text = "WHERE'S FLUFFY?", yell = false},
    {text = "Muahaha!", yell = false},
}
monster.loot = {
    {
        id = 2136,
        chance = 909,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 40,
    },
    {
        id = "small amethyst",
        chance = 1428,
        maxCount = 2,
    },
    {
        id = "fire axe",
        chance = 666,
    },
    {
        id = "golden legs",
        chance = 180,
    },
    {
        id = 2488,
        chance = 800,
    },
    {
        id = "guardian shield",
        chance = 1333,
    },
    {
        id = "demon shield",
        chance = 200,
    },
    {
        id = "tempest shield",
        chance = 100,
    },
    {
        id = "demon helmet",
        chance = 100,
    },
    {
        id = "pitchfork",
        chance = 2857,
    },
    {
        id = "soul orb",
        chance = 909,
        maxCount = 2,
    },
    {
        id = "demonic essence",
        chance = 1000,
        maxCount = 2,
    },
}

mtype:register(monster)
