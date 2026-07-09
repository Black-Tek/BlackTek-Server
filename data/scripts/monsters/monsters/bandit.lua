local mtype = Game.createMonsterType("Bandit")
local monster = {}

monster.name = "Bandit"
monster.description = "a bandit"
monster.experience = 65
monster.race = "blood"
monster.maxHealth = 245
monster.health = 245
monster.speed = 180
monster.manaCost = 450
monster.corpse = 20331
monster.outfit = {
    lookType = 129,
    lookHead = 58,
    lookBody = 40,
    lookLegs = 24,
    lookFeet = 95,
}
monster.runHealth = 25
monster.changeTarget = {
    interval = 5000,
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
        maxDamage = -45,
    },
}
monster.defenses = {
    defense = 11,
    armor = 11,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Your money or your life!", yell = false},
    {text = "Hand me your purse!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 48956,
        maxCount = 30,
    },
    {
        id = 2386,
        chance = 29905,
    },
    {
        id = 2511,
        chance = 16794,
    },
    {
        id = 2649,
        chance = 14967,
    },
    {
        id = 2398,
        chance = 10342,
    },
    {
        id = 2685,
        chance = 7692,
        maxCount = 2,
    },
    {
        id = 2458,
        chance = 4956,
    },
    {
        id = 2465,
        chance = 2376,
    },
    {
        id = 2459,
        chance = 544,
    },
}

mtype:register(monster)
