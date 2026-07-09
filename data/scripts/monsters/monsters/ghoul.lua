local mtype = Game.createMonsterType("Ghoul")
local monster = {}

monster.name = "Ghoul"
monster.description = "a ghoul"
monster.experience = 85
monster.race = "blood"
monster.maxHealth = 100
monster.health = 100
monster.speed = 144
monster.manaCost = 450
monster.corpse = 5976
monster.outfit = { lookType = 18 }
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -70,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        range = 1,
        minDamage = -15,
        maxDamage = -27,
        radius = 1,
        target = true,
        effect = CONST_ME_SMALLCLOUDS,
    },
}
monster.defenses = {
    defense = 8,
    armor = 8,
    {
        name = "healing",
        interval = 2000,
        chance = 5,
        minDamage = 9,
        maxDamage = 15,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 30},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "drunk", condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = 2148,
        chance = 69832,
        maxCount = 30,
    },
    {
        id = 11208,
        chance = 14661,
    },
    {
        id = 3976,
        chance = 9815,
        maxCount = 2,
    },
    {
        id = 2050,
        chance = 5171,
    },
    {
        id = 12423,
        chance = 5064,
    },
    {
        id = 12440,
        chance = 1028,
    },
    {
        id = 2483,
        chance = 1012,
    },
    {
        id = 2473,
        chance = 1012,
    },
    {
        id = 5913,
        chance = 1001,
    },
    {
        id = 2229,
        chance = 320,
    },
    {
        id = 2168,
        chance = 181,
    },
}

mtype:register(monster)
