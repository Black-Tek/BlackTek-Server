local mtype = Game.createMonsterType("Rift Worm")
local monster = {}

monster.name = "Rift Worm"
monster.description = "Rift Worm"
monster.experience = 1195
monster.race = "blood"
monster.maxHealth = 2800
monster.health = 2800
monster.speed = 200
monster.manaCost = 0
monster.outfit = { lookType = 295 }
monster.changeTarget = {
    interval = 60000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 50
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -160,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -500,
        maxDamage = -1000,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 5000,
        effect = CONST_ME_GROUNDSHAKER,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = -5},
    {type = COMBAT_EARTHDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "invisible", condition = true},
}

mtype:register(monster)
