local mtype = Game.createMonsterType("Armadile")
local monster = {}

monster.name = "Armadile"
monster.description = "an armadile"
monster.experience = 2900
monster.race = "undead"
monster.maxHealth = 3800
monster.health = 3800
monster.speed = 440
monster.manaCost = 0
monster.corpse = 18378
monster.outfit = { lookType = 487 }
monster.runHealth = 300
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    pushable = false,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -150,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 15,
        radius = 4,
        target = true,
        duration = 5000,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -430,
        maxDamage = -550,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 15,
        minDamage = -200,
        maxDamage = -400,
        radius = 4,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 66,
    armor = 66,
    {
        name = "invisible",
        interval = 2000,
        chance = 15,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_ENERGYDAMAGE, percent = 15},
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 15},
    {type = COMBAT_HOLYDAMAGE, percent = 15},
    {type = COMBAT_DEATHDAMAGE, percent = 45},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Creak!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 198,
    },
    {
        id = 2152,
        chance = 95297,
        maxCount = 7,
    },
    {
        id = 7591,
        chance = 15477,
        maxCount = 2,
    },
    {
        id = 7590,
        chance = 15434,
        maxCount = 2,
    },
    {
        id = 7589,
        chance = 15092,
        maxCount = 2,
    },
    {
        id = 7620,
        chance = 14964,
        maxCount = 3,
    },
    {
        id = 18432,
        chance = 14579,
    },
    {
        id = 7588,
        chance = 14579,
        maxCount = 2,
    },
    {
        id = 12403,
        chance = 12527,
    },
    {
        id = 18437,
        chance = 11030,
        maxCount = 10,
    },
    {
        id = 18436,
        chance = 7781,
        maxCount = 5,
    },
    {
        id = 13757,
        chance = 6456,
    },
    {
        id = 18421,
        chance = 5985,
        maxCount = 2,
    },
    {
        id = 18416,
        chance = 4147,
        maxCount = 2,
    },
    {
        id = 7886,
        chance = 2565,
    },
    {
        id = 7413,
        chance = 1496,
    },
    {
        id = 2169,
        chance = 1240,
    },
    {
        id = 7428,
        chance = 1069,
    },
    {
        id = 2528,
        chance = 599,
    },
    {
        id = 8878,
        chance = 214,
    },
}

mtype:register(monster)
