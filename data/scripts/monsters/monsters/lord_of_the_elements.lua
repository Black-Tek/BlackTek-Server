local mtype = Game.createMonsterType("Lord of the Elements")
local monster = {}

monster.name = "Lord of the Elements"
monster.description = "Lord of the Elements"
monster.experience = 8000
monster.race = "undead"
monster.maxHealth = 8000
monster.health = 8000
monster.speed = 370
monster.manaCost = 0
monster.corpse = 9009
monster.outfit = { lookType = 290 }
monster.changeTarget = {
    interval = 5000,
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
    canWalkOnFire = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -690,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 1000,
        chance = 25,
        minDamage = 100,
        maxDamage = 195,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "outfit",
        interval = 1500,
        chance = 40,
        duration = 3000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "outfit",
        interval = 1500,
        chance = 40,
        duration = 3000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "outfit",
        interval = 1500,
        chance = 40,
        duration = 3000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "outfit",
        interval = 1500,
        chance = 40,
        duration = 3000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 1},
    {type = COMBAT_FIREDAMAGE, percent = 30},
    {type = COMBAT_ENERGYDAMAGE, percent = 30},
    {type = COMBAT_ICEDAMAGE, percent = 30},
    {type = COMBAT_EARTHDAMAGE, percent = 45},
}
monster.immunities = {
    {type = "holy", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Blistering Fire Elemental", interval = 4000, chance = 50, max = 0},
    {name = "Jagged Earth Elemental", interval = 4000, chance = 50, max = 0},
    {name = "Roaring Water Elemental", interval = 4000, chance = 50, max = 0},
    {name = "Overcharged Energy Elemental", interval = 4000, chance = 50, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 30,
    {text = "WHO DARES CALLING ME?", yell = true},
    {text = "I'LL FREEZE YOU THEN I CRUSH YOU!", yell = true},
}
monster.loot = {
    {
        id = "small sapphire",
        chance = 7142,
        maxCount = 4,
    },
    {
        id = "small ruby",
        chance = 11111,
        maxCount = 4,
    },
    {
        id = "small emerald",
        chance = 11111,
        maxCount = 4,
    },
    {
        id = "small amethyst",
        chance = 11111,
        maxCount = 3,
    },
    {
        id = "platinum coin",
        chance = 50000,
        maxCount = 9,
    },
    {
        id = "earthborn titan armor",
        chance = 2063,
    },
    {
        id = "gold ingot",
        chance = 25000,
    },
}

mtype:register(monster)
