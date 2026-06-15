local mtype = Game.createMonsterType("Kraknaknork's Demon")
local monster = {}

monster.name = "Kraknaknork's Demon"
monster.description = "Kraknaknork's Demon"
monster.experience = 0
monster.race = "fire"
monster.maxHealth = 120
monster.health = 120
monster.speed = 400
monster.manaCost = 0
monster.corpse = 6068
monster.outfit = {
    lookType = 12,
    lookHead = 117,
    lookBody = 58,
    lookLegs = 117,
    lookFeet = 0,
}
monster.changeTarget = {
    interval = 5000,
    chance = 15,
}
monster.targetDistance = 1
monster.staticAttackChance = 85
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
        maxDamage = -499,
    },
    {
        name = "fire",
        interval = 3000,
        chance = 20,
        minDamage = -10,
        maxDamage = -40,
        radius = 4,
        target = true,
        effect = CONST_ME_EXPLOSIONHIT,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = -15},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_EARTHDAMAGE, percent = -8},
    {type = COMBAT_FIREDAMAGE, percent = 25},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 100,
    {text = "STEP A BIT CLOSER, RIGHT THERE, HAHAHA!", yell = true},
    {text = "COME AND DIE!", yell = true},
    {text = "ROOKIE FOR BREAKFAST", yell = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 48025,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 48025,
        maxCount = 86,
    },
    {
        id = "gold coin",
        chance = 24400,
    },
}

mtype:register(monster)
