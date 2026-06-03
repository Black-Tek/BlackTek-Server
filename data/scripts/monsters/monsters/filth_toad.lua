local mtype = Game.createMonsterType("Filth Toad")
local monster = {}

monster.name = "Filth Toad"
monster.description = "a filth toad"
monster.experience = 90
monster.race = "blood"
monster.maxHealth = 185
monster.health = 185
monster.speed = 210
monster.manaCost = 0
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
    summonable = false,
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
        chance = 75000,
        maxCount = 21,
    },
    {
        id = "two handed sword",
        chance = 500,
    },
    {
        id = "mace",
        chance = 2000,
    },
    {
        id = 2667,
        chance = 22000,
    },
    {
        id = "poisonous slime",
        chance = 3000,
    },
}

mtype:register(monster)
