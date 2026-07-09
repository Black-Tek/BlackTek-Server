local mtype = Game.createMonsterType("Rift Phantom")
local monster = {}

monster.name = "Rift Phantom"
monster.description = "a rift phantom"
monster.experience = 0
monster.race = "undead"
monster.maxHealth = 100
monster.health = 100
monster.speed = 160
monster.manaCost = 0
monster.corpse = 10015
monster.outfit = { lookType = 48 }
monster.changeTarget = {
    interval = 2000,
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
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
}
monster.defenses = {
    defense = 10,
    armor = 10,
}
monster.immunities = {
    {type = "physical", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
}

mtype:register(monster)
