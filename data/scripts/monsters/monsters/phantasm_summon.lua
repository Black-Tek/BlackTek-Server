local mtype = Game.createMonsterType("Phantasm")
local monster = {}

monster.name = "Phantasm"
monster.description = "a phantasm"
monster.experience = 1
monster.race = "undead"
monster.maxHealth = 65
monster.health = 65
monster.speed = 340
monster.manaCost = 0
monster.corpse = 6344
monster.outfit = {
    lookType = 241,
    lookHead = 20,
}
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    attackable = true,
    hostile = true,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -15,
    },
    {
        name = "death",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -34,
        maxDamage = -59,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        minDamage = -5,
        maxDamage = -7,
        radius = 3,
        target = false,
        effect = CONST_ME_YELLOW_RINGS,
    },
    {
        name = "phantasm drown",
        interval = 2000,
        chance = 10,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 5,
        radius = 5,
        target = false,
        duration = 6000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
    {
        name = "healing",
        interval = 2000,
        chance = 30,
        minDamage = 40,
        maxDamage = 65,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        speed = 500,
        duration = 6000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 25,
        duration = 8000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "death", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "physical", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Oh my, you forgot to put your pants on!", yell = false},
    {text = "Weeheeheeheehee!", yell = false},
    {text = "Its nothing but a dream!", yell = false},
    {text = "Give in!", yell = false},
}

mtype:register(monster)
