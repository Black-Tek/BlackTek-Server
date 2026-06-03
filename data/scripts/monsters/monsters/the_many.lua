local mtype = Game.createMonsterType("The Many")
local monster = {}

monster.name = "The Many"
monster.description = "The Many"
monster.experience = 4000
monster.race = "blood"
monster.maxHealth = 5000
monster.health = 5000
monster.speed = 260
monster.manaCost = 0
monster.corpse = 6048
monster.outfit = { lookType = 121 }
monster.runHealth = 300
monster.changeTarget = {
    interval = 5000,
    chance = 8,
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -270,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 7,
        minDamage = -65,
        maxDamage = -320,
        length = 8,
        spread = 3,
        effect = CONST_ME_SMALLPLANTS,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 8,
        range = 7,
        radius = 4,
        target = true,
        speed = -360,
        duration = 15000,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 9,
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
        range = 7,
        minDamage = -70,
        maxDamage = -155,
        target = true,
        shootEffect = CONST_ANI_ICE,
        effect = CONST_ME_ICEATTACK,
    },
}
monster.defenses = {
    defense = 35,
    armor = 25,
    {
        name = "healing",
        interval = 2000,
        chance = 30,
        minDamage = 420,
        maxDamage = 500,
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
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "FCHHHHH", yell = false},
    {text = "HISSSS", yell = false},
}
monster.loot = {
    {
        id = "small sapphire",
        chance = 56325,
        maxCount = 5,
    },
    {
        id = "stone skin amulet",
        chance = 100000,
    },
    {
        id = "warrior helmet",
        chance = 75500,
    },
    {
        id = "royal helmet",
        chance = 25500,
    },
    {
        id = "medusa shield",
        chance = 75500,
    },
    {
        id = "strong mana potion",
        chance = 100000,
        maxCount = 2,
    },
    {
        id = "gold ingot",
        chance = 47725,
        maxCount = 3,
    },
    {
        id = "sacred tree amulet",
        chance = 75500,
    },
    {
        id = "egg of The Many",
        chance = 100000,
    },
}

mtype:register(monster)
