local mtype = Game.createMonsterType("Nightmare")
local monster = {}

monster.name = "Nightmare"
monster.description = "a nightmare"
monster.experience = 1666
monster.race = "blood"
monster.maxHealth = 2700
monster.health = 2700
monster.speed = 464
monster.manaCost = 0
monster.corpse = 6340
monster.outfit = { lookType = 245 }
monster.runHealth = 300
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -150,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -120,
        maxDamage = -170,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_DEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -150,
        maxDamage = -350,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 60,
        maxDamage = 100,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 420,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Close your eyes... I want to show you something.", yell = false},
    {text = "I will haunt you forever!", yell = false},
    {text = "Pffffrrrrrrrrrrrr.", yell = false},
    {text = "I will make you scream.", yell = false},
    {text = "Take a ride with me.", yell = false},
    {text = "Weeeheeheeeheee!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 97147,
        maxCount = 155,
    },
    {
        id = 2671,
        chance = 29615,
    },
    {
        id = 6558,
        chance = 20577,
    },
    {
        id = 5944,
        chance = 20224,
    },
    {
        id = 11223,
        chance = 15577,
    },
    {
        id = 11229,
        chance = 10224,
    },
    {
        id = 6500,
        chance = 9583,
    },
    {
        id = 2152,
        chance = 2660,
        maxCount = 3,
    },
    {
        id = 2532,
        chance = 1122,
    },
    {
        id = 6300,
        chance = 1058,
    },
    {
        id = 2547,
        chance = 1026,
        maxCount = 4,
    },
    {
        id = 2477,
        chance = 833,
    },
    {
        id = 6526,
        chance = 545,
    },
    {
        id = 2195,
        chance = 288,
    },
    {
        id = 5669,
        chance = 160,
    },
    {
        id = 2454,
        chance = 96,
    },
}

mtype:register(monster)
