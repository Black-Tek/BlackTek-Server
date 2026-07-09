local mtype = Game.createMonsterType("Rustheap Golem")
local monster = {}

monster.name = "Rustheap Golem"
monster.description = "a rustheap golem"
monster.experience = 2300
monster.race = "energy"
monster.maxHealth = 2800
monster.health = 2800
monster.speed = 250
monster.manaCost = 0
monster.corpse = 23355
monster.outfit = { lookType = 603 }
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
        skill = 118,
    },
    {
        name = "war golem electrify",
        interval = 2000,
        chance = 15,
        range = 1,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 25,
        radius = 3,
        target = true,
        speed = -900,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 25,
        range = 7,
        minDamage = -110,
        maxDamage = -210,
        shootEffect = CONST_ANI_ENERGY,
    },
}
monster.defenses = {
    defense = 33,
    armor = 26,
    {
        name = "speed",
        interval = 2000,
        chance = 11,
        speed = 428,
        duration = 6000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 70},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "paralyze", condition = true},
    {type = "lifedrain", combat = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "*clatter*", yell = false},
    {text = "*krrk*", yell = false},
    {text = "*frzzp*", yell = false},
}
monster.loot = {
    {
        id = 8309,
        chance = 1950,
    },
    {
        id = 9930,
        chance = 5860,
    },
    {
        id = 23567,
        chance = 14980,
    },
    {
        id = 2148,
        chance = 100000,
        maxCount = 100,
    },
    {
        id = 2148,
        chance = 80000,
        maxCount = 100,
    },
    {
        id = 2148,
        chance = 60000,
        maxCount = 90,
    },
    {
        id = 5880,
        chance = 330,
    },
    {
        id = 24124,
        chance = 1950,
    },
    {
        id = 2152,
        chance = 58310,
        maxCount = 3,
    },
    {
        id = 2143,
        chance = 1300,
    },
    {
        id = 2144,
        chance = 1630,
    },
    {
        id = 7588,
        chance = 10100,
        maxCount = 2,
    },
    {
        id = 7589,
        chance = 7170,
        maxCount = 2,
    },
    {
        id = 2391,
        chance = 3580,
    },
    {
        id = 7452,
        chance = 330,
    },
    {
        id = 23542,
        chance = 330,
    },
    {
        id = 23541,
        chance = 2930,
    },
    {
        id = 9810,
        chance = 5860,
    },
    {
        id = 9813,
        chance = 4230,
    },
}

mtype:register(monster)
