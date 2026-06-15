local mtype = Game.createMonsterType("Midnight Panther")
local monster = {}

monster.name = "Midnight Panther"
monster.description = "a midnight panther"
monster.experience = 900
monster.race = "blood"
monster.maxHealth = 1200
monster.health = 1200
monster.speed = 250
monster.manaCost = 0
monster.corpse = 13327
monster.outfit = { lookType = 385 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -90,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -75,
        maxDamage = -215,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_ENERGYAREA,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 370,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 50,
        maxDamage = 125,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 15,
        duration = 5000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = -1},
    {type = COMBAT_FIREDAMAGE, percent = 1},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "energy", combat = true, condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 57,
    },
    {
        id = "life ring",
        chance = 12500,
    },
    {
        id = "meat",
        chance = 25000,
        maxCount = 4,
    },
    {
        id = "panther head",
        chance = 12500,
    },
    {
        id = "panther paw",
        chance = 100000,
    },
}

mtype:register(monster)
