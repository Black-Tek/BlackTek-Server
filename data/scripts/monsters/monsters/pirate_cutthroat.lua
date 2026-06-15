local mtype = Game.createMonsterType("Pirate Cutthroat")
local monster = {}

monster.name = "Pirate Cutthroat"
monster.description = "a pirate cutthroat"
monster.experience = 175
monster.race = "blood"
monster.maxHealth = 325
monster.health = 325
monster.speed = 214
monster.manaCost = 495
monster.corpse = 20479
monster.outfit = { lookType = 96 }
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
        maxDamage = -170,
        -- NOTE: melee condition (poison=10); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 10000,
        },
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -95,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_EXPLOSION,
        effect = CONST_ME_EXPLOSIONAREA,
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
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Give up!", yell = false},
    {text = "Hiyaa!", yell = false},
    {text = "Plundeeeeer!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 77548,
        maxCount = 50,
    },
    {
        id = 11219,
        chance = 10074,
    },
    {
        id = 2483,
        chance = 3070,
    },
    {
        id = 2509,
        chance = 2884,
    },
    {
        id = 5710,
        chance = 1960,
    },
    {
        id = 14337,
        chance = 1092,
    },
    {
        id = 5918,
        chance = 1019,
    },
    {
        id = 5927,
        chance = 987,
    },
    {
        id = 6097,
        chance = 534,
    },
    {
        id = 6126,
        chance = 514,
    },
    {
        id = 6098,
        chance = 468,
    },
    {
        id = 5792,
        chance = 93,
    },
    {
        id = 5553,
        chance = 89,
    },
}

mtype:register(monster)
