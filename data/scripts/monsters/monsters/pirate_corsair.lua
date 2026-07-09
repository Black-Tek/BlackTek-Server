local mtype = Game.createMonsterType("Pirate Corsair")
local monster = {}

monster.name = "Pirate Corsair"
monster.description = "a pirate corsair"
monster.experience = 350
monster.race = "blood"
monster.maxHealth = 675
monster.health = 675
monster.speed = 238
monster.manaCost = 775
monster.corpse = 20475
monster.outfit = { lookType = 98 }
monster.runHealth = 40
monster.changeTarget = {
    interval = 4000,
    chance = 15,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -170,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -150,
        shootEffect = CONST_ANI_THROWINGSTAR,
    },
    {
        name = "pirate corsair skill reducer",
        interval = 2000,
        chance = 5,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = -5},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Give up!", yell = false},
    {text = "Hiyaa!", yell = false},
    {text = "Plundeeeeer!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 55352,
        maxCount = 86,
    },
    {
        id = 11219,
        chance = 10163,
    },
    {
        id = 2385,
        chance = 9816,
    },
    {
        id = 2399,
        chance = 8104,
        maxCount = 12,
    },
    {
        id = 2489,
        chance = 1874,
    },
    {
        id = 2521,
        chance = 1105,
    },
    {
        id = 6096,
        chance = 1094,
    },
    {
        id = 5926,
        chance = 1029,
    },
    {
        id = 7588,
        chance = 802,
    },
    {
        id = 6097,
        chance = 574,
    },
    {
        id = 6126,
        chance = 553,
    },
    {
        id = 6098,
        chance = 488,
    },
    {
        id = 5462,
        chance = 325,
    },
    {
        id = 5813,
        chance = 119,
    },
    {
        id = 5553,
        chance = 98,
    },
    {
        id = 2114,
        chance = 87,
    },
}

mtype:register(monster)
