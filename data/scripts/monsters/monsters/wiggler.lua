local mtype = Game.createMonsterType("Wiggler")
local monster = {}

monster.name = "Wiggler"
monster.experience = 900
monster.race = "venom"
monster.maxHealth = 1200
monster.health = 1200
monster.speed = 220
monster.manaCost = 0
monster.corpse = 18483
monster.outfit = { lookType = 510 }
monster.runHealth = 359
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -200,
        -- NOTE: melee condition (poison=500); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 500000,
        },
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        minDamage = -180,
        maxDamage = -270,
        length = 4,
        spread = 3,
        target = false,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -160,
        maxDamage = -200,
        shootEffect = CONST_ANI_POISONARROW,
        effect = CONST_ME_SMALLPLANTS,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        length = 3,
        spread = 2,
        target = false,
        speed = -700,
        duration = 30000,
        effect = CONST_ME_HITAREA,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 510,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 5},
    {type = COMBAT_FIREDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Zzzrp!", yell = false},
    {text = "Crick! Crick!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 100,
    },
    {
        id = "platinum coin",
        chance = 14790,
    },
    {
        id = "terra rod",
        chance = 850,
    },
    {
        id = "serpent sword",
        chance = 1360,
    },
    {
        id = "black shield",
        chance = 1320,
    },
    {
        id = "white mushroom",
        chance = 7142,
        maxCount = 5,
    },
    {
        id = "blue piece of cloth",
        chance = 2080,
    },
    {
        id = "yellow piece of cloth",
        chance = 2160,
    },
    {
        id = "strong health potion",
        chance = 4930,
    },
    {
        id = "strong mana potion",
        chance = 5250,
    },
    {
        id = "crystalline arrow",
        chance = 15540,
        maxCount = 5,
    },
    {
        id = "green crystal splinter",
        chance = 3600,
    },
    {
        id = "green crystal fragment",
        chance = 2070,
    },
    {
        id = "drill bolt",
        chance = 15260,
        maxCount = 5,
    },
}

mtype:register(monster)
