local mtype = Game.createMonsterType("Diamond Servant")
local monster = {}

monster.name = "Diamond Servant"
monster.description = "a diamond servant"
monster.experience = 700
monster.race = "energy"
monster.maxHealth = 1000
monster.health = 1000
monster.speed = 172
monster.manaCost = 0
monster.corpse = 13485
monster.outfit = { lookType = 397 }
monster.runHealth = 100
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
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -100,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 20,
        minDamage = -80,
        maxDamage = -120,
        radius = 3,
        target = false,
        effect = CONST_ME_YELLOWENERGY,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 15,
        minDamage = -125,
        maxDamage = -170,
        length = 5,
        spread = 2,
        target = false,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        range = 7,
        target = true,
        duration = 3000,
        shootEffect = CONST_ANI_DEATH,
        effect = CONST_ME_STUN,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 75},
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = -15},
    {type = COMBAT_DEATHDAMAGE, percent = 0},
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "invisible", condition = true},
    {type = "paralyze", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Error. LOAD 'PROGRAM',8,1", yell = false},
    {text = "Remain. Obedient.", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 94036,
        maxCount = 179,
    },
    {
        id = 5944,
        chance = 44999,
    },
    {
        id = 2177,
        chance = 9389,
    },
    {
        id = 7589,
        chance = 6047,
    },
    {
        id = 7588,
        chance = 5834,
    },
    {
        id = 9976,
        chance = 5237,
    },
    {
        id = 9690,
        chance = 5019,
    },
    {
        id = 10572,
        chance = 4967,
    },
    {
        id = 2164,
        chance = 903,
    },
    {
        id = 7889,
        chance = 695,
    },
    {
        id = 2154,
        chance = 561,
    },
    {
        id = 13758,
        chance = 550,
    },
    {
        id = 2189,
        chance = 550,
    },
    {
        id = 7440,
        chance = 415,
    },
    {
        id = 10221,
        chance = 114,
    },
    {
        id = 8878,
        chance = 16,
    },
    {
        id = 7428,
        chance = 10,
    },
}

mtype:register(monster)
