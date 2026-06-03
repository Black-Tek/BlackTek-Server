local mtype = Game.createMonsterType("Glooth Golem")
local monster = {}

monster.name = "Glooth Golem"
monster.description = "a glooth golem"
monster.experience = 2200
monster.race = "venom"
monster.maxHealth = 2700
monster.health = 2700
monster.speed = 260
monster.manaCost = 0
monster.corpse = 23343
monster.outfit = { lookType = 600 }
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
        attack = 50,
        skill = 60,
    },
    {
        name = "melee",
        interval = 2000,
        chance = 2,
        attack = 100,
        skill = 86,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 25,
        range = 7,
        minDamage = -125,
        maxDamage = -245,
        shootEffect = CONST_ANI_ENERGY,
    },
    {
        name = "war golem skill reducer",
        interval = 2000,
        chance = 16,
    },
    {
        name = "war golem electrify",
        interval = 2000,
        chance = 9,
        range = 7,
    },
}
monster.defenses = {
    defense = 35,
    armor = 27,
    {
        name = "speed",
        interval = 2000,
        chance = 13,
        speed = 404,
        duration = 4000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 20,
        minDamage = 200,
        maxDamage = 250,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 15},
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 5},
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = 30},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "*slosh*", yell = false},
    {text = "*clank*", yell = false},
}
monster.loot = {
    {
        id = 5880,
        chance = 1030,
    },
    {
        id = 23554,
        chance = 1720,
    },
    {
        id = 23541,
        chance = 1720,
    },
    {
        id = 23536,
        chance = 1370,
    },
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
        chance = 4470,
    },
    {
        id = 9690,
        chance = 690,
    },
    {
        id = 23514,
        chance = 9970,
    },
    {
        id = 2152,
        chance = 56010,
        maxCount = 4,
    },
    {
        id = 23474,
        chance = 16840,
    },
    {
        id = 8473,
        chance = 4470,
    },
    {
        id = 7590,
        chance = 9280,
    },
    {
        id = 23538,
        chance = 690,
    },
    {
        id = 23550,
        chance = 3440,
    },
    {
        id = 23549,
        chance = 1030,
    },
    {
        id = 23551,
        chance = 690,
    },
    {
        id = 2154,
        chance = 1030,
    },
    {
        id = 9970,
        chance = 7560,
        maxCount = 4,
    },
    {
        id = 2149,
        chance = 8590,
        maxCount = 4,
    },
}

mtype:register(monster)
