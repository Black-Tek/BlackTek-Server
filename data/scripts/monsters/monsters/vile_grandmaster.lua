local mtype = Game.createMonsterType("Vile Grandmaster")
local monster = {}

monster.name = "Vile Grandmaster"
monster.description = "a vile grandmaster"
monster.experience = 1500
monster.race = "blood"
monster.maxHealth = 1700
monster.health = 1700
monster.speed = 210
monster.manaCost = 0
monster.corpse = 20418
monster.outfit = {
    lookType = 268,
    lookHead = 59,
    lookBody = 19,
    lookLegs = 95,
    lookFeet = 94,
    lookAddons = 1,
}
monster.changeTarget = {
    interval = 2000,
    chance = 5,
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
        skill = 94,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 22,
        range = 7,
        minDamage = 0,
        maxDamage = -225,
        shootEffect = CONST_ANI_WHIRLWINDSWORD,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        minDamage = -25,
        maxDamage = -125,
        radius = 4,
        effect = CONST_ME_GROUNDSHAKER,
    },
}
monster.defenses = {
    defense = 33,
    armor = 27,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 220,
        maxDamage = 280,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 25},
    {type = COMBAT_FIREDAMAGE, percent = 25},
    {type = COMBAT_HOLYDAMAGE, percent = 50},
    {type = COMBAT_EARTHDAMAGE, percent = 25},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "You'll make a fine trophy!", yell = false},
    {text = "Is that the best, they can throw at me?", yell = false},
    {text = "I've seen orcs tougher than you!", yell = false},
    {text = "I will end this now!", yell = false},
    {text = "Your gods won't help you!", yell = false},
}
monster.loot = {
    {
        id = 12406,
        chance = 724,
    },
    {
        id = 2148,
        chance = 100000,
        maxCount = 100,
    },
    {
        id = 2148,
        chance = 60000,
        maxCount = 55,
    },
    {
        id = 2152,
        chance = 73188,
        maxCount = 2,
    },
    {
        id = 2121,
        chance = 7246,
    },
    {
        id = 12466,
        chance = 8695,
    },
    {
        id = 2666,
        chance = 7971,
    },
    {
        id = 2147,
        chance = 2173,
    },
    {
        id = 2146,
        chance = 2173,
    },
    {
        id = 2391,
        chance = 1449,
    },
    {
        id = 5911,
        chance = 2898,
    },
    {
        id = 2487,
        chance = 1449,
    },
    {
        id = 2491,
        chance = 1449,
    },
    {
        id = 2392,
        chance = 2173,
    },
    {
        id = 7591,
        chance = 9420,
    },
    {
        id = 2488,
        chance = 724,
    },
}

mtype:register(monster)
