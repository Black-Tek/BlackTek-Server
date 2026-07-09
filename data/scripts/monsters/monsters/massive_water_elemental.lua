local mtype = Game.createMonsterType("Massive Water Elemental")
local monster = {}

monster.name = "Massive Water Elemental"
monster.description = "a massive water elemental"
monster.experience = 1100
monster.race = "undead"
monster.maxHealth = 1250
monster.health = 1250
monster.speed = 280
monster.manaCost = 0
monster.corpse = 10499
monster.outfit = { lookType = 11 }
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
        maxDamage = -220,
        -- NOTE: melee condition (poison=300); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 300000,
        },
    },
    {
        name = "drown",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -330,
        maxDamage = -450,
        radius = 2,
        target = true,
        effect = CONST_ME_LOSEENERGY,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -170,
        maxDamage = -210,
        shootEffect = CONST_ANI_SMALLICE,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 20,
        minDamage = -355,
        maxDamage = -420,
        radius = 5,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "healing",
        interval = 2000,
        chance = 5,
        minDamage = 120,
        maxDamage = 250,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 30},
    {type = COMBAT_HOLYDAMAGE, percent = 30},
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
        chance = 1900,
        maxCount = 2,
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
        chance = 20000,
        maxCount = 2,
    },
    {
        id = "energy ring",
        chance = 910,
    },
    {
        id = "life ring",
        chance = 1000,
    },
    {
        id = 2667,
        chance = 40000,
        maxCount = 2,
    },
    {
        id = "rainbow trout",
        chance = 1340,
    },
    {
        id = "green perch",
        chance = 1590,
    },
    {
        id = "great mana potion",
        chance = 10400,
    },
    {
        id = "great health potion",
        chance = 10000,
    },
}

mtype:register(monster)
