local mtype = Game.createMonsterType("Plaguesmith")
local monster = {}

monster.name = "Plaguesmith"
monster.description = "a plaguesmith"
monster.experience = 4500
monster.race = "venom"
monster.maxHealth = 8250
monster.health = 8250
monster.speed = 240
monster.manaCost = 0
monster.corpse = 6516
monster.outfit = { lookType = 247 }
monster.runHealth = 500
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -539,
        -- NOTE: melee condition (poison=200); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 200000,
        },
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        minDamage = -60,
        maxDamage = -114,
        radius = 4,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        minDamage = -100,
        maxDamage = -350,
        length = 5,
        spread = 3,
        effect = CONST_ME_YELLOW_RINGS,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        radius = 4,
        target = false,
        speed = -800,
        duration = 30000,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 40,
    armor = 40,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 200,
        maxDamage = 280,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 440,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 30},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = 1},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "You are looking a bit feverish!", yell = false},
    {text = "You don't look that good!", yell = false},
    {text = "Hachoo!", yell = false},
    {text = "Cough Cough", yell = false},
}
monster.loot = {
    {
        id = "emerald bangle",
        chance = 341,
    },
    {
        id = "silver brooch",
        chance = 2000,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 40000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 65,
    },
    {
        id = "small amethyst",
        chance = 5000,
        maxCount = 3,
    },
    {
        id = "platinum coin",
        chance = 7142,
        maxCount = 2,
    },
    {
        id = "axe ring",
        chance = 4347,
    },
    {
        id = "club ring",
        chance = 4761,
    },
    {
        id = "piece of iron",
        chance = 20000,
    },
    {
        id = "mouldy cheese",
        chance = 50000,
    },
    {
        id = 2237,
        chance = 60000,
    },
    {
        id = "two handed sword",
        chance = 20000,
    },
    {
        id = "war hammer",
        chance = 2127,
    },
    {
        id = "morning star",
        chance = 29000,
    },
    {
        id = "battle hammer",
        chance = 20000,
    },
    {
        id = "hammer of wrath",
        chance = 952,
    },
    {
        id = "knight legs",
        chance = 6250,
    },
    {
        id = "steel shield",
        chance = 20000,
    },
    {
        id = "steel boots",
        chance = 1123,
    },
    {
        id = 2079,
        chance = 100,
    },
    {
        id = "piece of royal steel",
        chance = 1234,
    },
    {
        id = "piece of hell steel",
        chance = 1010,
    },
    {
        id = "piece of draconian steel",
        chance = 1030,
    },
    {
        id = "soul orb",
        chance = 11111,
    },
    {
        id = "demonic essence",
        chance = 9033,
    },
    {
        id = "onyx arrow",
        chance = 7692,
        maxCount = 4,
    },
    {
        id = "great health potion",
        chance = 10000,
    },
    {
        id = 9810,
        chance = 540,
    },
}

mtype:register(monster)
