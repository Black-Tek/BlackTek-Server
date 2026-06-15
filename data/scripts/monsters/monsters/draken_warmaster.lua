local mtype = Game.createMonsterType("Draken Warmaster")
local monster = {}

monster.name = "Draken Warmaster"
monster.description = "a draken warmaster"
monster.experience = 2400
monster.race = "blood"
monster.maxHealth = 4150
monster.health = 4150
monster.speed = 324
monster.manaCost = 0
monster.corpse = 11107
monster.outfit = { lookType = 334 }
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -300,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        minDamage = -240,
        maxDamage = -520,
        length = 4,
        spread = 3,
        effect = CONST_ME_EXPLOSIONHIT,
    },
}
monster.defenses = {
    defense = 40,
    armor = 40,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 510,
        maxDamage = 600,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_HOLYDAMAGE, percent = 5},
    {type = COMBAT_DEATHDAMAGE, percent = 50},
    {type = COMBAT_ENERGYDAMAGE, percent = 5},
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Attack aggrezzively! Dezztroy zze intruderzz!", yell = false},
    {text = "Hizzzzz!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 97355,
        maxCount = 200,
    },
    {
        id = 2152,
        chance = 50252,
        maxCount = 5,
    },
    {
        id = 2666,
        chance = 30158,
    },
    {
        id = 11321,
        chance = 12048,
    },
    {
        id = 11323,
        chance = 8001,
    },
    {
        id = 11322,
        chance = 6993,
    },
    {
        id = 7591,
        chance = 5049,
        maxCount = 3,
    },
    {
        id = 8473,
        chance = 3981,
    },
    {
        id = 11303,
        chance = 2026,
    },
    {
        id = 2528,
        chance = 1991,
    },
    {
        id = 2147,
        chance = 1573,
        maxCount = 5,
    },
    {
        id = 11304,
        chance = 943,
    },
    {
        id = 11305,
        chance = 827,
    },
    {
        id = 11301,
        chance = 791,
    },
    {
        id = 2123,
        chance = 208,
    },
}

mtype:register(monster)
