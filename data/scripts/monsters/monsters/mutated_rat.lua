local mtype = Game.createMonsterType("Mutated Rat")
local monster = {}

monster.name = "Mutated Rat"
monster.description = "a mutated rat"
monster.experience = 450
monster.race = "blood"
monster.maxHealth = 550
monster.health = 550
monster.speed = 230
monster.manaCost = 0
monster.corpse = 9871
monster.outfit = { lookType = 305 }
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
        maxDamage = -158,
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
        minDamage = -45,
        maxDamage = -85,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        minDamage = -80,
        maxDamage = -100,
        length = 5,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        range = 7,
        target = true,
        speed = -600,
        duration = 30000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -30,
        maxDamage = -70,
        radius = 3,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        range = 7,
        radius = 3,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 5,
        minDamage = 80,
        maxDamage = 95,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "lifedrain", combat = true},
    {type = "drunk", condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Grrrrrrrrrrrrrr!", yell = false},
    {text = "Fcccccchhhhhh", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 78725,
        maxCount = 130,
    },
    {
        id = 2229,
        chance = 20298,
    },
    {
        id = 2799,
        chance = 4874,
    },
    {
        id = 10585,
        chance = 3852,
    },
    {
        id = 2510,
        chance = 3777,
    },
    {
        id = 2381,
        chance = 2957,
    },
    {
        id = 2796,
        chance = 1370,
    },
    {
        id = 2235,
        chance = 950,
    },
    {
        id = 2165,
        chance = 562,
    },
    {
        id = 7618,
        chance = 507,
    },
    {
        id = 8900,
        chance = 297,
    },
    {
        id = 2528,
        chance = 50,
    },
}

mtype:register(monster)
