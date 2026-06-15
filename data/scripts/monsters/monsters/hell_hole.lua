local mtype = Game.createMonsterType("Hell Hole")
local monster = {}

monster.name = "Hell Hole"
monster.description = "a hell hole"
monster.experience = 0
monster.race = "undead"
monster.maxHealth = 100
monster.health = 100
monster.speed = 0
monster.manaCost = 0
monster.outfit = { lookTypeEx = 389 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = false,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = false,
    canPushCreatures = true,
}
monster.immunities = {
    {type = "physical", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "poison", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "holy", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 1
monster.summons = {
    {name = "deathspawn", interval = 2000, chance = 25, max = 0},
}

mtype:register(monster)
