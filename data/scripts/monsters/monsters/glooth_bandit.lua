local mtype = Game.createMonsterType("Glooth Bandit")
local monster = {}

monster.name = "Glooth Bandit"
monster.description = "a glooth bandit"
monster.experience = 2000
monster.race = "blood"
monster.maxHealth = 2600
monster.health = 2600
monster.speed = 280
monster.manaCost = 0
monster.corpse = 24251
monster.outfit = {
    lookType = 129,
    lookHead = 115,
    lookBody = 80,
    lookLegs = 114,
    lookFeet = 114,
    lookAddons = 0,
    lookMount = 0,
}
monster.changeTarget = {
    interval = 2000,
    chance = 2,
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
        attack = 60,
        skill = 90,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -200,
        shootEffect = CONST_ANI_ARROW,
    },
}
monster.defenses = {
    defense = 31,
    armor = 23,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 200,
        maxDamage = 250,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = 20},
    {type = COMBAT_PHYSICALDAMAGE, percent = 15},
}
monster.immunities = {
    {type = "poison", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = 3962,
        chance = 800,
    },
    {
        id = 23554,
        chance = 990,
    },
    {
        id = 23551,
        chance = 1030,
    },
    {
        id = 23574,
        chance = 5070,
    },
    {
        id = 23550,
        chance = 1000,
    },
    {
        id = 23535,
        chance = 580,
    },
    {
        id = 24183,
        chance = 3010,
    },
    {
        id = 23549,
        chance = 1000,
    },
    {
        id = 23514,
        chance = 2470,
    },
    {
        id = 23529,
        chance = 1010,
    },
    {
        id = 23517,
        chance = 2600,
    },
    {
        id = 2148,
        chance = 100000,
        maxCount = 100,
    },
    {
        id = 7591,
        chance = 8090,
    },
    {
        id = 7590,
        chance = 7400,
        maxCount = 2,
    },
    {
        id = 8472,
        chance = 3480,
    },
    {
        id = 2155,
        chance = 160,
    },
    {
        id = 2152,
        chance = 60050,
        maxCount = 3,
    },
    {
        id = 23536,
        chance = 730,
    },
    {
        id = 2436,
        chance = 420,
    },
    {
        id = 2149,
        chance = 2040,
        maxCount = 2,
    },
    {
        id = 9970,
        chance = 2530,
        maxCount = 2,
    },
    {
        id = 24185,
        chance = 8010,
    },
    {
        id = 7886,
        chance = 490,
    },
    {
        id = 7903,
        chance = 600,
    },
    {
        id = 7885,
        chance = 480,
    },
    {
        id = 7884,
        chance = 600,
    },
    {
        id = 8473,
        chance = 3440,
    },
    {
        id = 2454,
        chance = 100,
    },
}

mtype:register(monster)
