local mtype = Game.createMonsterType("Dryad")
local monster = {}

monster.name = "Dryad"
monster.description = "a dryad"
monster.experience = 190
monster.race = "blood"
monster.maxHealth = 310
monster.health = 310
monster.speed = 230
monster.manaCost = 0
monster.corpse = 20387
monster.outfit = {
    lookType = 137,
    lookHead = 99,
    lookBody = 41,
    lookLegs = 5,
    lookFeet = 102,
    lookAddons = 3,
}
monster.runHealth = 30
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
    pushable = true,
    canPushItems = true,
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
        maxDamage = -90,
    },
    {
        name = "earth",
        interval = 2500,
        chance = 9,
        minDamage = -20,
        maxDamage = -30,
        radius = 4,
        effect = CONST_ME_BIGPLANTS,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 6,
        range = 7,
        radius = 7,
        target = false,
        speed = -200,
        duration = 20000,
        effect = CONST_ME_POFF,
    },
    {
        name = "drunk",
        interval = 1000,
        chance = 12,
        range = 7,
        duration = 20000,
        effect = CONST_ME_HEARTS,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 15,
        maxDamage = 60,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 5000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 30},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Feel the wrath of mother Nature!", yell = false},
    {text = "Defiler of nature!", yell = false},
}
monster.loot = {
    {
        id = 2787,
        chance = 55000,
        maxCount = 2,
    },
    {
        id = 2148,
        chance = 40000,
        maxCount = 30,
    },
    {
        id = 7732,
        chance = 11000,
        maxCount = 3,
    },
    {
        id = 9928,
        chance = 1280,
    },
    {
        id = 9931,
        chance = 1200,
    },
    {
        id = 9929,
        chance = 1100,
    },
    {
        id = 9927,
        chance = 970,
    },
    {
        id = 2790,
        chance = 780,
        maxCount = 2,
    },
    {
        id = 13298,
        chance = 210,
    },
    {
        id = 2150,
        chance = 130,
    },
}

mtype:register(monster)
