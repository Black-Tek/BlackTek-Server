local mtype = Game.createMonsterType("Deathspawn")
local monster = {}

monster.name = "Deathspawn"
monster.experience = 20
monster.race = "blood"
monster.maxHealth = 225
monster.health = 225
monster.speed = 230
monster.manaCost = 305
monster.corpse = 2220
monster.outfit = {
    lookType = 226,
    lookHead = 76,
    lookBody = 0,
    lookLegs = 57,
    lookFeet = 0,
}
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
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -40,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 10,
        minDamage = -400,
        maxDamage = -700,
        length = 7,
        effect = CONST_ME_EXPLOSIONHIT,
    },
    {
        name = "energy",
        interval = 1000,
        chance = 11,
        minDamage = -250,
        maxDamage = -450,
        length = 7,
        effect = CONST_ME_PURPLEENERGY,
    },
}
monster.defenses = {
    defense = 5,
    armor = 1,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = -15},
    {type = COMBAT_ENERGYDAMAGE, percent = -15},
    {type = COMBAT_ICEDAMAGE, percent = -15},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Ribbit!", yell = false},
    {text = "Ribbit! Ribbit!", yell = false},
}

mtype:register(monster)
