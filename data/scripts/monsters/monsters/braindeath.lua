local mtype = Game.createMonsterType("Braindeath")
local monster = {}

monster.name = "Braindeath"
monster.description = "a braindeath"
monster.experience = 985
monster.race = "venom"
monster.maxHealth = 1225
monster.health = 1225
monster.speed = 218
monster.manaCost = 0
monster.corpse = 7256
monster.outfit = { lookType = 256 }
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
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = true,
    canWalkOnFire = true,
    canWalkOnPoison = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -100,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -93,
        maxDamage = -170,
        shootEffect = CONST_ANI_ENERGY,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -75,
        maxDamage = -125,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -85,
        maxDamage = -170,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -65,
        maxDamage = -125,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -75,
        maxDamage = -85,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -155,
    },
}
monster.defenses = {
    defense = 12,
    armor = 12,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 150,
        maxDamage = 200,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 260,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 15},
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = -15},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Vampire", interval = 2000, chance = 10, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "You have disturbed my thoughts!", yell = false},
    {text = "Let me turn you into something more useful!", yell = false},
    {text = "Let me taste your brain!", yell = false},
    {text = "You will be punished!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 99448,
        maxCount = 90,
    },
    {
        id = 2450,
        chance = 14985,
    },
    {
        id = 7364,
        chance = 9593,
        maxCount = 4,
    },
    {
        id = 2509,
        chance = 5899,
    },
    {
        id = 10580,
        chance = 5041,
    },
    {
        id = 5898,
        chance = 2967,
    },
    {
        id = 2423,
        chance = 1965,
    },
    {
        id = 7407,
        chance = 1446,
    },
    {
        id = 2175,
        chance = 958,
    },
    {
        id = 7452,
        chance = 178,
    },
    {
        id = 2518,
        chance = 102,
    },
    {
        id = 3972,
        chance = 99,
    },
}

mtype:register(monster)
