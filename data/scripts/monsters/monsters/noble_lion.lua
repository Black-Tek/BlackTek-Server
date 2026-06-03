local mtype = Game.createMonsterType("Noble Lion")
local monster = {}

monster.name = "Noble Lion"
monster.description = "a noble lion"
monster.experience = 400
monster.race = "blood"
monster.maxHealth = 1000
monster.health = 1000
monster.speed = 230
monster.manaCost = 0
monster.corpse = 21420
monster.outfit = { lookType = 570 }
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
        maxDamage = -165,
    },
}
monster.defenses = {
    defense = 27,
    armor = 27,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 260,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 50},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Groarrrr! Rwarrrr!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 74,
    },
    {
        id = 2666,
        chance = 25278,
    },
    {
        id = 2671,
        chance = 23609,
    },
    {
        id = 10608,
        chance = 20581,
    },
}

mtype:register(monster)
