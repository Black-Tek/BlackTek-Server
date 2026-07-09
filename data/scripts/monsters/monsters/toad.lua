local mtype = Game.createMonsterType("Toad")
local monster = {}

monster.name = "Toad"
monster.description = "a toad"
monster.experience = 60
monster.race = "blood"
monster.maxHealth = 135
monster.health = 135
monster.speed = 210
monster.manaCost = 400
monster.corpse = 6077
monster.outfit = { lookType = 222 }
monster.runHealth = 10
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
    convinceable = false,
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
        maxDamage = -30,
        -- NOTE: melee condition (poison=20); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 20000,
        },
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -8,
        maxDamage = -17,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_GREEN_RINGS,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 200,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Ribbit, ribbit!", yell = false},
    {text = "Ribbit!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 80000,
        maxCount = 20,
    },
    {
        id = "war hammer",
        chance = 148,
    },
    {
        id = "mace",
        chance = 2854,
    },
    {
        id = 2667,
        chance = 20000,
    },
    {
        id = "poisonous slime",
        chance = 4761,
    },
}

mtype:register(monster)
