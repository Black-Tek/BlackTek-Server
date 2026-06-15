local mtype = Game.createMonsterType("Renegade Orc")
local monster = {}

monster.name = "Renegade Orc"
monster.description = "a renegade orc"
monster.experience = 270
monster.race = "blood"
monster.maxHealth = 450
monster.health = 450
monster.speed = 220
monster.manaCost = 0
monster.corpse = 6001
monster.outfit = { lookType = 59 }
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
    canPushCreatures = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -130,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -50,
        shootEffect = CONST_ANI_THROWINGSTAR,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -2},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "fire", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Harga puchak muhmak!", yell = false},
}
monster.loot = {
    {
        id = 2667,
        chance = 30000,
    },
    {
        id = 2148,
        chance = 28000,
        maxCount = 35,
    },
    {
        id = 12435,
        chance = 19000,
    },
    {
        id = 2510,
        chance = 10000,
    },
    {
        id = 2410,
        chance = 9850,
        maxCount = 4,
    },
    {
        id = 2789,
        chance = 9650,
    },
    {
        id = 2207,
        chance = 3920,
    },
    {
        id = 2397,
        chance = 2800,
    },
    {
        id = 7378,
        chance = 2600,
    },
    {
        id = 2419,
        chance = 2100,
    },
    {
        id = 11113,
        chance = 890,
    },
    {
        id = 2413,
        chance = 830,
    },
    {
        id = 7618,
        chance = 550,
    },
    {
        id = 2647,
        chance = 420,
    },
    {
        id = 2475,
        chance = 160,
    },
}

mtype:register(monster)
