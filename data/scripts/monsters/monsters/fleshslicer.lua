local mtype = Game.createMonsterType("Fleshslicer")
local monster = {}

monster.name = "Fleshslicer"
monster.description = "Fleshslicer"
monster.experience = 5500
monster.race = "venom"
monster.maxHealth = 5700
monster.health = 5700
monster.speed = 360
monster.manaCost = 0
monster.corpse = 15296
monster.outfit = { lookType = 457 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
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
        maxDamage = -300,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        minDamage = -100,
        maxDamage = -450,
        length = 5,
        spread = 3,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -200,
        maxDamage = -500,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 450,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = 15486,
        chance = 100000,
        maxCount = 2,
    },
    {
        id = 2148,
        chance = 100000,
        maxCount = 185,
    },
    {
        id = 2152,
        chance = 100000,
        maxCount = 5,
    },
    {
        id = 15485,
        chance = 100000,
    },
    {
        id = 7590,
        chance = 82353,
        maxCount = 2,
    },
    {
        id = 16096,
        chance = 76471,
        maxCount = 2,
    },
    {
        id = 2147,
        chance = 35294,
        maxCount = 5,
    },
    {
        id = 3964,
        chance = 29412,
    },
    {
        id = 2143,
        chance = 29412,
        maxCount = 5,
    },
    {
        id = 2156,
        chance = 23529,
    },
    {
        id = 8473,
        chance = 17647,
    },
    {
        id = 7632,
        chance = 11765,
    },
    {
        id = 7413,
        chance = 11765,
    },
    {
        id = 6300,
        chance = 5882,
    },
}

mtype:register(monster)
