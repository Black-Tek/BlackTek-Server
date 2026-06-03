local mtype = Game.createMonsterType("Dwarf Soldier")
local monster = {}

monster.name = "Dwarf Soldier"
monster.description = "a dwarf soldier"
monster.experience = 70
monster.race = "blood"
monster.maxHealth = 135
monster.health = 135
monster.speed = 176
monster.manaCost = 360
monster.corpse = 6014
monster.outfit = { lookType = 71 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
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
        maxDamage = -70,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -60,
        shootEffect = CONST_ANI_BOLT,
    },
}
monster.defenses = {
    defense = 9,
    armor = 9,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Hail Durin!", yell = false},
}
monster.loot = {
    {
        id = 2787,
        chance = 39924,
        maxCount = 2,
    },
    {
        id = 2543,
        chance = 37815,
        maxCount = 7,
    },
    {
        id = 2148,
        chance = 28337,
        maxCount = 12,
    },
    {
        id = 2481,
        chance = 11820,
    },
    {
        id = 2554,
        chance = 10026,
    },
    {
        id = 2464,
        chance = 7525,
    },
    {
        id = 7363,
        chance = 3822,
        maxCount = 3,
    },
    {
        id = 2525,
        chance = 3056,
    },
    {
        id = 2455,
        chance = 3023,
    },
    {
        id = 2378,
        chance = 2481,
    },
    {
        id = 5880,
        chance = 303,
    },
    {
        id = 2208,
        chance = 121,
    },
}

mtype:register(monster)
