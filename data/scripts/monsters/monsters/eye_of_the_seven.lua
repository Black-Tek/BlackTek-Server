local mtype = Game.createMonsterType("Eye of the Seven")
local monster = {}

monster.name = "Eye of the Seven"
monster.description = "an eye of the seven"
monster.experience = 0
monster.race = "venom"
monster.maxHealth = 1
monster.health = 1
monster.speed = 0
monster.manaCost = 0
monster.corpse = 6036
monster.outfit = { lookType = 109 }
monster.changeTarget = {
    interval = 5000,
    chance = 20,
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
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -500,
    },
    {
        name = "lifedrain",
        interval = 1000,
        chance = 9,
        range = 6,
        minDamage = 0,
        maxDamage = -500,
    },
}
monster.defenses = {
    defense = 1,
    armor = 1,
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
}

mtype:register(monster)
