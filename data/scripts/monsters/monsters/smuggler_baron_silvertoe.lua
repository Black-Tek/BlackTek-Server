local mtype = Game.createMonsterType("Smuggler Baron Silvertoe")
local monster = {}

monster.name = "Smuggler Baron Silvertoe"
monster.description = "Smuggler Baron Silvertoe"
monster.experience = 170
monster.race = "blood"
monster.maxHealth = 280
monster.health = 280
monster.speed = 400
monster.manaCost = 0
monster.corpse = 20506
monster.outfit = {
    lookType = 134,
    lookHead = 38,
    lookBody = 0,
    lookLegs = 94,
    lookFeet = 95,
    lookAddons = 0,
}
monster.runHealth = 15
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
    canPushCreatures = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -50,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -40,
        shootEffect = CONST_ANI_THROWINGKNIFE,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 10,
        maxDamage = 50,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.maxSummons = 3
monster.summons = {
    {name = "Wild Warrior", interval = 2000, chance = 20, max = 0},
}
monster.voices = {
    interval = 1000,
    chance = 2,
    {text = "I will make your death look like an accident!", yell = false},
    {text = "You should not have interferred with my bussiness!", yell = false},
    {text = "Bribes are expensive, murder is cheap!", yell = false},
    {text = "I see some profit in your death!", yell = false},
    {text = "I expect you to die!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 80000,
        maxCount = 30,
    },
    {
        id = 2050,
        chance = 30200,
        maxCount = 2,
    },
    {
        id = 2649,
        chance = 14000,
    },
    {
        id = 2671,
        chance = 10000,
    },
    {
        id = 2406,
        chance = 10000,
    },
    {
        id = 2461,
        chance = 10000,
    },
    {
        id = 2403,
        chance = 10000,
    },
    {
        id = 2376,
        chance = 5000,
    },
    {
        id = 8840,
        chance = 5000,
        maxCount = 5,
    },
    {
        id = 2404,
        chance = 4000,
    },
    {
        id = 7397,
        chance = 110,
    },
}

mtype:register(monster)
