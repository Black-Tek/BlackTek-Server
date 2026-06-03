local mtype = Game.createMonsterType("Plagirath")
local monster = {}

monster.name = "Plagirath"
monster.description = "Plagirath"
monster.experience = 50000
monster.race = "venom"
monster.maxHealth = 290000
monster.health = 290000
monster.speed = 320
monster.manaCost = 0
monster.corpse = 25151
monster.outfit = {
    lookType = 862,
    lookHead = 83,
    lookBody = 61,
    lookLegs = 98,
    lookFeet = 79,
    lookAddons = 1,
}
monster.changeTarget = {
    interval = 5000,
    chance = 8,
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
    canWalkOnEnergy = true,
    canWalkOnFire = true,
    canWalkOnPoison = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -1350,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        minDamage = -500,
        maxDamage = -900,
        radius = 4,
        target = false,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "death",
        interval = 2000,
        chance = 20,
        range = 4,
        minDamage = -500,
        maxDamage = -900,
        radius = 4,
        target = true,
        effect = CONST_ME_POFF,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        minDamage = -1000,
        maxDamage = -1200,
        length = 10,
        spread = 3,
        effect = CONST_ME_POFF,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 20,
        minDamage = -1500,
        maxDamage = -1900,
        length = 10,
        effect = CONST_ME_POFF,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        radius = 7,
        speed = -600,
        duration = 20000,
        effect = CONST_ME_MAGIC_GREEN,
    },
}
monster.defenses = {
    defense = 55,
    armor = 55,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 3000,
        maxDamage = 4000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 30,
        speed = 440,
        duration = 6000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -25},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "BOOOOOOOMMM!!!!!", yell = false},
    {text = "WITHER AND DIE!", yell = true},
    {text = "DEATH AND DECAY!", yell = true},
    {text = "I CAN SENSE YOUR BODIES ROOTING!", yell = true},
}
monster.loot = {
    {
        id = 25172,
        chance = 100000,
    },
    {
        id = 11306,
        chance = 3000,
    },
    {
        id = 18411,
        chance = 1820,
    },
    {
        id = 18419,
        chance = 23000,
        maxCount = 6,
    },
    {
        id = 18420,
        chance = 23000,
        maxCount = 6,
    },
    {
        id = 18421,
        chance = 23000,
        maxCount = 6,
    },
    {
        id = 2143,
        chance = 12000,
        maxCount = 8,
    },
    {
        id = 2146,
        chance = 12000,
        maxCount = 9,
    },
    {
        id = 2148,
        chance = 49000,
        maxCount = 100,
    },
    {
        id = 2148,
        chance = 49000,
        maxCount = 100,
    },
    {
        id = 2150,
        chance = 10000,
        maxCount = 5,
    },
    {
        id = 2152,
        chance = 8000,
        maxCount = 58,
    },
    {
        id = 25383,
        chance = 800,
    },
    {
        id = 25415,
        chance = 500,
    },
    {
        id = 25522,
        chance = 800,
    },
    {
        id = 25523,
        chance = 800,
    },
    {
        id = 6500,
        chance = 11000,
    },
    {
        id = 7386,
        chance = 5000,
    },
    {
        id = 7632,
        chance = 14000,
        maxCount = 5,
    },
    {
        id = 7633,
        chance = 14000,
        maxCount = 5,
    },
    {
        id = 7887,
        chance = 5000,
    },
    {
        id = 8473,
        chance = 23000,
        maxCount = 15,
    },
    {
        id = 8901,
        chance = 4000,
    },
    {
        id = 9970,
        chance = 10000,
        maxCount = 8,
    },
}

mtype:register(monster)
