local mtype = Game.createMonsterType("Sparkion")
local monster = {}

monster.name = "Sparkion"
monster.description = "a sparkion"
monster.experience = 1520
monster.race = "energy"
monster.maxHealth = 2700
monster.health = 2700
monster.speed = 302
monster.manaCost = 0
monster.corpse = 26044
monster.outfit = { lookType = 877 }
monster.changeTarget = {
    interval = 5000,
    chance = 5,
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -300,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 20,
        minDamage = -350,
        maxDamage = -700,
        length = 6,
        effect = CONST_ME_GROUNDSHAKER,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 25,
        range = 5,
        minDamage = -150,
        maxDamage = -250,
        target = true,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "energycondition",
        interval = 2000,
        chance = 20,
        range = 6,
        minDamage = -300,
        maxDamage = -600,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_BLOCKHIT,
    },
}
monster.defenses = {
    defense = 40,
    armor = 40,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = -15},
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_HOLYDAMAGE, percent = 5},
    {type = COMBAT_DEATHDAMAGE, percent = 5},
    {type = COMBAT_FIREDAMAGE, percent = 15},
    {type = COMBAT_ICEDAMAGE, percent = 70},
    {type = COMBAT_ICEDAMAGE, percent = 90},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Zzing!", yell = false},
    {text = "Frizzle!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 100,
    },
    {
        id = "platinum coin",
        chance = 90000,
        maxCount = 3,
    },
    {
        id = "sparkion claw",
        chance = 16000,
    },
    {
        id = "energy drink",
        chance = 15000,
    },
    {
        id = "energy bar",
        chance = 15000,
    },
    {
        id = "sparkion stings",
        chance = 13000,
    },
    {
        id = "sparkion legs",
        chance = 11000,
    },
    {
        id = "sparkion tail",
        chance = 9700,
    },
    {
        id = "great mana potion",
        chance = 9500,
        maxCount = 2,
    },
    {
        id = "great spirit potion",
        chance = 9500,
        maxCount = 2,
    },
    {
        id = "great health potion",
        chance = 9500,
        maxCount = 2,
    },
    {
        id = "blue crystal splinter",
        chance = 7700,
    },
    {
        id = "cyan crystal fragment",
        chance = 5300,
    },
    {
        id = "small sapphire",
        chance = 4400,
        maxCount = 2,
    },
    {
        id = "blue crystal shard",
        chance = 4000,
    },
    {
        id = "blue gem",
        chance = 900,
    },
    {
        id = "wand of cosmic energy",
        chance = 900,
    },
    {
        id = 26187,
        chance = 400,
    },
    {
        id = 26189,
        chance = 400,
    },
    {
        id = 26185,
        chance = 400,
    },
    {
        id = 26199,
        chance = 250,
    },
    {
        id = 26198,
        chance = 250,
    },
    {
        id = 26200,
        chance = 250,
    },
}

mtype:register(monster)
