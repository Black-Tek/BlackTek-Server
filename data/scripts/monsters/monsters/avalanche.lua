local mtype = Game.createMonsterType("Avalanche")
local monster = {}

monster.name = "Avalanche"
monster.description = "Avalanche"
monster.experience = 305
monster.race = "undead"
monster.maxHealth = 550
monster.health = 550
monster.speed = 195
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 261 }
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -180,
    },
    {
        name = "drown",
        interval = 1000,
        chance = 100,
        minDamage = -10,
        maxDamage = -50,
        length = 5,
        spread = 6,
        effect = CONST_ME_ENERGYAREA,
    },
    {
        name = "speed",
        interval = 4000,
        chance = 100,
        radius = 3,
        speed = -400,
        duration = 10000,
        effect = CONST_ME_POFF,
    },
    {
        name = "physical",
        interval = 6000,
        chance = 100,
        range = 5,
        minDamage = 0,
        maxDamage = -40,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_LARGEROCK,
    },
}
monster.defenses = {
    defense = 27,
    armor = 26,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 30},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 5,
    {text = "You will pay for imprisoning me here.", yell = false},
    {text = "Puny warmblood.", yell = false},
}

mtype:register(monster)
