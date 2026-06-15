local mtype = Game.createMonsterType("Mushroom Sniffer")
local monster = {}

monster.name = "Mushroom Sniffer"
monster.description = "a mushroom sniffer"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 250
monster.health = 250
monster.speed = 120
monster.manaCost = 0
monster.corpse = 2935
monster.outfit = { lookType = 60 }
monster.runHealth = 25
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = false,
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 90},
    {type = COMBAT_ICEDAMAGE, percent = 90},
    {type = COMBAT_ENERGYDAMAGE, percent = 90},
    {type = COMBAT_FIREDAMAGE, percent = 90},
    {type = COMBAT_DEATHDAMAGE, percent = 90},
    {type = COMBAT_HOLYDAMAGE, percent = 90},
    {type = COMBAT_PHYSICALDAMAGE, percent = 90},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Sniff", yell = false},
    {text = "Oink", yell = false},
    {text = "Oink oink", yell = false},
}

mtype:register(monster)
