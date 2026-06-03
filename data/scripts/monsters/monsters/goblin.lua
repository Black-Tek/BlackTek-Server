local mtype = Game.createMonsterType("Goblin")
local monster = {}

monster.name = "Goblin"
monster.description = "a goblin"
monster.experience = 25
monster.race = "blood"
monster.maxHealth = 50
monster.health = 50
monster.speed = 120
monster.manaCost = 290
monster.corpse = 6002
monster.outfit = { lookType = 61 }
monster.runHealth = 35
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
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
        maxDamage = -10,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -25,
        shootEffect = CONST_ANI_SMALLSTONE,
    },
}
monster.defenses = {
    defense = 6,
    armor = 6,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Me have him!", yell = false},
    {text = "Zig Zag! Gobo attack!", yell = false},
    {text = "Help! Goblinkiller!", yell = false},
    {text = "Bugga! Bugga!", yell = false},
    {text = "Me green, me mean!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 50250,
        maxCount = 9,
    },
    {
        id = 1294,
        chance = 15265,
    },
    {
        id = 2667,
        chance = 13004,
    },
    {
        id = 2559,
        chance = 9863,
    },
    {
        id = 2406,
        chance = 8832,
    },
    {
        id = 2449,
        chance = 4934,
    },
    {
        id = 2467,
        chance = 2514,
    },
    {
        id = 2461,
        chance = 1947,
    },
    {
        id = 2379,
        chance = 1795,
    },
    {
        id = 2230,
        chance = 1065,
    },
    {
        id = 2235,
        chance = 999,
    },
    {
        id = 12495,
        chance = 951,
    },
}

mtype:register(monster)
