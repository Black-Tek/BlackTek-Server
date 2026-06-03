local mtype = Game.createMonsterType("Rukor Zad")
local monster = {}

monster.name = "Rukor Zad"
monster.description = "Rukor Zad"
monster.experience = 380
monster.race = "blood"
monster.maxHealth = 380
monster.health = 380
monster.speed = 430
monster.manaCost = 0
monster.corpse = 20578
monster.outfit = {
    lookType = 152,
    lookHead = 114,
    lookBody = 95,
    lookLegs = 95,
    lookFeet = 95,
    lookAddons = 3,
}
monster.changeTarget = {
    interval = 4000,
    chance = 0,
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
    boss = true,
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
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -100,
        shootEffect = CONST_ANI_THROWINGSTAR,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -100,
        shootEffect = CONST_ANI_POISONARROW,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -8,
        maxDamage = -8,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "drunk",
        interval = 3000,
        chance = 34,
        range = 7,
        duration = 5000,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 2000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 1000,
    chance = 2,
    {text = "I can kill a man in a thousand ways. And that`s only with a spoon!", yell = false},
    {text = "You shouldn't have come here!", yell = false},
    {text = "Haiiii!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 93210,
        maxCount = 50,
    },
    {
        id = 2050,
        chance = 35000,
        maxCount = 2,
    },
    {
        id = 2403,
        chance = 19750,
    },
    {
        id = 2399,
        chance = 9210,
        maxCount = 14,
    },
    {
        id = 7366,
        chance = 6200,
        maxCount = 7,
    },
    {
        id = 2404,
        chance = 5000,
    },
    {
        id = 2457,
        chance = 4190,
    },
    {
        id = 2510,
        chance = 2910,
    },
    {
        id = 2513,
        chance = 2560,
    },
    {
        id = 2509,
        chance = 1940,
    },
    {
        id = 3968,
        chance = 1480,
    },
    {
        id = 3969,
        chance = 1240,
    },
    {
        id = 2145,
        chance = 1220,
    },
}

mtype:register(monster)
