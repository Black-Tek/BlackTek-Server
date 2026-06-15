local mtype = Game.createMonsterType("Magicthrower")
local monster = {}

monster.name = "Magicthrower"
monster.description = "a magicthrower"
monster.experience = 0
monster.race = "undead"
monster.maxHealth = 1
monster.health = 1
monster.speed = 0
monster.manaCost = 0
monster.outfit = { lookTypeEx = 1551 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
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
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "energy",
        interval = 2000,
        chance = 100,
        range = 7,
        minDamage = 0,
        maxDamage = -100,
        shootEffect = CONST_ANI_ENERGY,
    },
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

mtype:register(monster)
