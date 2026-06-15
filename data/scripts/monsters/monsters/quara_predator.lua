local mtype = Game.createMonsterType("Quara Predator")
local monster = {}

monster.name = "Quara Predator"
monster.description = "a quara predator"
monster.experience = 1600
monster.race = "blood"
monster.maxHealth = 2200
monster.health = 2200
monster.speed = 450
monster.manaCost = 0
monster.corpse = 6067
monster.outfit = { lookType = 20 }
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
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -470,
    },
}
monster.defenses = {
    defense = 40,
    armor = 40,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 270,
        duration = 5000,
        effect = CONST_ME_MAGIC_GREEN,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 25,
        maxDamage = 75,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "invisible", condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Rrrah!", yell = false},
    {text = "Rraaar!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 78107,
        maxCount = 154,
    },
    {
        id = 12447,
        chance = 9797,
    },
    {
        id = 7378,
        chance = 9260,
        maxCount = 7,
    },
    {
        id = 2145,
        chance = 5322,
        maxCount = 2,
    },
    {
        id = 2670,
        chance = 4985,
    },
    {
        id = 2387,
        chance = 3005,
    },
    {
        id = 5895,
        chance = 1922,
    },
    {
        id = 7591,
        chance = 1034,
    },
    {
        id = 7383,
        chance = 657,
    },
    {
        id = 7368,
        chance = 528,
    },
    {
        id = 7897,
        chance = 462,
    },
    {
        id = 5741,
        chance = 382,
    },
    {
        id = 13305,
        chance = 22,
    },
}

mtype:register(monster)
