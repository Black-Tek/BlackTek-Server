local mtype = Game.createMonsterType("Colerian the Barbarian")
local monster = {}

monster.name = "Colerian the Barbarian"
monster.description = "Colerian the Barbarian"
monster.experience = 90
monster.race = "blood"
monster.maxHealth = 265
monster.health = 265
monster.speed = 190
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = {
    lookType = 253,
    lookHead = 77,
    lookBody = 61,
    lookLegs = 97,
    lookFeet = 114,
}
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
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -60,
    },
    {
        name = "physical",
        interval = 1000,
        chance = 80,
        range = 5,
        minDamage = 0,
        maxDamage = -40,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_PIERCINGBOLT,
    },
}
monster.defenses = {
    defense = 20,
    armor = 19,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "Flee, coward!", yell = false},
    {text = "You will lose!", yell = false},
    {text = "Yeehaawh", yell = false},
}

mtype:register(monster)
