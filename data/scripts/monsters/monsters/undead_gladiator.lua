local mtype = Game.createMonsterType("Undead Gladiator")
local monster = {}

monster.name = "Undead Gladiator"
monster.description = "an undead gladiator"
monster.experience = 800
monster.race = "undead"
monster.maxHealth = 1000
monster.health = 1000
monster.speed = 220
monster.manaCost = 0
monster.corpse = 9823
monster.outfit = { lookType = 306 }
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
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -250,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -135,
        shootEffect = CONST_ANI_WHIRLWINDSWORD,
    },
}
monster.defenses = {
    defense = 35,
    armor = 35,
    {
        name = "invisible",
        interval = 2000,
        chance = 15,
        duration = 3000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = 80},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Let's battle it out in a duel!", yell = false},
    {text = "Bring it!", yell = false},
    {text = "I'll fight here in eternity and beyond.", yell = false},
    {text = "I will not give up!", yell = false},
    {text = "Another foolish adventurer who tries to beat me.", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 95314,
        maxCount = 149,
    },
    {
        id = 2399,
        chance = 14874,
        maxCount = 18,
    },
    {
        id = 2419,
        chance = 10743,
    },
    {
        id = 8872,
        chance = 5209,
    },
    {
        id = 10573,
        chance = 5201,
    },
    {
        id = 2478,
        chance = 4923,
    },
    {
        id = 2465,
        chance = 4820,
    },
    {
        id = 3965,
        chance = 4519,
    },
    {
        id = 2647,
        chance = 2268,
    },
    {
        id = 2463,
        chance = 2244,
    },
    {
        id = 2377,
        chance = 1982,
    },
    {
        id = 2200,
        chance = 1927,
    },
    {
        id = 2490,
        chance = 1395,
    },
    {
        id = 7618,
        chance = 404,
    },
    {
        id = 2430,
        chance = 246,
    },
    {
        id = 2497,
        chance = 182,
    },
    {
        id = 5885,
        chance = 166,
    },
    {
        id = 2165,
        chance = 48,
    },
}

mtype:register(monster)
