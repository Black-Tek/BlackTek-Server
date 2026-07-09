local mtype = Game.createMonsterType("Draptor")
local monster = {}

monster.name = "Draptor"
monster.description = "a draptor"
monster.experience = 2400
monster.race = "blood"
monster.maxHealth = 3000
monster.health = 3000
monster.speed = 282
monster.manaCost = 0
monster.corpse = 13316
monster.outfit = { lookType = 382 }
monster.runHealth = 350
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
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -150,
    },
    {
        name = "energy",
        interval = 3000,
        chance = 30,
        minDamage = -130,
        maxDamage = -310,
        radius = 3,
        effect = CONST_ME_YELLOWENERGY,
    },
    {
        name = "energy",
        interval = 3000,
        chance = 35,
        range = 7,
        minDamage = -200,
        maxDamage = -300,
        shootEffect = CONST_ANI_ENERGY,
    },
    {
        name = "fire",
        interval = 2500,
        chance = 17,
        minDamage = -70,
        maxDamage = -250,
        length = 8,
        spread = 3,
        effect = CONST_ME_FIREAREA,
    },
}
monster.defenses = {
    defense = 37,
    armor = 23,
    {
        name = "healing",
        interval = 1000,
        chance = 25,
        minDamage = 57,
        maxDamage = 93,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 12,
        speed = 457,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 33750,
        maxCount = 90,
    },
    {
        id = "gold coin",
        chance = 33750,
        maxCount = 60,
    },
    {
        id = "strong health potion",
        chance = 3150,
    },
    {
        id = "strong mana potion",
        chance = 4150,
    },
    {
        id = "dragon robe",
        chance = 950,
    },
    {
        id = "draptor scales",
        chance = 6650,
    },
}

mtype:register(monster)
