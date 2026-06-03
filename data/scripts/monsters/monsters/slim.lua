local mtype = Game.createMonsterType("Slim")
local monster = {}

monster.name = "Slim"
monster.description = "Slim"
monster.experience = 580
monster.race = "undead"
monster.maxHealth = 1025
monster.health = 1025
monster.speed = 200
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 101 }
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
        maxDamage = -150,
    },
    {
        name = "poisonfield",
        interval = 1000,
        chance = 50,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "physical",
        interval = 3049,
        chance = 40,
        minDamage = -10,
        maxDamage = -50,
        effect = CONST_ME_HITAREA,
    },
}
monster.defenses = {
    defense = 38,
    armor = 36,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "Zhroozzzzs.", yell = false},
}

mtype:register(monster)
