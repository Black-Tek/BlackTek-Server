local mtype = Game.createMonsterType("Lizard Dragon Priest")
local monster = {}

monster.name = "Lizard Dragon Priest"
monster.description = "a lizard dragon priest"
monster.experience = 1320
monster.race = "blood"
monster.maxHealth = 1450
monster.health = 1450
monster.speed = 220
monster.manaCost = 0
monster.corpse = 11280
monster.outfit = { lookType = 339 }
monster.runHealth = 50
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
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -50,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -125,
        maxDamage = -190,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREATTACK,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -320,
        maxDamage = -400,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 30,
        minDamage = 200,
        maxDamage = 300,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 15,
        duration = 4000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 45},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Dragon Hatchling", interval = 2000, chance = 20, max = 2},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "I ssssmell warm blood!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 5000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 4400,
        maxCount = 88,
    },
    {
        id = "small amethyst",
        chance = 4900,
        maxCount = 3,
    },
    {
        id = "platinum coin",
        chance = 4090,
        maxCount = 2,
    },
    {
        id = "yellow gem",
        chance = 1000,
    },
    {
        id = "life ring",
        chance = 770,
    },
    {
        id = "terra rod",
        chance = 1001,
    },
    {
        id = "wand of inferno",
        chance = 1480,
    },
    {
        id = "lizard leather",
        chance = 980,
    },
    {
        id = "lizard scale",
        chance = 1130,
    },
    {
        id = "strong mana potion",
        chance = 12110,
    },
    {
        id = "great mana potion",
        chance = 7940,
    },
    {
        id = "focus cape",
        chance = 660,
    },
    {
        id = "bunch of ripe rice",
        chance = 960,
    },
    {
        id = "Zaoan shoes",
        chance = 450,
    },
    {
        id = "Zaoan robe",
        chance = 300,
    },
    {
        id = "dragon priest's wandtip",
        chance = 9910,
    },
}

mtype:register(monster)
