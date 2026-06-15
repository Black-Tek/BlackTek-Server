local mtype = Game.createMonsterType("Deathslicer")
local monster = {}

monster.name = "Deathslicer"
monster.description = "a deathslicer"
monster.experience = 0
monster.race = "undead"
monster.maxHealth = 1
monster.health = 1
monster.speed = 200
monster.manaCost = 0
monster.corpse = 2253
monster.outfit = { lookType = 102 }
monster.runHealth = 20
monster.changeTarget = {
    interval = 5000,
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
    pushable = true,
    canPushItems = true,
    canPushCreatures = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -500,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 25,
        minDamage = -200,
        maxDamage = -400,
        radius = 2,
        target = false,
        effect = CONST_ME_BLOCKHIT,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 10,
        minDamage = -100,
        maxDamage = -200,
        length = 5,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 199,
    armor = 199,
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
