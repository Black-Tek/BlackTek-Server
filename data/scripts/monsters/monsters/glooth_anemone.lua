local mtype = Game.createMonsterType("Glooth Anemone")
local monster = {}

monster.name = "Glooth Anemone"
monster.description = "a glooth anemone"
monster.experience = 1800
monster.race = "venom"
monster.maxHealth = 2400
monster.health = 2400
monster.speed = 180
monster.manaCost = 0
monster.corpse = 23359
monster.outfit = { lookType = 604 }
monster.changeTarget = {
    interval = 2000,
    chance = 3,
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        attack = 50,
        skill = 60,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 9,
        range = 7,
        minDamage = -100,
        maxDamage = -200,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_GLOOTHSPEAR,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 7,
        range = 7,
        minDamage = -50,
        maxDamage = -100,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        minDamage = 0,
        maxDamage = 0,
        radius = 5,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 25,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 9,
        minDamage = 50,
        maxDamage = 100,
        effect = CONST_ME_MAGIC_GREEN,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = 5},
    {type = COMBAT_FIREDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "lifedrain", combat = true},
    {type = "invisible", condition = true},
    {type = "poison", combat = true, condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Glooth Blob", interval = 2000, chance = 10, max = 0},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "*shglib*", yell = false},
}
monster.loot = {
    {
        id = 23515,
        chance = 10190,
    },
    {
        id = 23568,
        chance = 21020,
    },
    {
        id = 2796,
        chance = 3180,
    },
    {
        id = 23554,
        chance = 320,
    },
    {
        id = 2148,
        chance = 100000,
        maxCount = 100,
    },
    {
        id = 2148,
        chance = 60000,
        maxCount = 70,
    },
    {
        id = 7588,
        chance = 6690,
        maxCount = 2,
    },
    {
        id = 7589,
        chance = 6690,
        maxCount = 2,
    },
    {
        id = 8473,
        chance = 960,
    },
    {
        id = 2152,
        chance = 57320,
        maxCount = 3,
    },
    {
        id = 9970,
        chance = 9240,
        maxCount = 3,
    },
    {
        id = 2149,
        chance = 8600,
        maxCount = 3,
    },
    {
        id = 2147,
        chance = 11150,
        maxCount = 3,
    },
    {
        id = 23534,
        chance = 640,
    },
    {
        id = 23543,
        chance = 5100,
    },
    {
        id = 23550,
        chance = 320,
    },
    {
        id = 23549,
        chance = 1270,
    },
    {
        id = 23551,
        chance = 640,
    },
    {
        id = 23529,
        chance = 1270,
    },
}

mtype:register(monster)
