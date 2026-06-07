local mtype = Game.createMonsterType("Demon (Goblin)")
local monster = {}

monster.name = "Demon (Goblin)"
monster.description = "a demon goblin"
monster.experience = 25
monster.race = "blood"
monster.maxHealth = 50
monster.health = 50
monster.speed = 120
monster.manaCost = 0
monster.corpse = 6002
monster.outfit = { lookType = 35 }
monster.runHealth = 35
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
    canPushItems = false,
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
        maxDamage = -10,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -25,
        shootEffect = CONST_ANI_SMALLSTONE,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = 0,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = 0,
        radius = 7,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = 0,
        maxDamage = 0,
        length = 8,
        effect = CONST_ME_PURPLEENERGY,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 10,
        range = 1,
        minDamage = 0,
        maxDamage = 0,
        shootEffect = CONST_ANI_ENERGY,
    },
}
monster.defenses = {
    defense = 6,
    armor = 6,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50320,
        maxCount = 9,
    },
    {
        id = "small stone",
        chance = 15290,
    },
    {
        id = 2667,
        chance = 12750,
    },
    {
        id = "small axe",
        chance = 9700,
    },
    {
        id = "short sword",
        chance = 8870,
    },
    {
        id = "bone club",
        chance = 4900,
    },
    {
        id = "leather armor",
        chance = 2510,
    },
    {
        id = "leather helmet",
        chance = 1940,
    },
    {
        id = "dagger",
        chance = 1800,
    },
    {
        id = 2230,
        chance = 1130,
    },
    {
        id = "mouldy cheese",
        chance = 1000,
    },
    {
        id = "goblin ear",
        chance = 940,
    },
}

mtype:register(monster)
