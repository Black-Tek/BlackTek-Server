local mtype = Game.createMonsterType("Slippery Northern Pike")
local monster = {}

monster.name = "Slippery Northern Pike"
monster.description = "a slippery northern pike"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 100
monster.health = 100
monster.speed = 310
monster.manaCost = 0
monster.corpse = 2669
monster.outfit = { lookType = 454 }
monster.runHealth = 100
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
    pushable = false,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
}
monster.defenses = {
    defense = 9,
    armor = 9,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "invisible", condition = true},
    {type = "drown", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Blub!", yell = false},
}

mtype:register(monster)
