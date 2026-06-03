local mtype = Game.createMonsterType("Roaring Lion")
local monster = {}

monster.name = "Roaring Lion"
monster.description = "a roaring lion"
monster.experience = 800
monster.race = "blood"
monster.maxHealth = 1000
monster.health = 1000
monster.speed = 210
monster.manaCost = 0
monster.corpse = 21420
monster.outfit = { lookType = 570 }
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
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -130,
    },
}
monster.defenses = {
    defense = 28,
    armor = 28,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 50},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Groarrrr! Rwarrrr!", yell = false},
    {text = "Growl!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 115,
    },
    {
        id = 2666,
        chance = 25326,
    },
    {
        id = 2671,
        chance = 24811,
    },
    {
        id = 10608,
        chance = 20073,
    },
    {
        id = 2152,
        chance = 9973,
    },
    {
        id = 2146,
        chance = 7149,
    },
    {
        id = 9970,
        chance = 6926,
    },
    {
        id = 2147,
        chance = 6842,
    },
    {
        id = 2150,
        chance = 6700,
    },
    {
        id = 2193,
        chance = 2857,
    },
    {
        id = 2164,
        chance = 731,
    },
    {
        id = 2491,
        chance = 581,
    },
}

mtype:register(monster)
