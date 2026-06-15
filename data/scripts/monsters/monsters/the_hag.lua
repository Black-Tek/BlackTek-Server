local mtype = Game.createMonsterType("The Hag")
local monster = {}

monster.name = "The Hag"
monster.description = "The Hag"
monster.experience = 510
monster.race = "blood"
monster.maxHealth = 935
monster.health = 935
monster.speed = 205
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = {
    lookType = 264,
    lookHead = 19,
    lookBody = 1,
    lookLegs = 40,
    lookFeet = 2,
}
monster.targetDistance = 5
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
        maxDamage = -100,
    },
    {
        name = "drunk",
        interval = 3000,
        chance = 35,
        range = 5,
        radius = 1,
        target = true,
        duration = 15000,
    },
    {
        name = "speed",
        interval = 4000,
        chance = 55,
        range = 5,
        radius = 1,
        target = true,
        speed = -400,
        duration = 12000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 25,
    armor = 24,
    {
        name = "healing",
        interval = 2000,
        chance = 35,
        minDamage = 15,
        maxDamage = 75,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 3000,
        chance = 50,
        duration = 8000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 30},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Ghost", interval = 2000, chance = 26, max = 0},
    {name = "Crypt Shambler", interval = 2000, chance = 26, max = 0},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "If you think I am to old to fight then you're wrong!", yell = false},
    {text = "I've forgotten more things then you have ever learned!", yell = false},
    {text = "Let me teach you a few things youngster!", yell = false},
    {text = "I'll teach you respect for the old!", yell = false},
}

mtype:register(monster)
