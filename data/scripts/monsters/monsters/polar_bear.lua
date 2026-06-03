local mtype = Game.createMonsterType("Polar Bear")
local monster = {}

monster.name = "Polar Bear"
monster.description = "a polar bear"
monster.experience = 28
monster.race = "blood"
monster.maxHealth = 85
monster.health = 85
monster.speed = 156
monster.manaCost = 315
monster.corpse = 5987
monster.outfit = { lookType = 42 }
monster.runHealth = 5
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = false,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -30,
    },
}
monster.defenses = {
    defense = 7,
    armor = 7,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "GROARRR!", yell = false},
}
monster.loot = {
    {
        id = 2666,
        chance = 50363,
    },
    {
        id = 2671,
        chance = 50166,
    },
    {
        id = 10567,
        chance = 1013,
    },
}

mtype:register(monster)
