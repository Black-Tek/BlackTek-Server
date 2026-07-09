local mtype = Game.createMonsterType("Fluffy")
local monster = {}

monster.name = "Fluffy"
monster.description = "Fluffy"
monster.experience = 3550
monster.race = "blood"
monster.maxHealth = 4500
monster.health = 4500
monster.speed = 310
monster.manaCost = 0
monster.corpse = 6332
monster.outfit = { lookType = 240 }
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
        maxDamage = -648,
    },
    {
        name = "energy",
        interval = 1500,
        chance = 30,
        minDamage = -100,
        maxDamage = -200,
        length = 8,
        spread = 3,
        effect = CONST_ME_LOSEENERGY,
    },
    {
        name = "earth",
        interval = 1000,
        chance = 15,
        minDamage = -120,
        maxDamage = -300,
        length = 8,
        spread = 3,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "physical",
        interval = 1000,
        chance = 25,
        range = 7,
        minDamage = -105,
        maxDamage = -235,
        shootEffect = CONST_ANI_SUDDENDEATH,
    },
    {
        name = "earth",
        interval = 1000,
        chance = 20,
        range = 7,
        minDamage = -135,
        maxDamage = -255,
        radius = 6,
        target = false,
        effect = CONST_ME_LOSEENERGY,
    },
}
monster.defenses = {
    defense = 35,
    armor = 25,
}
monster.immunities = {
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Wooof!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 20,
    },
    {
        id = 2230,
        chance = 25000,
    },
    {
        id = "spike sword",
        chance = 2500,
    },
    {
        id = "knight axe",
        chance = 2857,
    },
    {
        id = 2671,
        chance = 50000,
        maxCount = 8,
    },
    {
        id = "soul orb",
        chance = 5555,
    },
    {
        id = "demonic essence",
        chance = 7200,
    },
    {
        id = 6558,
        chance = 8888,
    },
    {
        id = 6570,
        chance = 5538,
        maxCount = 4,
    },
    {
        id = 6571,
        chance = 1538,
    },
}

mtype:register(monster)
