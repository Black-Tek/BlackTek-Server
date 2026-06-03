local mtype = Game.createMonsterType("Dragon Hatchling")
local monster = {}

monster.name = "Dragon Hatchling"
monster.description = "a dragon hatchling"
monster.experience = 185
monster.race = "blood"
monster.maxHealth = 380
monster.health = 380
monster.speed = 170
monster.manaCost = 0
monster.corpse = 7621
monster.outfit = { lookType = 271 }
monster.runHealth = 20
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
        maxDamage = -55,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        minDamage = -60,
        maxDamage = -90,
        length = 5,
        spread = 2,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -30,
        maxDamage = -55,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 8,
        maxDamage = 33,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 75},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
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
        chance = 67500,
        maxCount = 55,
    },
    {
        id = "dragon ham",
        chance = 61000,
    },
    {
        id = "health potion",
        chance = 400,
    },
    {
        id = "dragon's tail",
        chance = 4300,
    },
}

mtype:register(monster)
