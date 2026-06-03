local mtype = Game.createMonsterType("Pigeon")
local monster = {}

monster.name = "Pigeon"
monster.description = "a pigeon"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 30
monster.health = 30
monster.speed = 260
monster.manaCost = 0
monster.corpse = 19709
monster.outfit = { lookType = 531 }
monster.runHealth = 30
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
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.defenses = {
    defense = 5,
    armor = 5,
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Coooo! Cooo!", yell = false},
    {text = "Coo! Coooo! Coo! Cooo!", yell = false},
    {text = "Coo! Coo! Coooo!", yell = false},
}

mtype:register(monster)
