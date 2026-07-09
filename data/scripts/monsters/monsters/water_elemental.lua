local mtype = Game.createMonsterType("Water Elemental")
local monster = {}

monster.name = "Water Elemental"
monster.description = "a water elemental"
monster.experience = 650
monster.race = "undead"
monster.maxHealth = 550
monster.health = 550
monster.speed = 220
monster.manaCost = 0
monster.corpse = 10499
monster.outfit = { lookType = 286 }
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
        maxDamage = -160,
        -- NOTE: melee condition (poison=60); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 60000,
        },
    },
    {
        name = "drown",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -125,
        maxDamage = -235,
        radius = 2,
        target = true,
        effect = CONST_ME_LOSEENERGY,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -88,
        maxDamage = -150,
        shootEffect = CONST_ANI_SMALLICE,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 15,
        minDamage = -225,
        maxDamage = -260,
        radius = 5,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "healing",
        interval = 2000,
        chance = 5,
        minDamage = 50,
        maxDamage = 80,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 300,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 35},
    {type = COMBAT_HOLYDAMAGE, percent = 50},
    {type = COMBAT_DEATHDAMAGE, percent = 50},
    {type = COMBAT_ENERGYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
}
monster.loot = {
    {
        id = "small diamond",
        chance = 1000,
    },
    {
        id = "small sapphire",
        chance = 1000,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "small emerald",
        chance = 1000,
        maxCount = 2,
    },
    {
        id = "platinum coin",
        chance = 10000,
    },
    {
        id = "energy ring",
        chance = 950,
    },
    {
        id = "life ring",
        chance = 930,
    },
    {
        id = 2667,
        chance = 20000,
    },
    {
        id = "rainbow trout",
        chance = 940,
    },
    {
        id = "green perch",
        chance = 1050,
    },
    {
        id = "strong health potion",
        chance = 10000,
    },
    {
        id = "strong mana potion",
        chance = 10000,
    },
    {
        id = 7632,
        chance = 800,
    },
    {
        id = 7633,
        chance = 800,
    },
    {
        id = "leviathan's amulet",
        chance = 260,
    },
}

mtype:register(monster)
