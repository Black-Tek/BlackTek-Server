local mtype = Game.createMonsterType("Gang Member")
local monster = {}

monster.name = "Gang Member"
monster.description = "a gang member"
monster.experience = 70
monster.race = "blood"
monster.maxHealth = 295
monster.health = 295
monster.speed = 190
monster.manaCost = 420
monster.corpse = 20403
monster.outfit = {
    lookType = 151,
    lookHead = 114,
    lookBody = 19,
    lookLegs = 23,
    lookFeet = 40,
}
monster.runHealth = 35
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
        maxDamage = -70,
    },
}
monster.defenses = {
    defense = 8,
    armor = 8,
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "This is our territory!", yell = false},
    {text = "Help me guys!", yell = false},
    {text = "I don't like the way you look!", yell = false},
    {text = "You're wearing the wrong colours!", yell = false},
    {text = "Don't mess with us!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 48971,
        maxCount = 30,
    },
    {
        id = 2649,
        chance = 15541,
    },
    {
        id = 2398,
        chance = 9182,
    },
    {
        id = 2691,
        chance = 5251,
    },
    {
        id = 2468,
        chance = 5040,
    },
    {
        id = 2209,
        chance = 1029,
    },
}

mtype:register(monster)
