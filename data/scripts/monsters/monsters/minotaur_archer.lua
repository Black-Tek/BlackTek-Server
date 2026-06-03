local mtype = Game.createMonsterType("Minotaur Archer")
local monster = {}

monster.name = "Minotaur Archer"
monster.description = "a minotaur archer"
monster.experience = 65
monster.race = "blood"
monster.maxHealth = 100
monster.health = 100
monster.speed = 160
monster.manaCost = 390
monster.corpse = 5982
monster.outfit = { lookType = 24 }
monster.runHealth = 29
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
    pushable = false,
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
        chance = 40,
        range = 7,
        minDamage = 0,
        maxDamage = -80,
        target = true,
        shootEffect = CONST_ANI_BOLT,
    },
}
monster.defenses = {
    defense = 6,
    armor = 6,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Ruan Wihmpy!", yell = false},
    {text = "Kaplar!", yell = false},
}
monster.loot = {
    {
        id = 2543,
        chance = 90124,
        maxCount = 20,
    },
    {
        id = 2148,
        chance = 75036,
        maxCount = 30,
    },
    {
        id = 12407,
        chance = 15323,
    },
    {
        id = 7363,
        chance = 12190,
        maxCount = 4,
    },
    {
        id = 12439,
        chance = 8034,
    },
    {
        id = 2666,
        chance = 4952,
    },
    {
        id = 12428,
        chance = 1973,
        maxCount = 2,
    },
    {
        id = 5878,
        chance = 1925,
    },
    {
        id = 2455,
        chance = 877,
    },
    {
        id = 2483,
        chance = 539,
    },
    {
        id = 2465,
        chance = 519,
    },
}

mtype:register(monster)
