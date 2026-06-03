local mtype = Game.createMonsterType("Draken Spellweaver")
local monster = {}

monster.name = "Draken Spellweaver"
monster.description = "a draken spellweaver"
monster.experience = 3100
monster.race = "blood"
monster.maxHealth = 5000
monster.health = 5000
monster.speed = 210
monster.manaCost = 0
monster.corpse = 11316
monster.outfit = { lookType = 340 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 70
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
        maxDamage = -252,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        minDamage = -240,
        maxDamage = -480,
        length = 4,
        spread = 3,
        effect = CONST_ME_EXPLOSIONHIT,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -100,
        maxDamage = -250,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -150,
        maxDamage = -300,
        target = true,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        minDamage = -200,
        maxDamage = -380,
        radius = 4,
        target = true,
        effect = CONST_ME_POFF,
    },
    {
        name = "monster soulfire",
        interval = 2000,
        chance = 10,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        minDamage = -280,
        maxDamage = -360,
        target = true,
        shootEffect = CONST_ANI_POISON,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 270,
        maxDamage = 530,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 75},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Kazzzzuuuum!!", yell = false},
    {text = "Fissziss!", yell = false},
    {text = "Zzzzzooom!!", yell = false},
}
monster.loot = {
    {
        id = "ring of the sky",
        chance = 370,
    },
    {
        id = "small ruby",
        chance = 6910,
        maxCount = 5,
    },
    {
        id = "gold coin",
        chance = 41000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 58000,
        maxCount = 100,
    },
    {
        id = "platinum coin",
        chance = 25510,
        maxCount = 5,
    },
    {
        id = "green gem",
        chance = 970,
    },
    {
        id = "wand of inferno",
        chance = 1660,
    },
    {
        id = "meat",
        chance = 30400,
    },
    {
        id = "great mana potion",
        chance = 4970,
    },
    {
        id = "focus cape",
        chance = 1450,
    },
    {
        id = "Zaoan shoes",
        chance = 1980,
    },
    {
        id = "weaver's wandtip",
        chance = 19790,
    },
    {
        id = "draken trophy",
        chance = 10,
    },
    {
        id = "spellweaver's robe",
        chance = 620,
    },
    {
        id = "Zaoan robe",
        chance = 770,
    },
    {
        id = "luminous orb",
        chance = 1980,
    },
    {
        id = "draken sulphur",
        chance = 3930,
    },
    {
        id = "harness",
        chance = 30,
    },
    {
        id = "bamboo leaves",
        chance = 180,
    },
}

mtype:register(monster)
