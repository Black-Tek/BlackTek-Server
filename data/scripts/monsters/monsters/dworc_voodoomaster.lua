local mtype = Game.createMonsterType("Dworc Voodoomaster")
local monster = {}

monster.name = "Dworc Voodoomaster"
monster.experience = 55
monster.race = "blood"
monster.maxHealth = 80
monster.health = 80
monster.speed = 150
monster.manaCost = 0
monster.corpse = 6055
monster.outfit = { lookType = 214 }
monster.runHealth = 15
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
    illusionable = true,
    convinceable = false,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -20,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        range = 1,
        minDamage = 0,
        maxDamage = -40,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        range = 7,
        speed = -800,
        duration = 5000,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        range = 7,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 10,
        range = 7,
        duration = 5000,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        minDamage = -6,
        maxDamage = -18,
        radius = 6,
        target = false,
    },
    {
        name = "poisonfield",
        interval = 2000,
        chance = 10,
        range = 7,
        radius = 1,
        target = true,
    },
}
monster.defenses = {
    defense = 3,
    armor = 3,
    {
        name = "healing",
        interval = 2000,
        chance = 20,
        minDamage = 3,
        maxDamage = 9,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 200,
        duration = 4000,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 15,
        duration = 7000,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 35},
    {type = COMBAT_FIREDAMAGE, percent = -15},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Brak brrretz!", yell = false},
    {text = "Grow truk grrrrr.", yell = false},
    {text = "Prek tars, dekklep zurk.", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 74835,
        maxCount = 17,
    },
    {
        id = 2467,
        chance = 10093,
    },
    {
        id = 2230,
        chance = 5812,
    },
    {
        id = 2050,
        chance = 5547,
    },
    {
        id = 2231,
        chance = 3066,
    },
    {
        id = 2229,
        chance = 1973,
        maxCount = 3,
    },
    {
        id = 2411,
        chance = 1009,
    },
    {
        id = 7618,
        chance = 586,
    },
    {
        id = 2174,
        chance = 503,
    },
    {
        id = 3967,
        chance = 486,
    },
    {
        id = 3955,
        chance = 120,
    },
}

mtype:register(monster)
