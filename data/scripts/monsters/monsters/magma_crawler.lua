local mtype = Game.createMonsterType("Magma Crawler")
local monster = {}

monster.name = "Magma Crawler"
monster.description = "a magma crawler"
monster.experience = 2700
monster.race = "fire"
monster.maxHealth = 4800
monster.health = 4800
monster.speed = 220
monster.manaCost = 0
monster.corpse = 17336
monster.outfit = { lookType = 492 }
monster.runHealth = 300
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
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
        maxDamage = -203,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        minDamage = -300,
        maxDamage = -1100,
        length = 8,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        minDamage = -290,
        maxDamage = -800,
        length = 3,
        spread = 2,
        effect = CONST_ME_FIREATTACK,
    },
    {
        name = "magma crawler soulfire",
        interval = 2000,
        chance = 20,
    },
    {
        name = "monster soulfire",
        interval = 2000,
        chance = 10,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        minDamage = -140,
        maxDamage = -180,
        radius = 3,
        target = false,
        effect = CONST_ME_HITBYFIRE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        radius = 2,
        target = false,
        speed = -800,
        duration = 20000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 45,
    armor = 45,
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 5000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = 5},
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Crrroak!", yell = false},
}
monster.loot = {
    {
        id = "small diamond",
        chance = 8800,
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
        maxCount = 100,
    },
    {
        id = "platinum coin",
        chance = 95000,
        maxCount = 5,
    },
    {
        id = "yellow gem",
        chance = 1030,
    },
    {
        id = "energy ring",
        chance = 1650,
    },
    {
        id = "fire sword",
        chance = 1680,
    },
    {
        id = "black shield",
        chance = 1550,
    },
    {
        id = "iron ore",
        chance = 4280,
    },
    {
        id = "white piece of cloth",
        chance = 2310,
    },
    {
        id = "red piece of cloth",
        chance = 930,
    },
    {
        id = "yellow piece of cloth",
        chance = 2980,
    },
    {
        id = "great mana potion",
        chance = 6500,
    },
    {
        id = "great health potion",
        chance = 7270,
    },
    {
        id = "magma amulet",
        chance = 3120,
    },
    {
        id = "magma boots",
        chance = 1820,
    },
    {
        id = "wand of draconia",
        chance = 4280,
    },
    {
        id = "fiery heart",
        chance = 7810,
    },
    {
        id = 13757,
        chance = 1675,
    },
    {
        id = "crystalline arrow",
        chance = 5950,
        maxCount = 10,
    },
    {
        id = "wand of everblazing",
        chance = 690,
    },
    {
        id = "blue crystal shard",
        chance = 3930,
        maxCount = 2,
    },
    {
        id = "brown crystal splinter",
        chance = 8500,
        maxCount = 2,
    },
    {
        id = "blue crystal splinter",
        chance = 8500,
        maxCount = 2,
    },
    {
        id = "green crystal fragment",
        chance = 7000,
    },
    {
        id = "magma clump",
        chance = 11600,
    },
    {
        id = "blazing bone",
        chance = 11500,
    },
}

mtype:register(monster)
