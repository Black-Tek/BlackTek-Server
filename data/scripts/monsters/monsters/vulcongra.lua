local mtype = Game.createMonsterType("Vulcongra")
local monster = {}

monster.name = "Vulcongra"
monster.description = "Vulcongra"
monster.experience = 1100
monster.race = "fire"
monster.maxHealth = 1600
monster.health = 1600
monster.speed = 320
monster.manaCost = 0
monster.corpse = 18476
monster.outfit = { lookType = 509 }
monster.runHealth = 220
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -235,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        minDamage = -195,
        maxDamage = -340,
        length = 8,
        effect = CONST_ME_EXPLOSIONHIT,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -100,
        maxDamage = -200,
        target = true,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "vulcongra soulfire",
        interval = 3000,
        chance = 100,
    },
}
monster.defenses = {
    defense = 35,
    armor = 35,
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 5},
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Fuchah! Fuchah!", yell = false},
    {text = "Fuchah!", yell = false},
    {text = "Yag! Yag! Yag!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 107,
    },
    {
        id = 2152,
        chance = 14338,
    },
    {
        id = 18424,
        chance = 12277,
    },
    {
        id = 2676,
        chance = 9563,
        maxCount = 10,
    },
    {
        id = 10553,
        chance = 9041,
    },
    {
        id = 18425,
        chance = 7893,
    },
    {
        id = 7589,
        chance = 7319,
    },
    {
        id = 7588,
        chance = 7280,
    },
    {
        id = 18417,
        chance = 4945,
    },
    {
        id = 2207,
        chance = 3053,
    },
    {
        id = 18420,
        chance = 2401,
    },
    {
        id = 2187,
        chance = 978,
    },
    {
        id = 7890,
        chance = 887,
    },
    {
        id = 13757,
        chance = 848,
    },
    {
        id = 2392,
        chance = 300,
    },
    {
        id = 7899,
        chance = 170,
    },
}

mtype:register(monster)
