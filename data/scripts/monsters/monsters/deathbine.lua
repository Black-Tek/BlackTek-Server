local mtype = Game.createMonsterType("Deathbine")
local monster = {}

monster.name = "Deathbine"
monster.description = "Deathbine"
monster.experience = 340
monster.race = "venom"
monster.maxHealth = 525
monster.health = 525
monster.speed = 240
monster.manaCost = 0
monster.corpse = 6047
monster.outfit = { lookType = 120 }
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -200,
        -- NOTE: melee condition (poison=5); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 5000,
        },
    },
    {
        name = "earth",
        interval = 1000,
        chance = 25,
        range = 7,
        minDamage = -60,
        maxDamage = -90,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 34,
        range = 7,
        speed = -850,
        duration = 30000,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "earth",
        interval = 1000,
        chance = 12,
        minDamage = -40,
        maxDamage = -130,
        radius = 3,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 10,
    armor = 26,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 35},
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "small emerald",
        chance = 100000,
        maxCount = 4,
    },
    {
        id = "platinum coin",
        chance = 100000,
        maxCount = 5,
    },
    {
        id = "dark mushroom",
        chance = 50000,
    },
    {
        id = "shadow herb",
        chance = 100000,
    },
    {
        id = "mandrake",
        chance = 5555,
    },
    {
        id = "seeds",
        chance = 50000,
    },
    {
        id = "terra boots",
        chance = 50000,
    },
    {
        id = "terra amulet",
        chance = 50000,
    },
    {
        id = "springsprout rod",
        chance = 50000,
    },
    {
        id = "carniphila seeds",
        chance = 100000,
    },
    {
        id = "sweet smelling bait",
        chance = 2854,
    },
}

mtype:register(monster)
