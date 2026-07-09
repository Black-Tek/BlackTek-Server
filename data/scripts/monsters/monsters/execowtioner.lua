local mtype = Game.createMonsterType("Execowtioner")
local monster = {}

monster.name = "Execowtioner"
monster.description = "an execowtioner"
monster.experience = 2400
monster.race = "blood"
monster.maxHealth = 3500
monster.health = 3500
monster.speed = 270
monster.manaCost = 0
monster.corpse = 23375
monster.outfit = { lookType = 609 }
monster.changeTarget = {
    interval = 2000,
    chance = 4,
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
        attack = 80,
        skill = 90,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 8,
        range = 7,
        minDamage = -135,
        maxDamage = -280,
        radius = 5,
        target = true,
        shootEffect = CONST_ANI_WHIRLWINDAXE,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 8,
        range = 7,
        minDamage = -90,
        maxDamage = -200,
        target = true,
        shootEffect = CONST_ANI_WHIRLWINDAXE,
        effect = CONST_ME_EXPLOSIONAREA,
    },
}
monster.defenses = {
    defense = 40,
    armor = 40,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 1},
    {type = COMBAT_DEATHDAMAGE, percent = 15},
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "I'm the one who puts the cute into execute!", yell = false},
    {text = "I'm here to punish!", yell = false},
    {text = "Justice is swift and unavoidable!", yell = false},
    {text = "I'll bring justice!", yell = false},
    {text = "There is excellence in execution!", yell = false},
    {text = "Your sentence is death!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 100,
    },
    {
        id = 2148,
        chance = 100000,
        maxCount = 98,
    },
    {
        id = 2152,
        chance = 67610,
        maxCount = 3,
    },
    {
        id = 9971,
        chance = 390,
    },
    {
        id = 5911,
        chance = 3230,
    },
    {
        id = 5878,
        chance = 14710,
    },
    {
        id = 12428,
        chance = 6580,
        maxCount = 2,
    },
    {
        id = 23572,
        chance = 13160,
    },
    {
        id = 7591,
        chance = 11480,
    },
    {
        id = 7590,
        chance = 10060,
    },
    {
        id = 2666,
        chance = 7230,
    },
    {
        id = 9970,
        chance = 5810,
        maxCount = 2,
    },
    {
        id = 2147,
        chance = 4520,
        maxCount = 2,
    },
    {
        id = 7412,
        chance = 900,
    },
    {
        id = 2487,
        chance = 770,
    },
    {
        id = 23547,
        chance = 1420,
    },
    {
        id = 2430,
        chance = 770,
    },
    {
        id = 7413,
        chance = 390,
    },
    {
        id = 7401,
        chance = 520,
    },
}

mtype:register(monster)
