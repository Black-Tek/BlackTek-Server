local mtype = Game.createMonsterType("Mahrdis")
local monster = {}

monster.name = "Mahrdis"
monster.description = "Mahrdis"
monster.experience = 3050
monster.race = "undead"
monster.maxHealth = 3900
monster.health = 3900
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
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -400,
        -- NOTE: melee condition (poison=65); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 65000,
        },
    },
    {
        name = "physical",
        interval = 1600,
        chance = 7,
        range = 1,
        minDamage = -60,
        maxDamage = -600,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 7,
        range = 7,
        minDamage = -60,
        maxDamage = -600,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 13,
        range = 7,
        speed = -850,
        duration = 50000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 34,
        minDamage = -80,
        maxDamage = -800,
        radius = 3,
        target = false,
        effect = CONST_ME_EXPLOSIONAREA,
    },
    {
        name = "firefield",
        interval = 1000,
        chance = 12,
        radius = 4,
        target = false,
        effect = CONST_ME_BLOCKHIT,
    },
    {
        name = "firecondition",
        interval = 2000,
        chance = 13,
        minDamage = -50,
        maxDamage = -500,
        length = 8,
        spread = 3,
        effect = CONST_ME_EXPLOSIONHIT,
    },
}
monster.defenses = {
    defense = 42,
    armor = 42,
    {
        name = "healing",
        interval = 2000,
        chance = 40,
        minDamage = 150,
        maxDamage = 600,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = -20},
    {type = COMBAT_ICEDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 4
monster.summons = {
    {name = "Fire Elemental", interval = 2000, chance = 30, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Ashes to ashes!", yell = false},
    {text = "Fire, Fire!", yell = false},
    {text = "The eternal flame demands its due!", yell = false},
    {text = "Burnnnnnnnnn!", yell = false},
    {text = "May my flames engulf you!", yell = false},
    {text = "This is why I'm hot.", yell = false},
}
monster.loot = {
    {
        id = 2353,
        chance = 100000,
    },
    {
        id = 2148,
        chance = 88738,
        maxCount = 230,
    },
    {
        id = 2147,
        chance = 12255,
        maxCount = 3,
    },
    {
        id = 7591,
        chance = 10213,
    },
    {
        id = 2168,
        chance = 6752,
    },
    {
        id = 2432,
        chance = 936,
    },
    {
        id = 2156,
        chance = 624,
    },
    {
        id = 2141,
        chance = 142,
    },
    {
        id = 11207,
        chance = 113,
    },
    {
        id = 2539,
        chance = 113,
    },
}

mtype:register(monster)
