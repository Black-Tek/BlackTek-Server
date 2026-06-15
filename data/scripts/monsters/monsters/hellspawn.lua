local mtype = Game.createMonsterType("Hellspawn")
local monster = {}

monster.name = "Hellspawn"
monster.description = "a hellspawn"
monster.experience = 2550
monster.race = "fire"
monster.maxHealth = 3500
monster.health = 3500
monster.speed = 344
monster.manaCost = 0
monster.corpse = 9923
monster.outfit = { lookType = 322 }
monster.changeTarget = {
    interval = 4000,
    chance = 15,
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
    canWalkOnEnergy = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -350,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 20,
        minDamage = -150,
        maxDamage = -175,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREATTACK,
    },
    {
        name = "hellspawn soulfire",
        interval = 2000,
        chance = 10,
        range = 5,
    },
}
monster.defenses = {
    defense = 44,
    armor = 44,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 120,
        maxDamage = 230,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 270,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_EARTHDAMAGE, percent = 80},
    {type = COMBAT_FIREDAMAGE, percent = 40},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = 30},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Your fragile bones are like toothpicks to me.", yell = false},
    {text = "You little weasel will not live to see another day.", yell = false},
    {text = "I'm just a messenger of what's yet to come.", yell = false},
    {text = "HRAAAAAAAAAAAAAAAARRRR", yell = true},
    {text = "I'm taking you down with me!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 92683,
        maxCount = 236,
    },
    {
        id = 7591,
        chance = 30215,
    },
    {
        id = 11221,
        chance = 14587,
    },
    {
        id = 2394,
        chance = 7720,
    },
    {
        id = 6500,
        chance = 7483,
    },
    {
        id = 8473,
        chance = 7317,
    },
    {
        id = 7368,
        chance = 7151,
        maxCount = 2,
    },
    {
        id = 2513,
        chance = 6938,
    },
    {
        id = 2788,
        chance = 6299,
        maxCount = 2,
    },
    {
        id = 9970,
        chance = 4736,
        maxCount = 3,
    },
    {
        id = 9809,
        chance = 2226,
    },
    {
        id = 2477,
        chance = 2202,
    },
    {
        id = 2475,
        chance = 1397,
    },
    {
        id = 7452,
        chance = 521,
    },
    {
        id = 7439,
        chance = 497,
    },
    {
        id = 9969,
        chance = 213,
    },
    {
        id = 9810,
        chance = 189,
    },
    {
        id = 9948,
        chance = 71,
    },
    {
        id = 7421,
        chance = 24,
    },
}

mtype:register(monster)
