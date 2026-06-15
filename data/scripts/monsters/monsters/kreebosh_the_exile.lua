local mtype = Game.createMonsterType("Kreebosh the Exile")
local monster = {}

monster.name = "Kreebosh the Exile"
monster.description = "Kreebosh the Exile"
monster.experience = 350
monster.race = "blood"
monster.maxHealth = 705
monster.health = 705
monster.speed = 270
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 103 }
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -100,
    },
    {
        name = "fire",
        interval = 6000,
        chance = 80,
        minDamage = 0,
        maxDamage = -120,
        radius = 3,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "speed",
        interval = 3500,
        chance = 35,
        range = 5,
        radius = 1,
        target = true,
        speed = -450,
        duration = 20000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "physical",
        interval = 6000,
        chance = 40,
        range = 5,
        minDamage = -20,
        maxDamage = -100,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "energy",
        interval = 5000,
        chance = 20,
        range = 5,
        minDamage = -40,
        maxDamage = -200,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "drunk",
        interval = 1000,
        chance = 20,
        range = 5,
        radius = 1,
        target = true,
        duration = 30000,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 50,
        range = 5,
        radius = 1,
        target = true,
        duration = 60000,
        effect = CONST_ME_MAGIC_GREEN,
    },
}
monster.defenses = {
    defense = 40,
    armor = 30,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 55},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Green Djinn", interval = 5000, chance = 20, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 5,
    {text = "I bet you wish you weren't here.", yell = false},
}

mtype:register(monster)
