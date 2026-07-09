local mtype = Game.createMonsterType("Hive Pore")
local monster = {}

monster.name = "Hive Pore"
monster.description = "Hive Pore"
monster.experience = 0
monster.race = "venom"
monster.maxHealth = 1
monster.health = 1
monster.speed = 0
monster.manaCost = 0
monster.outfit = { lookTypeEx = 15467 }
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
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.defenses = {
    {
        name = "effect",
        interval = 30000,
        chance = 100,
        radius = 3,
        target = false,
        effect = CONST_ME_HITBYPOISON,
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
monster.maxSummons = 3
monster.summons = {
    {name = "Lesser Swarmer", interval = 10000, chance = 100, max = 0},
}

mtype:register(monster)
