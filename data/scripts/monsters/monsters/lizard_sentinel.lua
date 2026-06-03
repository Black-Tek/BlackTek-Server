local mtype = Game.createMonsterType("Lizard Sentinel")
local monster = {}

monster.name = "Lizard Sentinel"
monster.description = "a lizard sentinel"
monster.experience = 110
monster.race = "blood"
monster.maxHealth = 265
monster.health = 265
monster.speed = 180
monster.manaCost = 560
monster.corpse = 6040
monster.outfit = { lookType = 114 }
monster.runHealth = 10
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
    convinceable = true,
    pushable = true,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -45,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 30,
        range = 7,
        minDamage = 0,
        maxDamage = -70,
        shootEffect = CONST_ANI_SPEAR,
    },
}
monster.defenses = {
    defense = 17,
    armor = 17,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Tssss!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 86497,
        maxCount = 54,
    },
    {
        id = 2389,
        chance = 10100,
        maxCount = 3,
    },
    {
        id = 2464,
        chance = 9189,
    },
    {
        id = 2483,
        chance = 8069,
    },
    {
        id = 3965,
        chance = 4424,
    },
    {
        id = 5876,
        chance = 2207,
    },
    {
        id = 5881,
        chance = 2031,
    },
    {
        id = 2381,
        chance = 1504,
    },
    {
        id = 2425,
        chance = 1317,
    },
    {
        id = 7618,
        chance = 1219,
    },
    {
        id = 3974,
        chance = 373,
    },
    {
        id = 2145,
        chance = 88,
    },
}

mtype:register(monster)
