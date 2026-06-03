local mtype = Game.createMonsterType("Forest Fury")
local monster = {}

monster.name = "Forest Fury"
monster.description = "a forest fury"
monster.experience = 235
monster.race = "blood"
monster.maxHealth = 480
monster.health = 480
monster.speed = 220
monster.manaCost = 0
monster.corpse = 21359
monster.outfit = { lookType = 569 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
monster.staticAttackChance = 80
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
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -125,
    },
    {
        name = "physical",
        interval = 1500,
        chance = 40,
        range = 7,
        minDamage = 0,
        maxDamage = -100,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_HUNTINGSPEAR,
        effect = CONST_ME_MAGIC_GREEN,
    },
    {
        name = "forest fury skill reducer",
        interval = 2000,
        chance = 20,
        range = 5,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
}
monster.elements = {
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
        chance = 87797,
        maxCount = 55,
    },
    {
        id = 2543,
        chance = 50567,
        maxCount = 15,
    },
    {
        id = 2666,
        chance = 24975,
    },
    {
        id = 7363,
        chance = 14846,
        maxCount = 5,
    },
    {
        id = 21312,
        chance = 12036,
    },
    {
        id = 21311,
        chance = 7445,
    },
    {
        id = 2455,
        chance = 2579,
    },
    {
        id = 9970,
        chance = 482,
    },
    {
        id = 7438,
        chance = 79,
    },
}

mtype:register(monster)
