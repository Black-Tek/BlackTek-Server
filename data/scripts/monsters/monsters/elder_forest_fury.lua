local mtype = Game.createMonsterType("Elder Forest Fury")
local monster = {}

monster.name = "Elder Forest Fury"
monster.description = "an elder forest fury"
monster.experience = 330
monster.race = "blood"
monster.maxHealth = 670
monster.health = 670
monster.speed = 240
monster.manaCost = 0
monster.corpse = 21359
monster.outfit = { lookType = 569 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 3
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
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
        maxDamage = -53,
        -- NOTE: melee condition (poison=5); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 5000,
        },
    },
    {
        name = "physical",
        interval = 1500,
        chance = 40,
        range = 5,
        minDamage = 0,
        maxDamage = -100,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_HUNTINGSPEAR,
        effect = CONST_ME_MAGIC_GREEN,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 40},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "To arms, sisters!", yell = false},
    {text = "Feel the wrath of mother forest!", yell = false},
    {text = "By the power of Greenskull!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 90713,
        maxCount = 68,
    },
    {
        id = 2543,
        chance = 43844,
        maxCount = 15,
    },
    {
        id = 2666,
        chance = 20086,
    },
    {
        id = 21312,
        chance = 18575,
    },
    {
        id = 7363,
        chance = 17063,
        maxCount = 5,
    },
    {
        id = 21311,
        chance = 11879,
    },
    {
        id = 3965,
        chance = 9071,
        maxCount = 2,
    },
    {
        id = 2455,
        chance = 3456,
    },
    {
        id = 2149,
        chance = 3240,
    },
    {
        id = 8840,
        chance = 864,
        maxCount = 3,
    },
    {
        id = 7443,
        chance = 432,
    },
    {
        id = 2181,
        chance = 432,
    },
}

mtype:register(monster)
