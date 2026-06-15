local mtype = Game.createMonsterType("Adept of the Cult")
local monster = {}

monster.name = "Adept of the Cult"
monster.description = "an adept of the cult"
monster.experience = 400
monster.race = "blood"
monster.maxHealth = 430
monster.health = 430
monster.speed = 215
monster.manaCost = 0
monster.corpse = 20311
monster.outfit = {
    lookType = 194,
    lookHead = 95,
    lookBody = 94,
    lookLegs = 94,
    lookFeet = 19,
}
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
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -90,
        -- NOTE: melee condition (poison=2); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 2000,
        },
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -70,
        maxDamage = -150,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        range = 7,
        radius = 1,
        target = true,
        duration = 4000,
        shootEffect = CONST_ANI_HOLY,
        effect = CONST_ME_HOLYDAMAGE,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "healing",
        interval = 3000,
        chance = 20,
        minDamage = 45,
        maxDamage = 60,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 3000,
        effect = CONST_ME_YELLOW_RINGS,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 30},
    {type = COMBAT_EARTHDAMAGE, percent = 40},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Ghoul", interval = 2000, chance = 10, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Feel the power of the cult!", yell = false},
    {text = "Praise the voodoo!", yell = false},
    {text = "Power to the cult!", yell = false},
}
monster.loot = {
    {
        id = 1962,
        chance = 940,
    },
    {
        id = "small ruby",
        chance = 320,
    },
    {
        id = "gold coin",
        chance = 65520,
        maxCount = 60,
    },
    {
        id = 2169,
        chance = 420,
    },
    {
        id = "silver amulet",
        chance = 1020,
    },
    {
        id = "hailstorm rod",
        chance = 220,
    },
    {
        id = "clerical mace",
        chance = 1260,
    },
    {
        id = "red robe",
        chance = 80,
    },
    {
        id = "pirate voodoo doll",
        chance = 1730,
    },
    {
        id = 6089,
        chance = 700,
    },
    {
        id = "lunar staff",
        chance = 120,
    },
    {
        id = "amber staff",
        chance = 680,
    },
    {
        id = "cultish robe",
        chance = 10080,
    },
    {
        id = "cultish symbol",
        chance = 90,
    },
    {
        id = "rope belt",
        chance = 10000,
    },
    {
        id = "broken key ring",
        chance = 120,
    },
}

mtype:register(monster)
