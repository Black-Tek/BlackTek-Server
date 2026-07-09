local mtype = Game.createMonsterType("Axeitus Headbanger")
local monster = {}

monster.name = "Axeitus Headbanger"
monster.description = "Axeitus Headbanger"
monster.experience = 140
monster.race = "blood"
monster.maxHealth = 365
monster.health = 365
monster.speed = 170
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 71 }
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
        maxDamage = -80,
    },
    {
        name = "physical",
        interval = 1000,
        chance = 80,
        range = 5,
        minDamage = 0,
        maxDamage = -50,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_SMALLSTONE,
    },
}
monster.defenses = {
    defense = 17,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "Hicks!", yell = false},
    {text = "Stand still! Both of you! hicks", yell = false},
    {text = "This victory will earn me a casket of beer.", yell = false},
}

mtype:register(monster)
