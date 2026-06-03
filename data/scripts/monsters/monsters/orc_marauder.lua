local mtype = Game.createMonsterType("Orc Marauder")
local monster = {}

monster.name = "Orc Marauder"
monster.description = "an orc marauder"
monster.experience = 205
monster.race = "blood"
monster.maxHealth = 235
monster.health = 235
monster.speed = 210
monster.manaCost = 0
monster.corpse = 11251
monster.outfit = { lookType = 342 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = true,
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
        maxDamage = -100,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 50,
        range = 7,
        minDamage = 0,
        maxDamage = -100,
        shootEffect = CONST_ANI_ONYXARROW,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 350,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 5},
    {type = COMBAT_EARTHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Grrrrrr", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 55000,
        maxCount = 90,
    },
    {
        id = "obsidian lance",
        chance = 1110,
    },
    {
        id = "orcish axe",
        chance = 1320,
    },
    {
        id = "crossbow",
        chance = 440,
    },
    {
        id = "bow",
        chance = 5210,
    },
    {
        id = "meat",
        chance = 24600,
    },
    {
        id = "silkweaver bow",
        chance = 70,
    },
    {
        id = "orc tooth",
        chance = 3890,
    },
    {
        id = "shaggy tail",
        chance = 10090,
    },
    {
        id = "broken crossbow",
        chance = 4830,
    },
    {
        id = "orc leather",
        chance = 3800,
    },
}

mtype:register(monster)
