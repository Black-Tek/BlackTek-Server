local mtype = Game.createMonsterType("Clay Guardian")
local monster = {}

monster.name = "Clay Guardian"
monster.description = "a clay guardian"
monster.experience = 400
monster.race = "undead"
monster.maxHealth = 625
monster.health = 625
monster.speed = 210
monster.manaCost = 0
monster.corpse = 13972
monster.outfit = { lookType = 333 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 60
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -125,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -30,
        maxDamage = -150,
        shootEffect = CONST_ANI_SMALLEARTH,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 50,
        minDamage = -20,
        maxDamage = -30,
        radius = 3,
        target = false,
        effect = CONST_ME_POFF,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 40,
        maxDamage = 130,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 40},
    {type = COMBAT_DEATHDAMAGE, percent = 50},
    {type = COMBAT_ICEDAMAGE, percent = 50},
    {type = COMBAT_FIREDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "small stone",
        chance = 10000,
        maxCount = 10,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 63,
    },
    {
        id = "blank rune",
        chance = 25000,
    },
    {
        id = "earth arrow",
        chance = 5555,
        maxCount = 8,
    },
    {
        id = "small topaz",
        chance = 320,
    },
    {
        id = "lump of earth",
        chance = 25000,
    },
    {
        id = "clay lump",
        chance = 1100,
    },
}

mtype:register(monster)
