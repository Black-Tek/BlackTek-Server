local mtype = Game.createMonsterType("Poacher")
local monster = {}

monster.name = "Poacher"
monster.description = "a poacher"
monster.experience = 70
monster.race = "blood"
monster.maxHealth = 90
monster.health = 90
monster.speed = 198
monster.manaCost = 530
monster.corpse = 20487
monster.outfit = {
    lookType = 129,
    lookHead = 60,
    lookBody = 118,
    lookLegs = 118,
    lookFeet = 116,
    lookAddons = 1,
}
monster.runHealth = 5
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
    illusionable = true,
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -35,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -35,
        shootEffect = CONST_ANI_ARROW,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "You will not live to tell anyone!", yell = false},
    {text = "You are my game today!", yell = false},
    {text = "Look what has stepped into my trap!", yell = false},
}
monster.loot = {
    {
        id = 2544,
        chance = 47154,
        maxCount = 18,
    },
    {
        id = 2461,
        chance = 30758,
    },
    {
        id = 2649,
        chance = 27912,
    },
    {
        id = 2456,
        chance = 14901,
    },
    {
        id = 2690,
        chance = 11049,
        maxCount = 2,
    },
    {
        id = 2050,
        chance = 5082,
    },
    {
        id = 2545,
        chance = 2521,
        maxCount = 3,
    },
    {
        id = 2578,
        chance = 1006,
    },
}

mtype:register(monster)
