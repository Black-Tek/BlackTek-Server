local mtype = Game.createMonsterType("Tirecz")
local monster = {}

monster.name = "Tirecz"
monster.description = "Tirecz"
monster.experience = 6000
monster.race = "blood"
monster.maxHealth = 25000
monster.health = 25000
monster.speed = 220
monster.manaCost = 0
monster.corpse = 11103
monster.outfit = { lookType = 334 }
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
    canPushCreatures = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -410,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 25,
        range = 7,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 25,
        minDamage = -120,
        maxDamage = -460,
        length = 8,
        spread = 3,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        minDamage = 0,
        maxDamage = -290,
        radius = 3,
        target = false,
        effect = CONST_ME_BLOCKHIT,
    },
    {
        name = "energy",
        interval = 3000,
        chance = 30,
        minDamage = -80,
        maxDamage = -345,
        length = 8,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        minDamage = -200,
        maxDamage = -370,
        radius = 7,
        target = false,
        effect = CONST_ME_DRAWBLOOD,
    },
}
monster.defenses = {
    defense = 19,
    armor = 16,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = 30},
    {type = COMBAT_DEATHDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 30},
    {type = COMBAT_ICEDAMAGE, percent = 30},
    {type = COMBAT_EARTHDAMAGE, percent = 30},
    {type = COMBAT_FIREDAMAGE, percent = 50},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Hissss!", yell = false},
}

mtype:register(monster)
