local mtype = Game.createMonsterType("Emerald Damselfly")
local monster = {}

monster.name = "Emerald Damselfly"
monster.description = "an emerald damselfly"
monster.experience = 35
monster.race = "venom"
monster.maxHealth = 90
monster.health = 90
monster.speed = 200
monster.manaCost = 0
monster.corpse = 19706
monster.outfit = { lookType = 528 }
monster.changeTarget = {
    interval = 5000,
    chance = 0,
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
        maxDamage = -4,
        -- NOTE: melee condition (poison=10); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 10000,
        },
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -12,
        shootEffect = CONST_ANI_POISON,
    },
}
monster.defenses = {
    defense = 6,
    armor = 6,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 4,
        maxDamage = 10,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Bzzzzz!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 90901,
        maxCount = 18,
    },
    {
        id = 19738,
        chance = 11824,
    },
    {
        id = 19743,
        chance = 9986,
    },
    {
        id = 2544,
        chance = 7541,
        maxCount = 5,
    },
    {
        id = 2120,
        chance = 5031,
    },
    {
        id = 7618,
        chance = 3588,
    },
    {
        id = 7620,
        chance = 3560,
    },
}

mtype:register(monster)
