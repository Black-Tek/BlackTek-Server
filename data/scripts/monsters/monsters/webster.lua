local mtype = Game.createMonsterType("Webster")
local monster = {}

monster.name = "Webster"
monster.description = "Webster"
monster.experience = 1200
monster.race = "undead"
monster.maxHealth = 2950
monster.health = 2950
monster.speed = 290
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 263 }
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canWalkOnEnergy = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -250,
    },
    {
        name = "speed",
        interval = 3500,
        chance = 65,
        range = 1,
        radius = 1,
        target = true,
        speed = -500,
        duration = 40,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "energy",
        interval = 3000,
        chance = 75,
        range = 7,
        minDamage = -13,
        maxDamage = -80,
        shootEffect = CONST_ANI_ENERGY,
    },
}
monster.defenses = {
    defense = 34,
    armor = 29,
    {
        name = "speed",
        interval = 5000,
        chance = 100,
        speed = 500,
        duration = 2500,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "You are lost!", yell = false},
    {text = "Come my little morsel.", yell = false},
}

mtype:register(monster)
