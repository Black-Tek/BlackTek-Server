local mtype = Game.createMonsterType("Silencer")
local monster = {}

monster.name = "Silencer"
monster.description = "a silencer"
monster.experience = 5100
monster.race = "blood"
monster.maxHealth = 5400
monster.health = 5400
monster.speed = 470
monster.manaCost = 0
monster.corpse = 22489
monster.outfit = { lookType = 585 }
monster.changeTarget = {
    interval = 4000,
    chance = 5,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -315,
        -- NOTE: melee condition (poison=600); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 600000,
        },
    },
    {
        name = "silencer skill reducer",
        interval = 2000,
        chance = 10,
        range = 3,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 15,
        minDamage = -40,
        maxDamage = -150,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_ONYXARROW,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 71,
    armor = 71,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 490,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 220,
        maxDamage = 425,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_DEATHDAMAGE, percent = 70},
    {type = COMBAT_FIREDAMAGE, percent = 30},
    {type = COMBAT_ENERGYDAMAGE, percent = 15},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Prrrroooaaaah!!! PRROAAAH!!", yell = false},
    {text = "PRRRROOOOOAAAAAHHHH!!!", yell = false},
    {text = "HUUUSSSSSSSSH!!", yell = false},
    {text = "Hussssssh!!", yell = false},
}
monster.loot = {
    {
        id = 2152,
        chance = 99919,
        maxCount = 8,
    },
    {
        id = 2148,
        chance = 99717,
        maxCount = 100,
    },
    {
        id = 22534,
        chance = 16350,
    },
    {
        id = 22535,
        chance = 9528,
    },
    {
        id = 7368,
        chance = 8357,
        maxCount = 10,
    },
    {
        id = 7454,
        chance = 2584,
    },
    {
        id = 7413,
        chance = 1978,
    },
    {
        id = 7407,
        chance = 1938,
    },
    {
        id = 2165,
        chance = 1534,
    },
    {
        id = 2521,
        chance = 1453,
    },
    {
        id = 7387,
        chance = 1373,
    },
    {
        id = 7451,
        chance = 1090,
    },
    {
        id = 7886,
        chance = 929,
    },
    {
        id = 7885,
        chance = 888,
    },
    {
        id = 22396,
        chance = 686,
    },
    {
        id = 2195,
        chance = 444,
    },
}

mtype:register(monster)
