local mtype = Game.createMonsterType("Corym Charlatan")
local monster = {}

monster.name = "Corym Charlatan"
monster.description = "a corym charlatan"
monster.experience = 150
monster.race = "blood"
monster.maxHealth = 250
monster.health = 250
monster.speed = 180
monster.manaCost = 490
monster.corpse = 19725
monster.outfit = {
    lookType = 532,
    lookHead = 0,
    lookBody = 78,
    lookLegs = 59,
    lookFeet = 0,
}
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
    pushable = false,
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
        maxDamage = -100,
    },
}
monster.defenses = {
    defense = 17,
    armor = 17,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 25},
    {type = COMBAT_DEATHDAMAGE, percent = 15},
    {type = COMBAT_ENERGYDAMAGE, percent = 25},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Mehehe!", yell = false},
    {text = "Beware! Me hexing you!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 35,
    },
    {
        id = 2696,
        chance = 10000,
    },
    {
        id = "bola",
        chance = 7692,
    },
    {
        id = "spike shield",
        chance = 492,
    },
    {
        id = "ratana",
        chance = 909,
    },
    {
        id = "life preserver",
        chance = 854,
    },
    {
        id = "cheese cutter",
        chance = 5882,
    },
    {
        id = "cheesy figurine",
        chance = 518,
    },
    {
        id = "earflap",
        chance = 7142,
    },
    {
        id = "soft cheese",
        chance = 12500,
    },
    {
        id = "rat cheese",
        chance = 14285,
    },
    {
        id = "leather harness",
        chance = 537,
    },
}

mtype:register(monster)
