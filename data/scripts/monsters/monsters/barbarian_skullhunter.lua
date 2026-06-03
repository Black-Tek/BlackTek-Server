local mtype = Game.createMonsterType("Barbarian Skullhunter")
local monster = {}

monster.name = "Barbarian Skullhunter"
monster.description = "a barbarian skullhunter"
monster.experience = 85
monster.race = "blood"
monster.maxHealth = 135
monster.health = 135
monster.speed = 168
monster.manaCost = 450
monster.corpse = 20347
monster.outfit = {
    lookType = 254,
    lookHead = 0,
    lookBody = 77,
    lookLegs = 96,
    lookFeet = 114,
}
monster.changeTarget = {
    interval = 60000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 70
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -65,
    },
}
monster.defenses = {
    defense = 8,
    armor = 8,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = 50},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "paralyze", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "You will become my trophy.", yell = false},
    {text = "Fight harder, coward.", yell = false},
    {text = "Show that you are a worthy opponent.", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 74364,
        maxCount = 30,
    },
    {
        id = 2050,
        chance = 60308,
    },
    {
        id = 2460,
        chance = 19950,
    },
    {
        id = 2403,
        chance = 15045,
    },
    {
        id = 2473,
        chance = 7886,
    },
    {
        id = 2483,
        chance = 4024,
    },
    {
        id = 2229,
        chance = 3002,
    },
    {
        id = 7618,
        chance = 914,
    },
    {
        id = 5913,
        chance = 521,
    },
    {
        id = 2168,
        chance = 240,
    },
    {
        id = 7457,
        chance = 99,
    },
    {
        id = 7462,
        chance = 95,
    },
    {
        id = 7449,
        chance = 87,
    },
}

mtype:register(monster)
