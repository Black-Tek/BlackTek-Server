local mtype = Game.createMonsterType("Crystal Spider")
local monster = {}

monster.name = "Crystal Spider"
monster.description = "a crystal spider"
monster.experience = 900
monster.race = "undead"
monster.maxHealth = 1250
monster.health = 1250
monster.speed = 230
monster.manaCost = 0
monster.corpse = 7344
monster.outfit = { lookType = 263 }
monster.changeTarget = {
    interval = 4000,
    chance = 15,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
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
        -- NOTE: melee condition (poison=160); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 160000,
        },
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        radius = 6,
        target = false,
        speed = -800,
        duration = 15000,
        effect = CONST_ME_POFF,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -50,
        maxDamage = -100,
        target = true,
        shootEffect = CONST_ANI_ICE,
        effect = CONST_ME_ICEAREA,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        range = 7,
        target = true,
        speed = -600,
        duration = 10000,
        shootEffect = CONST_ANI_SNOWBALL,
    },
}
monster.defenses = {
    defense = 43,
    armor = 43,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 250,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Screeech!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 197,
    },
    {
        id = 7589,
        chance = 14973,
    },
    {
        id = 2463,
        chance = 9929,
    },
    {
        id = 7364,
        chance = 5874,
        maxCount = 6,
    },
    {
        id = 3351,
        chance = 5164,
    },
    {
        id = 7441,
        chance = 4906,
    },
    {
        id = 7449,
        chance = 2517,
    },
    {
        id = 5879,
        chance = 1983,
    },
    {
        id = 2125,
        chance = 1640,
    },
    {
        id = 2124,
        chance = 1587,
    },
    {
        id = 2169,
        chance = 1485,
    },
    {
        id = 2477,
        chance = 747,
    },
    {
        id = 7290,
        chance = 717,
    },
    {
        id = 7902,
        chance = 664,
    },
    {
        id = 2476,
        chance = 559,
    },
    {
        id = 7437,
        chance = 142,
    },
    {
        id = 2171,
        chance = 123,
    },
    {
        id = 5801,
        chance = 83,
    },
}

mtype:register(monster)
