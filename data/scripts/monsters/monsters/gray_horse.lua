local mtype = Game.createMonsterType("Horse")
local monster = {}

monster.name = "Horse"
monster.description = "a horse"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 75
monster.health = 75
monster.speed = 190
monster.manaCost = 0
monster.outfit = { lookType = 434 }
monster.runHealth = 75
monster.changeTarget = {
    interval = 4000,
    chance = 20,
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
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Weeeeheeeeeee", yell = false},
    {text = "*snort*", yell = false},
    {text = "*Weeeeheeeeaaa*", yell = false},
}

mtype:register(monster)
