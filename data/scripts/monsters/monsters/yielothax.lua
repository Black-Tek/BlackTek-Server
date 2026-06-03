local mtype = Game.createMonsterType("Yielothax")
local monster = {}

monster.name = "Yielothax"
monster.description = "Yielothax"
monster.experience = 1250
monster.race = "venom"
monster.maxHealth = 1500
monster.health = 1500
monster.speed = 240
monster.manaCost = 0
monster.corpse = 13752
monster.outfit = { lookType = 408 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 75
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
        interval = 1000,
        minDamage = 0,
        maxDamage = -203,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -100,
        maxDamage = -130,
        length = 4,
        effect = CONST_ME_ENERGYAREA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        minDamage = -150,
        maxDamage = -250,
        radius = 3,
        target = false,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        minDamage = -70,
        maxDamage = -120,
        radius = 3,
        target = true,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        minDamage = -50,
        maxDamage = -150,
        length = 4,
        spread = 3,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 100,
        maxDamage = 150,
        effect = CONST_ME_HITBYPOISON,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 50},
    {type = COMBAT_FIREDAMAGE, percent = 25},
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "IIEEH!! Iiih iih ih iiih!!!", yell = true},
    {text = "Bsssssssm Bssssssm Bsssssssssssm!", yell = true},
}
monster.loot = {
    {
        id = "small diamond",
        chance = 4761,
        maxCount = 5,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 27,
    },
    {
        id = "talon",
        chance = 917,
    },
    {
        id = "might ring",
        chance = 3846,
    },
    {
        id = "wand of cosmic energy",
        chance = 523,
    },
    {
        id = "epee",
        chance = 564,
    },
    {
        id = "brown mushroom",
        chance = 9090,
        maxCount = 3,
    },
    {
        id = "mastermind potion",
        chance = 490,
    },
    {
        id = "strong health potion",
        chance = 20000,
    },
    {
        id = "strong mana potion",
        chance = 20000,
    },
    {
        id = "lightning pendant",
        chance = 862,
    },
    {
        id = "lightning legs",
        chance = 480,
    },
    {
        id = "shockwave amulet",
        chance = 571,
    },
    {
        id = "broken ring of ending",
        chance = 261,
    },
    {
        id = "yielowax",
        chance = 300,
    },
    {
        id = "yielocks",
        chance = 320,
    },
}

mtype:register(monster)
