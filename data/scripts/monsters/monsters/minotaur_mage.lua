local mtype = Game.createMonsterType("Minotaur Mage")
local monster = {}

monster.name = "Minotaur Mage"
monster.description = "a minotaur mage"
monster.experience = 150
monster.race = "blood"
monster.maxHealth = 155
monster.health = 155
monster.speed = 170
monster.manaCost = 0
monster.corpse = 5981
monster.outfit = { lookType = 23 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
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
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -20,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -20,
        maxDamage = -58,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -50,
        maxDamage = -105,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "energyfield",
        interval = 2000,
        chance = 10,
        range = 7,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_ENERGYBALL,
    },
}
monster.defenses = {
    defense = 18,
    armor = 18,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Learrn tha secrret uf deathhh!", yell = false},
    {text = "Kaplar!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 85291,
        maxCount = 35,
    },
    {
        id = 2684,
        chance = 14690,
        maxCount = 8,
    },
    {
        id = 12429,
        chance = 6068,
    },
    {
        id = 2050,
        chance = 5008,
    },
    {
        id = 2649,
        chance = 4908,
    },
    {
        id = 2461,
        chance = 3197,
    },
    {
        id = 12428,
        chance = 3096,
        maxCount = 2,
    },
    {
        id = 5878,
        chance = 2090,
    },
    {
        id = 7425,
        chance = 959,
    },
    {
        id = 2189,
        chance = 556,
    },
    {
        id = 7620,
        chance = 470,
    },
}

mtype:register(monster)
