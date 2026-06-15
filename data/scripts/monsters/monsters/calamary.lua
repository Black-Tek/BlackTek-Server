local mtype = Game.createMonsterType("Calamary")
local monster = {}

monster.name = "Calamary"
monster.description = "a calamary"
monster.experience = 0
monster.race = "undead"
monster.maxHealth = 75
monster.health = 75
monster.speed = 160
monster.manaCost = 0
monster.corpse = 15280
monster.outfit = { lookType = 451 }
monster.runHealth = 75
monster.changeTarget = {
    interval = 4000,
    chance = 8,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
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
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Bubble!", yell = false},
    {text = "Bobble!", yell = false},
}
monster.loot = {
    {
        id = "shrimp",
        chance = 12270,
        maxCount = 2,
    },
}

mtype:register(monster)
