local mtype = Game.createMonsterType("Wailing Widow")
local monster = {}

monster.name = "Wailing Widow"
monster.description = "a wailing widow"
monster.experience = 450
monster.race = "venom"
monster.maxHealth = 850
monster.health = 850
monster.speed = 254
monster.manaCost = 0
monster.corpse = 11310
monster.outfit = { lookType = 347 }
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
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -120,
        -- NOTE: melee condition (poison=160); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 160000,
        },
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 20,
        range = 7,
        radius = 4,
        target = false,
        duration = 4000,
        effect = CONST_ME_SOUND_RED,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -40,
        maxDamage = -70,
        radius = 3,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -60,
        maxDamage = -110,
        shootEffect = CONST_ANI_POISON,
    },
}
monster.defenses = {
    defense = 31,
    armor = 31,
    {
        name = "healing",
        interval = 2000,
        chance = 5,
        minDamage = 70,
        maxDamage = 100,
        effect = CONST_ME_SOUND_WHITE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 820,
        duration = 5000,
        effect = CONST_ME_SOUND_YELLOW,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_HOLYDAMAGE, percent = 5},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "drunk", condition = true},
}
monster.loot = {
    {
        id = 2148,
        chance = 96855,
        maxCount = 140,
    },
    {
        id = 11328,
        chance = 19966,
    },
    {
        id = 7620,
        chance = 5251,
    },
    {
        id = 7618,
        chance = 4942,
    },
    {
        id = 2381,
        chance = 4708,
    },
    {
        id = 2510,
        chance = 3173,
    },
    {
        id = 2796,
        chance = 2986,
    },
    {
        id = 11323,
        chance = 2256,
    },
    {
        id = 11329,
        chance = 683,
    },
}

mtype:register(monster)
