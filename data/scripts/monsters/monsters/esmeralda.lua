local mtype = Game.createMonsterType("Esmeralda")
local monster = {}

monster.name = "Esmeralda"
monster.description = "Esmeralda"
monster.experience = 600
monster.race = "blood"
monster.maxHealth = 800
monster.health = 800
monster.speed = 245
monster.manaCost = 0
monster.corpse = 9871
monster.outfit = { lookType = 305 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
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
        maxDamage = -175,
        -- NOTE: melee condition (poison=100); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 100000,
        },
    },
    {
        name = "earth",
        interval = 2000,
        chance = 30,
        range = 7,
        minDamage = 0,
        maxDamage = -110,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 22,
        minDamage = -100,
        maxDamage = -100,
        length = 8,
        effect = CONST_ME_SMALLPLANTS,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 15,
        minDamage = -80,
        maxDamage = -80,
        radius = 3,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 25,
        range = 7,
        minDamage = 0,
        maxDamage = -110,
        radius = 3,
        target = false,
        effect = CONST_ME_DRAWBLOOD,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -10},
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
    interval = 2000,
    chance = 10,
    {text = "Fcccccchhhhhh", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 170,
    },
    {
        id = 10585,
        chance = 100000,
    },
    {
        id = 2214,
        chance = 100000,
    },
    {
        id = 2152,
        chance = 94872,
        maxCount = 4,
    },
    {
        id = 2147,
        chance = 71795,
        maxCount = 3,
    },
    {
        id = 2476,
        chance = 52564,
    },
    {
        id = 2528,
        chance = 43590,
    },
    {
        id = 2381,
        chance = 28205,
    },
    {
        id = 2438,
        chance = 25641,
    },
    {
        id = 7884,
        chance = 10256,
    },
    {
        id = 2799,
        chance = 5128,
    },
}

mtype:register(monster)
