local mtype = Game.createMonsterType("Coral Frog")
local monster = {}

monster.name = "Coral Frog"
monster.experience = 20
monster.race = "blood"
monster.maxHealth = 60
monster.health = 60
monster.speed = 200
monster.manaCost = 305
monster.corpse = 6079
monster.outfit = {
    lookType = 226,
    lookHead = 114,
    lookBody = 79,
    lookLegs = 78,
    lookFeet = 114,
}
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
    illusionable = false,
    convinceable = false,
    pushable = true,
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
        maxDamage = -24,
    },
}
monster.defenses = {
    defense = 5,
    armor = 5,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Ribbit!", yell = false},
    {text = "Ribbit! Ribbit!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 76780,
        maxCount = 10,
    },
    {
        id = "worm",
        chance = 13510,
    },
}

mtype:register(monster)
