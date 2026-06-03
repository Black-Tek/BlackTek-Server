local mtype = Game.createMonsterType("Bad Dream")
local monster = {}

monster.name = "Bad Dream"
monster.description = "a bad dream"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 7200
monster.health = 7200
monster.speed = 0
monster.manaCost = 0
monster.corpse = 22497
monster.outfit = { lookTypeEx = 22444 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = false,
    hostile = false,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.immunities = {
    {type = "physical", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "holy", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "manadrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "drunk", condition = true},
    {type = "outfit", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Ruu... mosha tak..", yell = false},
    {text = "Kororr roshakk!", yell = false},
    {text = "Grrrr!...", yell = false},
    {text = "Brrzfchh", yell = false},
}

mtype:register(monster)
