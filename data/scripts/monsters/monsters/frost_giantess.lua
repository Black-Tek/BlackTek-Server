local mtype = Game.createMonsterType("Frost Giantess")
local monster = {}

monster.name = "Frost Giantess"
monster.description = "a frost giantess"
monster.experience = 150
monster.race = "blood"
monster.maxHealth = 275
monster.health = 275
monster.speed = 194
monster.manaCost = 490
monster.corpse = 7330
monster.outfit = { lookType = 265 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
monster.staticAttackChance = 60
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -60,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 30,
        range = 7,
        minDamage = 0,
        maxDamage = -90,
        shootEffect = CONST_ANI_LARGEROCK,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 300,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -3},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
}
monster.voices = {
    interval = 1000,
    chance = 5,
    {text = "Ymirs Mjalle!", yell = false},
    {text = "No run so much, must stay fat!", yell = false},
    {text = "Hörre Sjan Flan!", yell = false},
    {text = "Damned fast food.", yell = false},
    {text = "Come kiss the cook!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 82053,
        maxCount = 40,
    },
    {
        id = 2671,
        chance = 20794,
    },
    {
        id = 1294,
        chance = 10397,
        maxCount = 3,
    },
    {
        id = 2406,
        chance = 7946,
    },
    {
        id = 10575,
        chance = 4953,
    },
    {
        id = 7441,
        chance = 2093,
    },
    {
        id = 2513,
        chance = 1532,
    },
    {
        id = 7620,
        chance = 945,
    },
    {
        id = 7460,
        chance = 287,
    },
    {
        id = 2490,
        chance = 172,
    },
    {
        id = 7290,
        chance = 102,
    },
    {
        id = 2209,
        chance = 70,
    },
}

mtype:register(monster)
