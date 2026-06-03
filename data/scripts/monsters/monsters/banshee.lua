local mtype = Game.createMonsterType("Banshee")
local monster = {}

monster.name = "Banshee"
monster.description = "a banshee"
monster.experience = 900
monster.race = "undead"
monster.maxHealth = 1000
monster.health = 1000
monster.speed = 220
monster.manaCost = 0
monster.corpse = 6019
monster.outfit = { lookType = 78 }
monster.runHealth = 599
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
    canWalkOnEnergy = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -100,
        -- NOTE: melee condition (poison=3); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 3000,
        },
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        minDamage = -100,
        maxDamage = -200,
        radius = 4,
        target = false,
        effect = CONST_ME_SOUND_RED,
    },
    {
        name = "death",
        interval = 2000,
        chance = 5,
        range = 1,
        minDamage = -55,
        maxDamage = -350,
        radius = 1,
        target = true,
        effect = CONST_ME_SMALLCLOUDS,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        range = 7,
        speed = -300,
        duration = 15000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 120,
        maxDamage = 190,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Dance for me your dance of death!", yell = false},
    {text = "Let the music play!", yell = false},
    {text = "I will mourn your death!", yell = false},
    {text = "Are you ready to rock?", yell = false},
    {text = "Feel my gentle kiss of death.", yell = false},
    {text = "That's what I call easy listening!", yell = false},
    {text = "IIIIEEEeeeeeehhhHHHH!", yell = true},
}
monster.loot = {
    {
        id = "candlestick",
        chance = 70000,
    },
    {
        id = "gold coin",
        chance = 30000,
        maxCount = 80,
    },
    {
        id = "silver amulet",
        chance = 9400,
    },
    {
        id = "simple dress",
        chance = 6300,
    },
    {
        id = "hair of a banshee",
        chance = 5000,
    },
    {
        id = "petrified scream",
        chance = 4250,
    },
    {
        id = "black pearl",
        chance = 2130,
    },
    {
        id = "silver brooch",
        chance = 1550,
    },
    {
        id = "poison dagger",
        chance = 1550,
    },
    {
        id = "white pearl",
        chance = 1110,
    },
    {
        id = 2372,
        chance = 970,
    },
    {
        id = "stone skin amulet",
        chance = 820,
    },
    {
        id = "strong mana potion",
        chance = 770,
    },
    {
        id = "ring of healing",
        chance = 750,
    },
    {
        id = "blue robe",
        chance = 700,
    },
    {
        id = "wedding ring",
        chance = 560,
    },
    {
        id = 2175,
        chance = 520,
    },
    {
        id = "terra mantle",
        chance = 340,
    },
    {
        id = 2124,
        chance = 180,
    },
    {
        id = "red robe",
        chance = 150,
    },
    {
        id = "life crystal",
        chance = 100,
    },
    {
        id = "sweet smelling bait",
        chance = 40,
    },
}

mtype:register(monster)
