local mtype = Game.createMonsterType("Demon Skeleton")
local monster = {}

monster.name = "Demon Skeleton"
monster.description = "a demon skeleton"
monster.experience = 240
monster.race = "undead"
monster.maxHealth = 400
monster.health = 400
monster.speed = 180
monster.manaCost = 620
monster.corpse = 5963
monster.outfit = { lookType = 37 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -185,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        range = 1,
        minDamage = -30,
        maxDamage = -50,
        radius = 1,
        target = true,
        effect = CONST_ME_SMALLCLOUDS,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "death", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "drunk", condition = true},
}
monster.loot = {
    {
        id = 2148,
        chance = 97515,
        maxCount = 75,
    },
    {
        id = 10564,
        chance = 11978,
    },
    {
        id = 2399,
        chance = 9982,
        maxCount = 3,
    },
    {
        id = 7618,
        chance = 9693,
        maxCount = 2,
    },
    {
        id = 7620,
        chance = 5050,
    },
    {
        id = 2513,
        chance = 4960,
    },
    {
        id = 2050,
        chance = 4861,
    },
    {
        id = 2417,
        chance = 4061,
    },
    {
        id = 2459,
        chance = 3377,
    },
    {
        id = 2144,
        chance = 2976,
    },
    {
        id = 2147,
        chance = 1531,
    },
    {
        id = 2194,
        chance = 535,
    },
    {
        id = 2178,
        chance = 469,
    },
    {
        id = 2515,
        chance = 113,
    },
}

mtype:register(monster)
