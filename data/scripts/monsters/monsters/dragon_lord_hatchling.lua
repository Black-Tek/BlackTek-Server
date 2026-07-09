local mtype = Game.createMonsterType("Dragon Lord Hatchling")
local monster = {}

monster.name = "Dragon Lord Hatchling"
monster.description = "a dragon lord hatchling"
monster.experience = 645
monster.race = "blood"
monster.maxHealth = 750
monster.health = 750
monster.speed = 190
monster.manaCost = 0
monster.corpse = 7622
monster.outfit = { lookType = 272 }
monster.runHealth = 30
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
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -90,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        minDamage = -90,
        maxDamage = -125,
        length = 5,
        spread = 2,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -55,
        maxDamage = -105,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "firefield",
        interval = 2000,
        chance = 10,
        range = 7,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_FIRE,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 26,
        maxDamage = 48,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "fire", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Fchu?", yell = false},
    {text = "Rooawwrr", yell = false},
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
        maxCount = 75,
    },
    {
        id = "dragon ham",
        chance = 80000,
    },
    {
        id = "green mushroom",
        chance = 560,
    },
    {
        id = "mana potion",
        chance = 300,
    },
    {
        id = "magma boots",
        chance = 100,
    },
}

mtype:register(monster)
