local mtype = Game.createMonsterType("Darakan the Executioner")
local monster = {}

monster.name = "Darakan the Executioner"
monster.description = "Darakan the Executioner"
monster.experience = 1600
monster.race = "blood"
monster.maxHealth = 3480
monster.health = 3480
monster.speed = 205
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = {
    lookType = 255,
    lookHead = 78,
    lookBody = 95,
    lookLegs = 0,
    lookFeet = 95,
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
        maxDamage = -210,
    },
    {
        name = "physical",
        interval = 1000,
        chance = 100,
        minDamage = -72,
        maxDamage = -130,
        shootEffect = CONST_ANI_SPEAR,
    },
}
monster.defenses = {
    defense = 31,
    armor = 30,
    {
        name = "healing",
        interval = 6000,
        chance = 65,
        minDamage = 20,
        maxDamage = 50,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 15},
    {type = COMBAT_FIREDAMAGE, percent = -15},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "FIGHT LIKE A BARBARIAN!", yell = true},
    {text = "VICTORY IS MINE!", yell = true},
    {text = "I AM your father!", yell = false},
    {text = "To be the man you have to beat the man!", yell = false},
}

mtype:register(monster)
