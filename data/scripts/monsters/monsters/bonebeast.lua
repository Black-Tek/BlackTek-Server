local mtype = Game.createMonsterType("Bonebeast")
local monster = {}

monster.name = "Bonebeast"
monster.description = "a bonebeast"
monster.experience = 580
monster.race = "undead"
monster.maxHealth = 515
monster.health = 515
monster.speed = 218
monster.manaCost = 0
monster.corpse = 6030
monster.outfit = { lookType = 101 }
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
    illusionable = false,
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
        maxDamage = -200,
        -- NOTE: melee condition (poison=100); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 100000,
        },
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -50,
        maxDamage = -90,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -25,
        maxDamage = -47,
        radius = 3,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        minDamage = -50,
        maxDamage = -60,
        radius = 3,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        minDamage = -70,
        maxDamage = -80,
        length = 6,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        target = true,
        speed = -600,
        duration = 13000,
    },
}
monster.defenses = {
    defense = 40,
    armor = 40,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 50,
        maxDamage = 60,
        effect = CONST_ME_HITBYPOISON,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "drunk", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Cccchhhhhhhhh!", yell = false},
    {text = "Knooorrrrr!", yell = false},
}
monster.loot = {
    {
        id = 2230,
        chance = 49750,
    },
    {
        id = "gold coin",
        chance = 30000,
        maxCount = 90,
    },
    {
        id = 2229,
        chance = 20000,
    },
    {
        id = "bony tail",
        chance = 9980,
    },
    {
        id = "plate armor",
        chance = 8000,
    },
    {
        id = "bone club",
        chance = 4950,
    },
    {
        id = "bone shield",
        chance = 2000,
    },
    {
        id = "green mushroom",
        chance = 1450,
    },
    {
        id = "hardened bone",
        chance = 1000,
    },
    {
        id = "health potion",
        chance = 540,
    },
    {
        id = "bonebeast trophy",
        chance = 120,
    },
}

mtype:register(monster)
