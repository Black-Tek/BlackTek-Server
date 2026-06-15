local mtype = Game.createMonsterType("Quara Mantassin")
local monster = {}

monster.name = "Quara Mantassin"
monster.description = "a quara mantassin"
monster.experience = 400
monster.race = "blood"
monster.maxHealth = 800
monster.health = 800
monster.speed = 590
monster.manaCost = 480
monster.corpse = 6064
monster.outfit = { lookType = 72 }
monster.runHealth = 40
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -140,
    },
}
monster.defenses = {
    defense = 16,
    armor = 16,
    {
        name = "invisible",
        interval = 2000,
        chance = 20,
        duration = 3000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 400,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "drown", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Zuerk Pachak!", yell = false},
    {text = "Shrrrr", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 79873,
        maxCount = 130,
    },
    {
        id = 12445,
        chance = 12746,
    },
    {
        id = 2670,
        chance = 5121,
    },
    {
        id = 2381,
        chance = 4984,
    },
    {
        id = 2165,
        chance = 1100,
    },
    {
        id = 2146,
        chance = 1092,
    },
    {
        id = 2654,
        chance = 1055,
    },
    {
        id = 2377,
        chance = 1017,
    },
    {
        id = 5895,
        chance = 622,
    },
    {
        id = 2479,
        chance = 91,
    },
    {
        id = 2656,
        chance = 83,
    },
}

mtype:register(monster)
