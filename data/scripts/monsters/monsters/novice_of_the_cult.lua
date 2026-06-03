local mtype = Game.createMonsterType("Novice of the Cult")
local monster = {}

monster.name = "Novice of the Cult"
monster.description = "a novice of the cult"
monster.experience = 100
monster.race = "blood"
monster.maxHealth = 285
monster.health = 285
monster.speed = 200
monster.manaCost = 0
monster.corpse = 20467
monster.outfit = {
    lookType = 133,
    lookHead = 114,
    lookBody = 114,
    lookLegs = 76,
    lookFeet = 114,
}
monster.runHealth = 40
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
    illusionable = true,
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
        maxDamage = -65,
        -- NOTE: melee condition (poison=1); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 1000,
        },
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -20,
        maxDamage = -80,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 20,
        maxDamage = 40,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -8},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = -8},
}
monster.maxSummons = 1
monster.summons = {
    {name = "Chicken", interval = 2000, chance = 10, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Fear us!", yell = false},
    {text = "You will not tell anyone what you have seen!", yell = false},
    {text = "Your curiosity will be punished!", yell = false},
}
monster.loot = {
    {
        id = 1962,
        chance = 700,
    },
    {
        id = "small diamond",
        chance = 210,
    },
    {
        id = "gold coin",
        chance = 43380,
        maxCount = 40,
    },
    {
        id = "wand of vortex",
        chance = 450,
    },
    {
        id = "garlic necklace",
        chance = 420,
    },
    {
        id = "dwarven ring",
        chance = 500,
    },
    {
        id = "scarf",
        chance = 2900,
    },
    {
        id = "pirate voodoo doll",
        chance = 520,
    },
    {
        id = 6087,
        chance = 970,
    },
    {
        id = "cultish robe",
        chance = 1030,
    },
    {
        id = "rope belt",
        chance = 5910,
    },
}

mtype:register(monster)
