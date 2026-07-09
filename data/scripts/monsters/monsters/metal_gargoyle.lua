local mtype = Game.createMonsterType("Metal Gargoyle")
local monster = {}

monster.name = "Metal Gargoyle"
monster.description = "a metal gargoyle"
monster.experience = 1400
monster.race = "energy"
monster.maxHealth = 2100
monster.health = 2100
monster.speed = 190
monster.manaCost = 0
monster.corpse = 23347
monster.outfit = { lookType = 601 }
monster.changeTarget = {
    interval = 2000,
    chance = 5,
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
        skill = 84,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        minDamage = -125,
        maxDamage = -230,
        length = 8,
        effect = CONST_ME_YELLOWENERGY,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 9,
        range = 7,
        minDamage = -85,
        maxDamage = -150,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
}
monster.defenses = {
    defense = 33,
    armor = 26,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = -5},
    {type = COMBAT_PHYSICALDAMAGE, percent = 40},
    {type = COMBAT_DEATHDAMAGE, percent = 80},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "lifedrain", combat = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "*click*", yell = false},
    {text = "*clonk*", yell = false},
    {text = "*stomp*", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 100,
    },
    {
        id = 2148,
        chance = 60000,
        maxCount = 100,
    },
    {
        id = 24124,
        chance = 1490,
    },
    {
        id = 23564,
        chance = 17160,
    },
    {
        id = 2152,
        chance = 43280,
        maxCount = 2,
    },
    {
        id = 7588,
        chance = 9700,
        maxCount = 2,
    },
    {
        id = 7589,
        chance = 9700,
        maxCount = 2,
    },
    {
        id = 23542,
        chance = 1490,
    },
    {
        id = 2167,
        chance = 2240,
    },
    {
        id = 2168,
        chance = 750,
    },
    {
        id = 11227,
        chance = 2240,
    },
    {
        id = 23540,
        chance = 1490,
    },
    {
        id = 23539,
        chance = 1490,
    },
    {
        id = 9810,
        chance = 2990,
    },
}

mtype:register(monster)
