local mtype = Game.createMonsterType("Weeper")
local monster = {}

monster.name = "Weeper"
monster.description = "a weeper"
monster.experience = 4800
monster.race = "fire"
monster.maxHealth = 6800
monster.health = 6800
monster.speed = 260
monster.manaCost = 0
monster.corpse = 17252
monster.outfit = { lookType = 489 }
monster.runHealth = 570
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
        maxDamage = -450,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        minDamage = -400,
        maxDamage = -1000,
        length = 8,
        effect = CONST_ME_FIREATTACK,
    },
    {
        name = "fire",
        interval = 3000,
        chance = 100,
        minDamage = -80,
        maxDamage = -250,
        radius = 3,
        target = false,
        effect = CONST_ME_HITBYFIRE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        length = 5,
        spread = 3,
        target = false,
        speed = -800,
        duration = 30000,
        effect = CONST_ME_BLOCKHIT,
    },
}
monster.defenses = {
    defense = 50,
    armor = 50,
    {
        name = "invisible",
        interval = 2000,
        chance = 5,
        duration = 3000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = 5},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "outfit", condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Moooaaan!", yell = false},
}
monster.loot = {
    {
        id = "small ruby",
        chance = 15690,
        maxCount = 3,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 99,
    },
    {
        id = "bronze amulet",
        chance = 100000,
        maxCount = 7,
    },
    {
        id = "fire sword",
        chance = 3280,
    },
    {
        id = "fire axe",
        chance = 2150,
    },
    {
        id = "great mana potion",
        chance = 14800,
        maxCount = 2,
    },
    {
        id = "magma legs",
        chance = 790,
    },
    {
        id = "magma coat",
        chance = 790,
    },
    {
        id = "ultimate health potion",
        chance = 15440,
        maxCount = 2,
    },
    {
        id = "fiery heart",
        chance = 13570,
    },
    {
        id = 13757,
        chance = 1320,
    },
    {
        id = "wand of everblazing",
        chance = 1480,
    },
    {
        id = "violet crystal shard",
        chance = 5430,
    },
    {
        id = "brown crystal splinter",
        chance = 12100,
        maxCount = 2,
    },
    {
        id = "red crystal fragment",
        chance = 8880,
    },
    {
        id = "magma clump",
        chance = 13240,
    },
    {
        id = "blazing bone",
        chance = 14890,
    },
    {
        id = "eye of a weeper",
        chance = 15870,
    },
    {
        id = "prismatic bolt",
        chance = 10900,
        maxCount = 5,
    },
}

mtype:register(monster)
