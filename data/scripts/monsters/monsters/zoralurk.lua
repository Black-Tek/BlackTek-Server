local mtype = Game.createMonsterType("Zoralurk")
local monster = {}

monster.name = "Zoralurk"
monster.description = "Zoralurk"
monster.experience = 30000
monster.race = "undead"
monster.maxHealth = 55000
monster.health = 55000
monster.speed = 400
monster.manaCost = 0
monster.corpse = 6068
monster.outfit = {
    lookType = 12,
    lookHead = 0,
    lookBody = 98,
    lookLegs = 86,
    lookFeet = 94,
}
monster.changeTarget = {
    interval = 10000,
    chance = 20,
}
monster.targetDistance = 1
monster.staticAttackChance = 98
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
        maxDamage = -1013,
    },
    {
        name = "energy",
        interval = 1000,
        chance = 12,
        minDamage = -600,
        maxDamage = -900,
        radius = 7,
        target = false,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "earth",
        interval = 1000,
        chance = 12,
        minDamage = -400,
        maxDamage = -800,
        radius = 7,
        target = false,
        effect = CONST_ME_SMALLPLANTS,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 25,
        range = 7,
        minDamage = -500,
        maxDamage = -800,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "fire",
        interval = 3000,
        chance = 35,
        range = 7,
        minDamage = -200,
        maxDamage = -600,
        radius = 7,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
}
monster.defenses = {
    defense = 65,
    armor = 55,
    {
        name = "healing",
        interval = 2000,
        chance = 35,
        minDamage = 300,
        maxDamage = 800,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 4000,
        chance = 80,
        speed = 440,
        duration = 6000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 10,
        duration = 10000,
        effect = CONST_ME_CRAPS,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 10,
        duration = 10000,
        effect = CONST_ME_CRAPS,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 10,
        duration = 10000,
        effect = CONST_ME_CRAPS,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 10,
        duration = 10000,
        effect = CONST_ME_CRAPS,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 10,
        duration = 10000,
        effect = CONST_ME_CRAPS,
    },
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "demon", interval = 4000, chance = 50, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 30,
    {text = "I AM ZORALURK, THE DEMON WITH A THOUSAND FACES!", yell = true},
    {text = "BRING IT, COCKROACHES!", yell = true},
}
monster.loot = {
    {
        id = "white pearl",
        chance = 10000,
        maxCount = 5,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 90,
    },
    {
        id = "boots of haste",
        chance = 16033,
    },
    {
        id = "giant sword",
        chance = 60000,
    },
    {
        id = "bright sword",
        chance = 20000,
    },
    {
        id = "bright sword",
        chance = 20000,
    },
    {
        id = "warlord sword",
        chance = 6000,
    },
    {
        id = "patched boots",
        chance = 7000,
    },
    {
        id = 6530,
        chance = 16000,
    },
}

mtype:register(monster)
