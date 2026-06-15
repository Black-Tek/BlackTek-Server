local mtype = Game.createMonsterType("Crazed Beggar")
local monster = {}

monster.name = "Crazed Beggar"
monster.description = "a crazed beggar"
monster.experience = 35
monster.race = "blood"
monster.maxHealth = 100
monster.health = 100
monster.speed = 154
monster.manaCost = 300
monster.corpse = 20351
monster.outfit = {
    lookType = 153,
    lookHead = 40,
    lookBody = 19,
    lookLegs = 21,
    lookFeet = 97,
    lookAddons = 3,
}
monster.runHealth = 10
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -25,
    },
}
monster.defenses = {
    defense = 4,
    armor = 4,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.voices = {
    interval = 1000,
    chance = 5,
    {text = "Hehehe!", yell = false},
    {text = "Raahhh!", yell = false},
    {text = "You are one of THEM! Die!", yell = false},
    {text = "Wanna buy roses??", yell = false},
    {text = "Make it stop!", yell = false},
    {text = "They're coming! They're coming!", yell = false},
    {text = "Gimme money!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 99393,
        maxCount = 9,
    },
    {
        id = 2237,
        chance = 60572,
    },
    {
        id = 2690,
        chance = 19237,
    },
    {
        id = 2666,
        chance = 9850,
    },
    {
        id = 2567,
        chance = 9474,
    },
    {
        id = 2556,
        chance = 6672,
    },
    {
        id = 2744,
        chance = 5055,
    },
    {
        id = 2570,
        chance = 4708,
    },
    {
        id = 2802,
        chance = 607,
    },
    {
        id = 1681,
        chance = 607,
    },
    {
        id = 5553,
        chance = 520,
    },
    {
        id = 6092,
        chance = 380,
    },
    {
        id = 2072,
        chance = 376,
    },
    {
        id = 9808,
        chance = 289,
    },
    {
        id = 2213,
        chance = 144,
    },
}

mtype:register(monster)
