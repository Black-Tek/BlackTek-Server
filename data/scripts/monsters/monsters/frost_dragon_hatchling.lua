local mtype = Game.createMonsterType("Frost Dragon Hatchling")
local monster = {}

monster.name = "Frost Dragon Hatchling"
monster.description = "a frost dragon hatchling"
monster.experience = 745
monster.race = "undead"
monster.maxHealth = 800
monster.health = 800
monster.speed = 200
monster.manaCost = 0
monster.corpse = 7969
monster.outfit = { lookType = 283 }
monster.runHealth = 80
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -160,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 10,
        minDamage = -60,
        maxDamage = -110,
        length = 5,
        spread = 2,
        effect = CONST_ME_ICEATTACK,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 15,
        minDamage = -60,
        maxDamage = -110,
        radius = 4,
        target = true,
        effect = CONST_ME_ICEAREA,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        radius = 4,
        target = true,
        speed = -600,
        duration = 12000,
        shootEffect = CONST_ANI_ICE,
        effect = CONST_ME_ICEAREA,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 45,
        maxDamage = 50,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "ice", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Rooawwrr", yell = false},
    {text = "Fchu?", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 86750,
        maxCount = 55,
    },
    {
        id = "dragon ham",
        chance = 79600,
    },
    {
        id = "health potion",
        chance = 560,
    },
    {
        id = "spellbook of enlightenment",
        chance = 400,
    },
    {
        id = "frosty heart",
        chance = 5000,
    },
}

mtype:register(monster)
