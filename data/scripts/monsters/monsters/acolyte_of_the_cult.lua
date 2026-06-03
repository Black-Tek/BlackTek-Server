local mtype = Game.createMonsterType("Acolyte of the Cult")
local monster = {}

monster.name = "Acolyte of the Cult"
monster.description = "an acolyte of the cult"
monster.experience = 300
monster.race = "blood"
monster.maxHealth = 390
monster.health = 390
monster.speed = 200
monster.manaCost = 0
monster.corpse = 20319
monster.outfit = {
    lookType = 194,
    lookHead = 95,
    lookBody = 100,
    lookLegs = 100,
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
        maxDamage = -100,
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
        minDamage = -60,
        maxDamage = -120,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 5,
        range = 7,
        radius = 1,
        target = true,
        duration = 3000,
        shootEffect = CONST_ANI_HOLY,
        effect = CONST_ME_HOLYDAMAGE,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 40,
        maxDamage = 60,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.maxSummons = 1
monster.summons = {
    {name = "Skeleton", interval = 2000, chance = 10, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Praise the voodoo!", yell = false},
    {text = "Power to the cult!", yell = false},
    {text = "Feel the power of the cult!", yell = false},
}
monster.loot = {
    {
        id = 1962,
        chance = 730,
    },
    {
        id = "gold coin",
        chance = 66940,
        maxCount = 40,
    },
    {
        id = "small emerald",
        chance = 550,
    },
    {
        id = "life ring",
        chance = 560,
    },
    {
        id = "terra rod",
        chance = 250,
    },
    {
        id = "dragon necklace",
        chance = 1050,
    },
    {
        id = "morning star",
        chance = 4990,
    },
    {
        id = "pirate voodoo doll",
        chance = 1060,
    },
    {
        id = 6088,
        chance = 480,
    },
    {
        id = "cultish robe",
        chance = 8070,
    },
    {
        id = "cultish symbol",
        chance = 40,
    },
    {
        id = "rope belt",
        chance = 10420,
    },
    {
        id = "broken key ring",
        chance = 60,
    },
}

mtype:register(monster)
