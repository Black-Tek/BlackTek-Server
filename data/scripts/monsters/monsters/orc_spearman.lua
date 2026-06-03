local mtype = Game.createMonsterType("Orc Spearman")
local monster = {}

monster.name = "Orc Spearman"
monster.description = "an orc spearman"
monster.experience = 38
monster.race = "blood"
monster.maxHealth = 105
monster.health = 105
monster.speed = 176
monster.manaCost = 310
monster.corpse = 5996
monster.outfit = { lookType = 50 }
monster.runHealth = 10
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = true,
    canPushItems = false,
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
        maxDamage = -25,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -30,
        shootEffect = CONST_ANI_SPEAR,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Ugaar!", yell = false},
}
monster.loot = {
    {
        id = 2666,
        chance = 30387,
    },
    {
        id = 2148,
        chance = 24846,
        maxCount = 11,
    },
    {
        id = 2389,
        chance = 17907,
    },
    {
        id = 2468,
        chance = 9210,
    },
    {
        id = 2482,
        chance = 8239,
    },
    {
        id = 2420,
        chance = 3027,
    },
    {
        id = 12435,
        chance = 2013,
    },
    {
        id = 11113,
        chance = 100,
    },
}

mtype:register(monster)
