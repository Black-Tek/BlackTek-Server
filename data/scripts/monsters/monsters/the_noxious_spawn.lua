local mtype = Game.createMonsterType("The Noxious Spawn")
local monster = {}

monster.name = "The Noxious Spawn"
monster.description = "The Noxious Spawn"
monster.experience = 6000
monster.race = "venom"
monster.maxHealth = 9500
monster.health = 9500
monster.speed = 360
monster.manaCost = 0
monster.corpse = 4323
monster.outfit = { lookType = 220 }
monster.runHealth = 275
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -250,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 6,
        range = 7,
        speed = -660,
        duration = 5000,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 7,
        minDamage = 0,
        maxDamage = -550,
        length = 8,
        spread = 3,
        effect = CONST_ME_SMALLPLANTS,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 9,
        minDamage = 0,
        maxDamage = -550,
        length = 8,
        effect = CONST_ME_SOUND_RED,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 12,
        range = 1,
        minDamage = 0,
        maxDamage = -300,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 11,
        range = 7,
        duration = 4000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.defenses = {
    defense = 25,
    armor = 18,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 900,
        maxDamage = 1000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Sssssouls for the one", yell = false},
    {text = "HISSSS", yell = true},
    {text = "Tsssse one will risssse again", yell = false},
    {text = "I bring your deathhh, mortalssss", yell = false},
}
monster.loot = {
    {
        id = 10611,
        chance = 100000,
    },
    {
        id = 11230,
        chance = 100000,
    },
    {
        id = 2152,
        chance = 80822,
        maxCount = 5,
    },
    {
        id = 2149,
        chance = 71233,
        maxCount = 5,
    },
    {
        id = 7590,
        chance = 64384,
        maxCount = 4,
    },
    {
        id = 10309,
        chance = 45205,
    },
    {
        id = 2528,
        chance = 43836,
    },
    {
        id = 7456,
        chance = 42466,
    },
    {
        id = 7386,
        chance = 36986,
    },
    {
        id = 2033,
        chance = 32877,
    },
    {
        id = 7368,
        chance = 31507,
        maxCount = 98,
    },
    {
        id = 2487,
        chance = 30137,
    },
    {
        id = 2796,
        chance = 16438,
    },
    {
        id = 8902,
        chance = 16438,
    },
    {
        id = 2168,
        chance = 12329,
    },
    {
        id = 2498,
        chance = 2740,
    },
    {
        id = 8880,
        chance = 2740,
    },
}

mtype:register(monster)
