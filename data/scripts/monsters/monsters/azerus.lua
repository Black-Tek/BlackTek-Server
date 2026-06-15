local mtype = Game.createMonsterType("Azerus")
local monster = {}

monster.name = "Azerus"
monster.description = "Azerus"
monster.experience = 6000
monster.race = "blood"
monster.maxHealth = 26000
monster.health = 26000
monster.speed = 320
monster.manaCost = 0
monster.corpse = 9738
monster.outfit = {
    lookType = 309,
    lookHead = 19,
    lookBody = 96,
    lookLegs = 21,
    lookFeet = 81,
}
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 1
monster.staticAttackChance = 85
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
        maxDamage = -900,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -2000,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_POFF,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -800,
        radius = 7,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_BIGCLOUDS,
    },
    {
        name = "death",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -524,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "lifedrain",
        interval = 1000,
        chance = 10,
        minDamage = -100,
        maxDamage = -500,
        length = 8,
        effect = CONST_ME_SLEEP,
    },
}
monster.defenses = {
    defense = 65,
    armor = 40,
    {
        name = "healing",
        interval = 1000,
        chance = 11,
        minDamage = 400,
        maxDamage = 900,
        effect = CONST_ME_MAGIC_GREEN,
    },
}
monster.maxSummons = 10
monster.summons = {
    {name = "Rift Worm", interval = 2000, chance = 30, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 12,
    {text = "The ultimate will finally consume this unworthy existence!", yell = false},
    {text = "My masters and I will tear down barriers and join the ultimate in its realm!", yell = false},
    {text = "The power of the Yalahari will all be mine!", yell = false},
    {text = "He who has returned from beyond has taught me secrets you can't even grasp!", yell = false},
    {text = "You can't hope to penetrate my shields!", yell = false},
    {text = "Do you really think you could beat me?", yell = false},
}

mtype:register(monster)
