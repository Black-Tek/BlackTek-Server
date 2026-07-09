local mtype = Game.createMonsterType("Modified Gnarlhound")
local monster = {}

monster.name = "Modified Gnarlhound"
monster.description = "a modified gnarlhound"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 1500
monster.health = 1500
monster.speed = 450
monster.manaCost = 0
monster.corpse = 13528
monster.outfit = { lookType = 515 }
monster.runHealth = 1500
monster.changeTarget = {
    interval = 2000,
    chance = 20,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = false,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.defenses = {
    defense = 200,
    armor = 200,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 90},
    {type = COMBAT_FIREDAMAGE, percent = 90},
    {type = COMBAT_ENERGYDAMAGE, percent = 90},
    {type = COMBAT_ICEDAMAGE, percent = 90},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "holy", combat = true, condition = true},
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
}

mtype:register(monster)
