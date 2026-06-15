local mtype = Game.createMonsterType("Lizard Snakecharmer")
local monster = {}

monster.name = "Lizard Snakecharmer"
monster.description = "a lizard snakecharmer"
monster.experience = 210
monster.race = "blood"
monster.maxHealth = 325
monster.health = 325
monster.speed = 184
monster.manaCost = 0
monster.corpse = 6041
monster.outfit = { lookType = 115 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = true,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -30,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -100,
        maxDamage = -200,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -50,
        maxDamage = -110,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_GREEN_RINGS,
    },
}
monster.defenses = {
    defense = 22,
    armor = 22,
    {
        name = "healing",
        interval = 2000,
        chance = 50,
        minDamage = 50,
        maxDamage = 100,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 20,
        duration = 3000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.maxSummons = 6
monster.summons = {
    {name = "cobra", interval = 2000, chance = 20, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "I ssssmell warm blood!", yell = false},
    {text = "Shhhhhhhh", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 83740,
        maxCount = 55,
    },
    {
        id = 3077,
        chance = 45000,
    },
    {
        id = "cape",
        chance = 8640,
    },
    {
        id = "life crystal",
        chance = 1430,
    },
    {
        id = "lizard leather",
        chance = 1320,
    },
    {
        id = "lizard scale",
        chance = 1000,
    },
    {
        id = "terra rod",
        chance = 980,
    },
    {
        id = "mana potion",
        chance = 700,
    },
    {
        id = "small amethyst",
        chance = 370,
    },
    {
        id = "life ring",
        chance = 190,
    },
    {
        id = "snakebite rod",
        chance = 120,
    },
    {
        id = "charmer's tiara",
        chance = 100,
    },
    {
        id = "yellow gem",
        chance = 30,
    },
}

mtype:register(monster)
