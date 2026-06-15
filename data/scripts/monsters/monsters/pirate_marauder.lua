local mtype = Game.createMonsterType("Pirate Marauder")
local monster = {}

monster.name = "Pirate Marauder"
monster.description = "a pirate marauder"
monster.experience = 125
monster.race = "blood"
monster.maxHealth = 210
monster.health = 210
monster.speed = 200
monster.manaCost = 490
monster.corpse = 20483
monster.outfit = { lookType = 93 }
monster.runHealth = 20
monster.changeTarget = {
    interval = 4000,
    chance = 15,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
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
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -140,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -40,
        shootEffect = CONST_ANI_SPEAR,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Plundeeeeer!", yell = false},
    {text = "Give up!", yell = false},
    {text = "Hiyaa!", yell = false},
}
monster.loot = {
    {
        id = 2050,
        chance = 9880,
    },
    {
        id = "gold coin",
        chance = 77670,
        maxCount = 40,
    },
    {
        id = "spear",
        chance = 5140,
        maxCount = 2,
    },
    {
        id = "chain armor",
        chance = 3000,
    },
    {
        id = "plate shield",
        chance = 5000,
    },
    {
        id = 5091,
        chance = 910,
    },
    {
        id = "rum flask",
        chance = 110,
    },
    {
        id = 5792,
        chance = 90,
    },
    {
        id = "bandana",
        chance = 880,
    },
    {
        id = "pirate bag",
        chance = 430,
    },
    {
        id = "empty goldfish bowl",
        chance = 80,
    },
    {
        id = "hook",
        chance = 520,
    },
    {
        id = "eye patch",
        chance = 530,
    },
    {
        id = "peg leg",
        chance = 520,
    },
    {
        id = "compass",
        chance = 9720,
    },
}

mtype:register(monster)
