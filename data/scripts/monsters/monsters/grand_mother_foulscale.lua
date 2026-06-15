local mtype = Game.createMonsterType("Grand Mother Foulscale")
local monster = {}

monster.name = "Grand Mother Foulscale"
monster.description = "Grand Mother Foulscale"
monster.experience = 1400
monster.race = "blood"
monster.maxHealth = 1850
monster.health = 1850
monster.speed = 180
monster.manaCost = 0
monster.corpse = 5973
monster.outfit = { lookType = 34 }
monster.runHealth = 400
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -170,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -45,
        maxDamage = -85,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 8,
        minDamage = -90,
        maxDamage = -150,
        length = 8,
        spread = 3,
        effect = CONST_ME_FIREAREA,
    },
}
monster.defenses = {
    defense = 20,
    armor = 27,
    {
        name = "healing",
        interval = 1000,
        chance = 17,
        minDamage = 34,
        maxDamage = 66,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 80},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 4
monster.summons = {
    {name = "dragon hatchling", interval = 4000, chance = 40, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "GROOAAARRR", yell = true},
    {text = "FCHHHHH", yell = true},
}
monster.loot = {
    {
        id = 5877,
        chance = 100000,
    },
    {
        id = 5920,
        chance = 100000,
    },
    {
        id = 2148,
        chance = 86486,
        maxCount = 97,
    },
    {
        id = 2672,
        chance = 32432,
    },
    {
        id = 2387,
        chance = 16216,
    },
    {
        id = 2647,
        chance = 16216,
    },
    {
        id = 2434,
        chance = 13514,
    },
    {
        id = 2455,
        chance = 8108,
    },
    {
        id = 2546,
        chance = 5405,
        maxCount = 7,
    },
    {
        id = 2516,
        chance = 5405,
    },
    {
        id = 3351,
        chance = 5405,
    },
    {
        id = 2413,
        chance = 2703,
    },
    {
        id = 7430,
        chance = 2703,
    },
}

mtype:register(monster)
