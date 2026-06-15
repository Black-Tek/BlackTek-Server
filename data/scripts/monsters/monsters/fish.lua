local mtype = Game.createMonsterType("Fish")
local monster = {}

monster.name = "Fish"
monster.description = "a fish"
monster.experience = 0
monster.race = "undead"
monster.maxHealth = 25
monster.health = 25
monster.speed = 180
monster.manaCost = 0
monster.corpse = 2667
monster.outfit = { lookType = 455 }
monster.runHealth = 25
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = false,
    canPushCreatures = true,
    canWalkOnEnergy = false,
}
monster.defenses = {
    defense = 5,
    armor = 5,
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
    {type = "drown", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Blib!", yell = false},
    {text = "Blub!", yell = false},
}

mtype:register(monster)
