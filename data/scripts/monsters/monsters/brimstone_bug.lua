local mtype = Game.createMonsterType("Brimstone Bug")
local monster = {}

monster.name = "Brimstone Bug"
monster.experience = 900
monster.race = "venom"
monster.maxHealth = 1300
monster.health = 1300
monster.speed = 200
monster.manaCost = 0
monster.corpse = 12527
monster.outfit = { lookType = 352 }
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
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -213,
        -- NOTE: melee condition (poison=400); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 400000,
        },
    },
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        range = 7,
        speed = -600,
        duration = 10000,
        shootEffect = CONST_ANI_DEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 5,
        minDamage = -140,
        maxDamage = -310,
        radius = 6,
        target = false,
        effect = CONST_ME_SMALLPLANTS,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        minDamage = -130,
        maxDamage = -200,
        length = 6,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        minDamage = -80,
        maxDamage = -120,
        length = 8,
        spread = 3,
        effect = CONST_ME_YELLOW_RINGS,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Chrrr!", yell = false},
}
monster.loot = {
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
        id = "small emerald",
        chance = 2702,
        maxCount = 4,
    },
    {
        id = "stealth ring",
        chance = 892,
    },
    {
        id = "platinum amulet",
        chance = 110,
    },
    {
        id = "magic sulphur",
        chance = 1639,
    },
    {
        id = "strong health potion",
        chance = 9003,
    },
    {
        id = "strong mana potion",
        chance = 9025,
    },
    {
        id = "poisonous slime",
        chance = 50000,
    },
    {
        id = "lump of earth",
        chance = 20000,
    },
    {
        id = "sulphurous stone",
        chance = 14970,
    },
    {
        id = "brimstone fangs",
        chance = 5710,
    },
    {
        id = "brimstone shell",
        chance = 10000,
    },
}

mtype:register(monster)
