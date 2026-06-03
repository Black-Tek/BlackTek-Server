local mtype = Game.createMonsterType("Dwarf Dispenser")
local monster = {}

monster.name = "Dwarf Dispenser"
monster.description = "a dwarf dispenser"
monster.experience = 0
monster.race = "undead"
monster.maxHealth = 100
monster.health = 100
monster.speed = 0
monster.manaCost = 0
monster.outfit = { lookTypeEx = 6116 }
monster.changeTarget = {
    interval = 5000,
    chance = 16,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = false,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = false,
    canPushCreatures = false,
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.maxSummons = 5
monster.summons = {
    {name = "Dwarf Henchman", interval = 1000, chance = 55, max = 0},
}

mtype:register(monster)
