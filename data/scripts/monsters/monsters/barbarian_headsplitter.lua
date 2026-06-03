local mtype = Game.createMonsterType("Barbarian Headsplitter")
local monster = {}

monster.name = "Barbarian Headsplitter"
monster.description = "a barbarian headsplitter"
monster.experience = 85
monster.race = "blood"
monster.maxHealth = 100
monster.health = 100
monster.speed = 168
monster.manaCost = 450
monster.corpse = 20343
monster.outfit = {
    lookType = 253,
    lookHead = 115,
    lookBody = 86,
    lookLegs = 119,
    lookFeet = 113,
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
        maxDamage = -50,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -60,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_WHIRLWINDAXE,
    },
}
monster.defenses = {
    defense = 7,
    armor = 7,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = 50},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "drunk", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "I will regain my honor with your blood!", yell = false},
    {text = "Surrender is not option!", yell = false},
    {text = "Its you or me!", yell = false},
    {text = "Die! Die! Die!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 75228,
        maxCount = 30,
    },
    {
        id = 2050,
        chance = 59812,
    },
    {
        id = 2460,
        chance = 20170,
    },
    {
        id = 2403,
        chance = 14928,
    },
    {
        id = 2473,
        chance = 4960,
    },
    {
        id = 2483,
        chance = 4046,
    },
    {
        id = 2229,
        chance = 3194,
    },
    {
        id = 5913,
        chance = 968,
    },
    {
        id = 7618,
        chance = 531,
    },
    {
        id = 2168,
        chance = 228,
    },
    {
        id = 7461,
        chance = 112,
    },
    {
        id = 7457,
        chance = 87,
    },
}

mtype:register(monster)
