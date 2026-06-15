local mtype = Game.createMonsterType("Dark Apprentice")
local monster = {}

monster.name = "Dark Apprentice"
monster.description = "a dark apprentice"
monster.experience = 100
monster.race = "blood"
monster.maxHealth = 225
monster.health = 225
monster.speed = 190
monster.manaCost = 0
monster.corpse = 20363
monster.outfit = {
    lookType = 133,
    lookHead = 78,
    lookBody = 57,
    lookLegs = 95,
    lookFeet = 115,
    lookAddons = 1,
}
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -60,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -2,
        maxDamage = -26,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -10,
        maxDamage = -20,
        shootEffect = CONST_ANI_ENERGY,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -24,
        shootEffect = CONST_ANI_SMALLSTONE,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 1,
        range = 3,
        target = true,
        duration = 2000,
        shootEffect = CONST_ANI_EXPLOSION,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 1,
        radius = 4,
        target = false,
        duration = 2000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 30,
        maxDamage = 40,
        effect = CONST_ME_MAGIC_GREEN,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 5,
        target = true,
        duration = 3000,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Outch!", yell = false},
    {text = "Oops, I did it again.", yell = false},
    {text = "From the spirits that I called Sir, deliver me!", yell = false},
    {text = "I must dispose of my masters enemies!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 75041,
        maxCount = 45,
    },
    {
        id = 2260,
        chance = 8034,
        maxCount = 3,
    },
    {
        id = 7620,
        chance = 2995,
    },
    {
        id = 7618,
        chance = 2884,
    },
    {
        id = 2191,
        chance = 2004,
    },
    {
        id = 2188,
        chance = 108,
    },
    {
        id = 13295,
        chance = 7,
    },
}

mtype:register(monster)
