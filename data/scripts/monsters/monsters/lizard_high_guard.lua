local mtype = Game.createMonsterType("Lizard High Guard")
local monster = {}

monster.name = "Lizard High Guard"
monster.description = "a lizard high guard"
monster.experience = 1450
monster.race = "blood"
monster.maxHealth = 1800
monster.health = 1800
monster.speed = 238
monster.manaCost = 0
monster.corpse = 11272
monster.outfit = { lookType = 337 }
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
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -306,
    },
}
monster.defenses = {
    defense = 40,
    armor = 40,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 25,
        maxDamage = 75,
        effect = CONST_ME_MAGIC_GREEN,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_FIREDAMAGE, percent = 45},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Hizzzzzzz!", yell = false},
    {text = "To armzzzz!", yell = false},
    {text = "Engage zze aggrezzor!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 96078,
        maxCount = 281,
    },
    {
        id = 7588,
        chance = 11938,
    },
    {
        id = 11333,
        chance = 8091,
    },
    {
        id = 7591,
        chance = 7075,
    },
    {
        id = 11325,
        chance = 6970,
    },
    {
        id = 11245,
        chance = 4932,
    },
    {
        id = 2152,
        chance = 4910,
        maxCount = 2,
    },
    {
        id = 11332,
        chance = 3006,
    },
    {
        id = 2149,
        chance = 2487,
        maxCount = 4,
    },
    {
        id = 11206,
        chance = 1209,
    },
    {
        id = 2528,
        chance = 1032,
    },
    {
        id = 5876,
        chance = 997,
    },
    {
        id = 5881,
        chance = 957,
    },
    {
        id = 11304,
        chance = 734,
    },
    {
        id = 11303,
        chance = 688,
    },
    {
        id = 11301,
        chance = 76,
    },
}

mtype:register(monster)
