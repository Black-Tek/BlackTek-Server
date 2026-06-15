local mtype = Game.createMonsterType("Orchid Frog")
local monster = {}

monster.name = "Orchid Frog"
monster.description = "an orchid frog"
monster.experience = 20
monster.race = "blood"
monster.maxHealth = 60
monster.health = 60
monster.speed = 200
monster.manaCost = 305
monster.corpse = 6079
monster.outfit = {
    lookType = 226,
    lookHead = 71,
    lookBody = 14,
    lookLegs = 71,
    lookFeet = 70,
}
monster.changeTarget = {
    interval = 5000,
    chance = 20,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = true,
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
        chance = 73230,
        maxCount = 10,
    },
    {
        id = "worm",
        chance = 8000,
    },
}

mtype:register(monster)
