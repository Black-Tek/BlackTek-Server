local mtype = Game.createMonsterType("The Halloween Hare")
local monster = {}

monster.name = "The Halloween Hare"
monster.description = "The Halloween Hare"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 1
monster.health = 1
monster.speed = 180
monster.manaCost = 0
monster.outfit = { lookType = 74 }
monster.changeTarget = {
    interval = 2000,
    chance = 95,
}
monster.targetDistance = 4
monster.staticAttackChance = 10
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
monster.attacks = {
    {
        name = "outfit",
        interval = 2000,
        chance = 16,
        radius = 3,
        duration = 60000,
        effect = CONST_ME_POFF,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 16,
        radius = 3,
        duration = 60000,
        effect = CONST_ME_POFF,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 16,
        radius = 3,
        duration = 60000,
        effect = CONST_ME_POFF,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 16,
        radius = 3,
        duration = 60000,
        effect = CONST_ME_POFF,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 16,
        radius = 3,
        duration = 60000,
        effect = CONST_ME_POFF,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 15,
        radius = 3,
        duration = 6000,
        effect = CONST_ME_POFF,
    },
}
monster.defenses = {
    defense = 5,
    armor = 5,
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
    {type = "invisible", condition = true},
}

mtype:register(monster)
