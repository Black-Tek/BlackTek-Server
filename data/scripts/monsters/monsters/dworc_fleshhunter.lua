local mtype = Game.createMonsterType("Dworc Fleshhunter")
local monster = {}

monster.name = "Dworc Fleshhunter"
monster.description = "a dworc fleshhunter"
monster.experience = 40
monster.race = "blood"
monster.maxHealth = 85
monster.health = 85
monster.speed = 148
monster.manaCost = 0
monster.corpse = 6058
monster.outfit = { lookType = 215 }
monster.runHealth = 8
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
        maxDamage = -25,
        -- NOTE: melee condition (poison=20); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 20000,
        },
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -15,
        shootEffect = CONST_ANI_THROWINGKNIFE,
    },
}
monster.defenses = {
    defense = 3,
    armor = 3,
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = -13},
    {type = COMBAT_FIREDAMAGE, percent = -8},
    {type = COMBAT_ICEDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Grak brrretz!", yell = false},
    {text = "Grow truk grrrrr.", yell = false},
    {text = "Prek tars, dekklep zurk.", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 74874,
        maxCount = 13,
    },
    {
        id = 2467,
        chance = 10817,
    },
    {
        id = 2568,
        chance = 8991,
    },
    {
        id = 2050,
        chance = 5553,
    },
    {
        id = 2229,
        chance = 3027,
        maxCount = 3,
    },
    {
        id = 2411,
        chance = 2061,
    },
    {
        id = 3965,
        chance = 1950,
    },
    {
        id = 2541,
        chance = 1035,
    },
    {
        id = 3967,
        chance = 480,
    },
    {
        id = 3964,
        chance = 97,
    },
}

mtype:register(monster)
