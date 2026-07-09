local mtype = Game.createMonsterType("Dwarf Geomancer")
local monster = {}

monster.name = "Dwarf Geomancer"
monster.description = "a dwarf geomancer"
monster.experience = 265
monster.race = "blood"
monster.maxHealth = 380
monster.health = 380
monster.speed = 200
monster.manaCost = 0
monster.corpse = 6015
monster.outfit = { lookType = 66 }
monster.runHealth = 150
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
monster.staticAttackChance = 70
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -100,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -50,
        maxDamage = -110,
        shootEffect = CONST_ANI_SMALLEARTH,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -25,
        maxDamage = -80,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 40,
        minDamage = 25,
        maxDamage = 130,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = 60},
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Hail Durin!", yell = false},
    {text = "Earth is the strongest element.", yell = false},
    {text = "Dust to dust.", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 75000,
        maxCount = 45,
    },
    {
        id = "white mushroom",
        chance = 60000,
        maxCount = 2,
    },
    {
        id = "blank rune",
        chance = 33000,
    },
    {
        id = "pear",
        chance = 24000,
    },
    {
        id = 2162,
        chance = 14000,
    },
    {
        id = "geomancer's robe",
        chance = 8000,
    },
    {
        id = "geomancer's staff",
        chance = 7000,
    },
    {
        id = "clerical mace",
        chance = 1120,
    },
    {
        id = "dwarven ring",
        chance = 580,
    },
    {
        id = "small sapphire",
        chance = 580,
    },
    {
        id = 2175,
        chance = 500,
    },
    {
        id = "terra boots",
        chance = 500,
    },
    {
        id = "iron ore",
        chance = 290,
    },
}

mtype:register(monster)
