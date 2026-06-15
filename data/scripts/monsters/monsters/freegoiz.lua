local mtype = Game.createMonsterType("Freegoiz")
local monster = {}

monster.name = "Freegoiz"
monster.description = "Freegoiz"
monster.experience = 35000
monster.race = "fire"
monster.maxHealth = 80000
monster.health = 80000
monster.speed = 380
monster.manaCost = 0
monster.corpse = 6068
monster.outfit = {
    lookType = 12,
    lookHead = 38,
    lookBody = 114,
    lookLegs = 0,
    lookFeet = 94,
}
monster.changeTarget = {
    interval = 5000,
    chance = 20,
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
        maxDamage = -2520,
    },
    {
        name = "death",
        interval = 1000,
        chance = 8,
        minDamage = -800,
        maxDamage = -1900,
        radius = 9,
        target = false,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 12,
        radius = 6,
        target = false,
        speed = -850,
        duration = 60000,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "physical",
        interval = 1000,
        chance = 10,
        minDamage = -600,
        maxDamage = -1450,
        radius = 5,
        target = false,
        effect = CONST_ME_HITAREA,
    },
    {
        name = "fire",
        interval = 3000,
        chance = 13,
        range = 7,
        minDamage = -300,
        maxDamage = -800,
        radius = 7,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "manadrain",
        interval = 3000,
        chance = 8,
        minDamage = -600,
        maxDamage = -700,
        radius = 10,
        target = false,
        effect = CONST_ME_ENERGYAREA,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 9,
        minDamage = -400,
        maxDamage = -800,
        length = 8,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "poisoncondition",
        interval = 5000,
        chance = 18,
        minDamage = -800,
        maxDamage = -1000,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 6,
        minDamage = -600,
        maxDamage = -1200,
        radius = 14,
        target = false,
        effect = CONST_ME_MAGIC_GREEN,
    },
}
monster.defenses = {
    defense = 145,
    armor = 188,
    {
        name = "healing",
        interval = 1000,
        chance = 15,
        minDamage = 1000,
        maxDamage = 3000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 8,
        speed = 480,
        duration = 6000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "BOW TO THE POWER OF THE RUTHLESS SEVEN!", yell = true},
    {text = "DESTRUCTION!", yell = true},
    {text = "CHAOS!", yell = true},
    {text = "DEATH TO ALL!", yell = true},
}
monster.loot = {
    {
        id = "purple tome",
        chance = 2600,
    },
    {
        id = "golden mug",
        chance = 7500,
    },
    {
        id = "teddy bear",
        chance = 14500,
    },
    {
        id = "ring of the sky",
        chance = 3500,
    },
    {
        id = 2124,
        chance = 5500,
    },
    {
        id = "crystal necklace",
        chance = 1500,
    },
    {
        id = 2142,
        chance = 3500,
    },
    {
        id = "white pearl",
        chance = 12500,
        maxCount = 15,
    },
    {
        id = "black pearl",
        chance = 15000,
        maxCount = 15,
    },
    {
        id = "small diamond",
        chance = 9500,
        maxCount = 5,
    },
    {
        id = "small sapphire",
        chance = 13500,
        maxCount = 10,
    },
    {
        id = "gold coin",
        chance = 99900,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 88800,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 77700,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 66600,
        maxCount = 100,
    },
    {
        id = "small emerald",
        chance = 15500,
        maxCount = 10,
    },
    {
        id = "small amethyst",
        chance = 13500,
        maxCount = 20,
    },
    {
        id = "talon",
        chance = 14000,
        maxCount = 7,
    },
    {
        id = "green gem",
        chance = 1500,
    },
    {
        id = "blue gem",
        chance = 1500,
    },
    {
        id = 2162,
        chance = 11500,
    },
    {
        id = "might ring",
        chance = 5000,
    },
    {
        id = "stealth ring",
        chance = 9500,
    },
    {
        id = "energy ring",
        chance = 13500,
    },
    {
        id = "silver amulet",
        chance = 13000,
    },
    {
        id = "platinum amulet",
        chance = 4500,
    },
    {
        id = "strange symbol",
        chance = 2500,
    },
    {
        id = "orb",
        chance = 12000,
    },
    {
        id = "life crystal",
        chance = 1000,
    },
    {
        id = 2178,
        chance = 4000,
    },
    {
        id = "gold ring",
        chance = 8000,
    },
    {
        id = "snakebite rod",
        chance = 3500,
    },
    {
        id = "necrotic rod",
        chance = 3500,
    },
    {
        id = "moonlight rod",
        chance = 3500,
    },
    {
        id = "wand of decay",
        chance = 2500,
    },
    {
        id = 2192,
        chance = 2500,
    },
    {
        id = "boots of haste",
        chance = 4000,
    },
    {
        id = "stone skin amulet",
        chance = 4000,
    },
    {
        id = "protection amulet",
        chance = 4500,
    },
    {
        id = "ring of healing",
        chance = 13000,
    },
    {
        id = 2231,
        chance = 9000,
    },
    {
        id = "two handed sword",
        chance = 20000,
    },
    {
        id = "double axe",
        chance = 20000,
    },
    {
        id = "giant sword",
        chance = 12500,
    },
    {
        id = "ice rapier",
        chance = 7500,
    },
    {
        id = "silver dagger",
        chance = 15500,
    },
    {
        id = "golden sickle",
        chance = 4500,
    },
    {
        id = "thunder hammer",
        chance = 13500,
    },
    {
        id = "fire axe",
        chance = 17000,
    },
    {
        id = "dragon hammer",
        chance = 4500,
    },
    {
        id = "skull staff",
        chance = 5000,
    },
    {
        id = "devil helmet",
        chance = 11000,
    },
    {
        id = "golden legs",
        chance = 5000,
    },
    {
        id = "magic plate armor",
        chance = 3000,
    },
    {
        id = 2514,
        chance = 7500,
    },
    {
        id = "demon shield",
        chance = 15500,
    },
    {
        id = 3955,
        chance = 100,
    },
}

mtype:register(monster)
