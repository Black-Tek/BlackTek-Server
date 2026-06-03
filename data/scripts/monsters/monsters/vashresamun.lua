local mtype = Game.createMonsterType("Vashresamun")
local monster = {}

monster.name = "Vashresamun"
monster.description = "Vashresamun"
monster.experience = 2950
monster.race = "undead"
monster.maxHealth = 4000
monster.health = 4000
monster.speed = 340
monster.manaCost = 0
monster.corpse = 6025
monster.outfit = { lookType = 90 }
monster.changeTarget = {
    interval = 5000,
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
        maxDamage = -200,
        -- NOTE: melee condition (poison=65); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 65000,
        },
    },
    {
        name = "death",
        interval = 2000,
        chance = 30,
        minDamage = -200,
        maxDamage = -600,
        radius = 5,
        target = false,
        effect = CONST_ME_SOUND_PURPLE,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        range = 1,
        minDamage = 0,
        maxDamage = -800,
    },
}
monster.defenses = {
    defense = 40,
    armor = 40,
    {
        name = "healing",
        interval = 1000,
        chance = 20,
        minDamage = 250,
        maxDamage = 450,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 12,
        range = 7,
        speed = 370,
        duration = 30000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Banshee", interval = 2000, chance = 20, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "If music is the food of death, drop dead.", yell = false},
    {text = "Are you enjoying my music?", yell = false},
    {text = "Come my maidens, we have visitors!", yell = false},
    {text = "Chakka Chakka!", yell = false},
    {text = "Heheheheee!", yell = false},
}
monster.loot = {
    {
        id = "blue note",
        chance = 100000,
    },
    {
        id = "gold coin",
        chance = 31000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 31000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 31000,
        maxCount = 50,
    },
    {
        id = "white pearl",
        chance = 10000,
    },
    {
        id = 2072,
        chance = 9200,
    },
    {
        id = "great mana potion",
        chance = 8000,
    },
    {
        id = "blue robe",
        chance = 1200,
    },
    {
        id = "crystal mace",
        chance = 1000,
    },
    {
        id = 2124,
        chance = 1000,
    },
    {
        id = 2074,
        chance = 330,
    },
    {
        id = "ancient tiara",
        chance = 170,
    },
    {
        id = "mini mummy",
        chance = 170,
    },
}

mtype:register(monster)
