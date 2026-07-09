local mtype = Game.createMonsterType("The Pit Lord")
local monster = {}

monster.name = "The Pit Lord"
monster.description = "The Pit Lord"
monster.experience = 2500
monster.race = "blood"
monster.maxHealth = 5270
monster.health = 5270
monster.speed = 270
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 55 }
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
        maxDamage = -380,
    },
    {
        name = "physical",
        interval = 7500,
        chance = 100,
        minDamage = -100,
        maxDamage = -250,
        shootEffect = CONST_ANI_LARGEROCK,
    },
}
monster.defenses = {
    defense = 44,
    armor = 46,
    {
        name = "speed",
        interval = 5000,
        chance = 100,
        speed = 500,
        duration = 2500,
        effect = CONST_ME_MAGIC_RED,
    },
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
    {type = COMBAT_EARTHDAMAGE, percent = 35},
    {type = COMBAT_ICEDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "I'LL GET YOU ALL!", yell = true},
    {text = "I won't let you escape!", yell = false},
    {text = "I'll crush you beneath my feet!", yell = false},
}

mtype:register(monster)
