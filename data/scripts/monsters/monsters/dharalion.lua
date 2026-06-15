local mtype = Game.createMonsterType("Dharalion")
local monster = {}

monster.name = "Dharalion"
monster.description = "Dharalion"
monster.experience = 570
monster.race = "blood"
monster.maxHealth = 380
monster.health = 380
monster.speed = 240
monster.manaCost = 0
monster.corpse = 6011
monster.outfit = { lookType = 203 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 4
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -30,
    },
    {
        name = "manadrain",
        interval = 1000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -100,
    },
    {
        name = "energy",
        interval = 1000,
        chance = 13,
        range = 7,
        minDamage = -80,
        maxDamage = -100,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "death",
        interval = 1000,
        chance = 10,
        range = 7,
        minDamage = -80,
        maxDamage = -151,
        shootEffect = CONST_ANI_SUDDENDEATH,
    },
}
monster.defenses = {
    defense = 25,
    armor = 15,
    {
        name = "healing",
        interval = 1000,
        chance = 20,
        minDamage = 90,
        maxDamage = 150,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 7,
        speed = 300,
        duration = 10000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "demon skeleton", interval = 1000, chance = 6, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Feel my wrath!", yell = false},
    {text = "Noone will stop my ascension!", yell = false},
    {text = "My powers are divine!", yell = false},
    {text = "You desecrated this temple!", yell = false},
    {text = "Muahahaha!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 100,
    },
    {
        id = 5922,
        chance = 100000,
    },
    {
        id = 10552,
        chance = 87671,
    },
    {
        id = 12421,
        chance = 83562,
    },
    {
        id = 2154,
        chance = 41096,
    },
    {
        id = 2682,
        chance = 24658,
    },
    {
        id = 2260,
        chance = 23288,
    },
    {
        id = 2198,
        chance = 16438,
    },
    {
        id = 2689,
        chance = 15068,
    },
    {
        id = 7590,
        chance = 13699,
    },
    {
        id = 2177,
        chance = 12329,
    },
    {
        id = 2802,
        chance = 10959,
    },
    {
        id = 3957,
        chance = 9589,
    },
    {
        id = 2652,
        chance = 5479,
    },
    {
        id = 7378,
        chance = 1370,
        maxCount = 2,
    },
}

mtype:register(monster)
