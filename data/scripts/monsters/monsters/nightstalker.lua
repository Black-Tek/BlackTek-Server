local mtype = Game.createMonsterType("Nightstalker")
local monster = {}

monster.name = "Nightstalker"
monster.description = "a nightstalker"
monster.experience = 500
monster.race = "undead"
monster.maxHealth = 700
monster.health = 700
monster.speed = 260
monster.manaCost = 0
monster.corpse = 9915
monster.outfit = { lookType = 320 }
monster.changeTarget = {
    interval = 4000,
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
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -90,
        -- NOTE: melee condition (poison=80); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 80000,
        },
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -60,
        maxDamage = -170,
        target = true,
        effect = CONST_ME_HOLYAREA,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        target = true,
        speed = -600,
        duration = 15000,
        effect = CONST_ME_SLEEP,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 240,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 4000,
        effect = CONST_ME_YELLOW_RINGS,
    },
    {
        name = "outfit",
        interval = 5000,
        chance = 10,
        duration = 4000,
    },
    {
        name = "outfit",
        interval = 5000,
        chance = 10,
        duration = 4000,
    },
    {
        name = "outfit",
        interval = 5000,
        chance = 10,
        duration = 4000,
    },
    {
        name = "outfit",
        interval = 5000,
        chance = 10,
        duration = 4000,
    },
    {
        name = "outfit",
        interval = 5000,
        chance = 10,
        duration = 4000,
    },
    {
        name = "outfit",
        interval = 5000,
        chance = 1,
        duration = 4000,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "The sunlight is so depressing.", yell = false},
    {text = "Come with me, my child.", yell = false},
    {text = "I've been in the shadow under your bed last night.", yell = false},
    {text = "You never know what hides in the night.", yell = false},
    {text = "I remember your face - and I know where you sleep.", yell = false},
    {text = "Only the sweetest and cruelest dreams for you, my love.", yell = false},
}
monster.loot = {
    {
        id = 2124,
        chance = 1030,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 10,
    },
    {
        id = "platinum amulet",
        chance = 121,
    },
    {
        id = "boots of haste",
        chance = 121,
    },
    {
        id = "protection amulet",
        chance = 847,
    },
    {
        id = "shadow herb",
        chance = 4761,
    },
    {
        id = "haunted blade",
        chance = 318,
    },
    {
        id = "chaos mace",
        chance = 121,
    },
    {
        id = "strong mana potion",
        chance = 1612,
    },
    {
        id = "spirit cloak",
        chance = 520,
    },
    {
        id = "crystal of balance",
        chance = 127,
    },
}

mtype:register(monster)
