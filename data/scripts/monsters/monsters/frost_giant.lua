local mtype = Game.createMonsterType("Frost Giant")
local monster = {}

monster.name = "Frost Giant"
monster.description = "a frost giant"
monster.experience = 150
monster.race = "blood"
monster.maxHealth = 270
monster.health = 270
monster.speed = 190
monster.manaCost = 490
monster.corpse = 7330
monster.outfit = { lookType = 257 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
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
        maxDamage = -110,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -90,
        shootEffect = CONST_ANI_LARGEROCK,
    },
}
monster.defenses = {
    defense = 22,
    armor = 22,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 300,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
}
monster.voices = {
    interval = 1000,
    chance = 5,
    {text = "Hmm Humansoup!", yell = false},
    {text = "Stand still ya tasy snack!", yell = false},
    {text = "Joh Thun!", yell = false},
    {text = "Hörre Sjan Flan!", yell = false},
    {text = "Bröre Smöde!", yell = false},
    {text = "Forle Bramma", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 82076,
        maxCount = 40,
    },
    {
        id = 2406,
        chance = 8228,
    },
    {
        id = 10575,
        chance = 4926,
    },
    {
        id = 2666,
        chance = 4919,
    },
    {
        id = 7441,
        chance = 2204,
    },
    {
        id = 2513,
        chance = 1386,
    },
    {
        id = 7618,
        chance = 905,
    },
    {
        id = 2381,
        chance = 556,
    },
    {
        id = 7460,
        chance = 287,
    },
    {
        id = 2490,
        chance = 169,
    },
    {
        id = 2209,
        chance = 119,
    },
    {
        id = 7290,
        chance = 62,
    },
}

mtype:register(monster)
