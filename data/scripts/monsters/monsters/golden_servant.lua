local mtype = Game.createMonsterType("Golden Servant")
local monster = {}

monster.name = "Golden Servant"
monster.description = "a golden servant"
monster.experience = 450
monster.race = "energy"
monster.maxHealth = 550
monster.health = 550
monster.speed = 210
monster.manaCost = 0
monster.corpse = 13489
monster.outfit = { lookType = 396 }
monster.runHealth = 50
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
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -100,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 15,
        minDamage = -60,
        maxDamage = -100,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_YELLOWENERGY,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 15,
        minDamage = -80,
        maxDamage = -110,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 10,
        minDamage = -90,
        maxDamage = -150,
        length = 5,
        spread = 2,
        target = false,
        effect = CONST_ME_YELLOWENERGY,
    },
}
monster.defenses = {
    defense = 29,
    armor = 29,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 25},
    {type = COMBAT_EARTHDAMAGE, percent = 80},
    {type = COMBAT_FIREDAMAGE, percent = 15},
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "holy", combat = true, condition = true},
    {type = "invisible", condition = true},
    {type = "paralyze", condition = true},
}
monster.voices = {
    interval = 1000,
    chance = 5,
    {text = "Error. LOAD 'PROGRAM',8,1", yell = false},
    {text = "Remain. Obedient.", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 84992,
        maxCount = 140,
    },
    {
        id = 7618,
        chance = 4962,
    },
    {
        id = 7620,
        chance = 4960,
    },
    {
        id = 2381,
        chance = 3030,
    },
    {
        id = 2796,
        chance = 1510,
    },
    {
        id = 9690,
        chance = 943,
    },
    {
        id = 8900,
        chance = 524,
    },
    {
        id = 2165,
        chance = 478,
    },
    {
        id = 13758,
        chance = 356,
    },
    {
        id = 13938,
        chance = 36,
    },
    {
        id = 2466,
        chance = 22,
    },
    {
        id = 2179,
        chance = 8,
    },
}

mtype:register(monster)
