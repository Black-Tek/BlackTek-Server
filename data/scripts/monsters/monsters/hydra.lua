local mtype = Game.createMonsterType("Hydra")
local monster = {}

monster.name = "Hydra"
monster.description = "a hydra"
monster.experience = 2100
monster.race = "blood"
monster.maxHealth = 2350
monster.health = 2350
monster.speed = 206
monster.manaCost = 0
monster.corpse = 6048
monster.outfit = { lookType = 121 }
monster.runHealth = 300
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = true,
    canWalkOnFire = true,
    canWalkOnPoison = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -270,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 25,
        range = 7,
        radius = 4,
        target = true,
        speed = -700,
        duration = 15000,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 10,
        minDamage = -100,
        maxDamage = -250,
        length = 8,
        spread = 3,
        effect = CONST_ME_LOSEENERGY,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 10,
        minDamage = -80,
        maxDamage = -155,
        target = true,
        shootEffect = CONST_ANI_SMALLICE,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        minDamage = -66,
        maxDamage = -320,
        length = 8,
        spread = 3,
        effect = CONST_ME_CARNIPHILA,
    },
}
monster.defenses = {
    defense = 27,
    armor = 27,
    {
        name = "healing",
        interval = 2000,
        chance = 25,
        minDamage = 260,
        maxDamage = 407,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 50},
    {type = COMBAT_HOLYDAMAGE, percent = 30},
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "FCHHHHH", yell = true},
    {text = "HISSSS", yell = true},
}
monster.loot = {
    {
        id = 2148,
        chance = 87759,
        maxCount = 246,
    },
    {
        id = 2671,
        chance = 60233,
    },
    {
        id = 2152,
        chance = 49522,
        maxCount = 3,
    },
    {
        id = 11199,
        chance = 10149,
    },
    {
        id = 2146,
        chance = 4949,
    },
    {
        id = 8842,
        chance = 4941,
    },
    {
        id = 2214,
        chance = 1215,
    },
    {
        id = 2476,
        chance = 1098,
    },
    {
        id = 4850,
        chance = 942,
    },
    {
        id = 2197,
        chance = 902,
    },
    {
        id = 2475,
        chance = 823,
    },
    {
        id = 2177,
        chance = 512,
    },
    {
        id = 7589,
        chance = 465,
    },
    {
        id = 2536,
        chance = 223,
    },
    {
        id = 2498,
        chance = 223,
    },
    {
        id = 2195,
        chance = 95,
    },
}

mtype:register(monster)
