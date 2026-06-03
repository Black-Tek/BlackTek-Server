local mtype = Game.createMonsterType("White Deer")
local monster = {}

monster.name = "White Deer"
monster.description = "a white deer"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 195
monster.health = 195
monster.speed = 190
monster.manaCost = 0
monster.corpse = 13513
monster.outfit = { lookType = 400 }
monster.runHealth = 195
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = false,
    illusionable = true,
    convinceable = false,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "*wheeze*", yell = false},
    {text = "ROOOAAARR!!", yell = false},
    {text = "*sniff*", yell = false},
    {text = "*bell*", yell = false},
}

mtype:register(monster)
