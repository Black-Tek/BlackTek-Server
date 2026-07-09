local mtype = Game.createMonsterType("Raging Mage")
local monster = {}

monster.name = "Raging Mage"
monster.description = "a raging mage"
monster.experience = 3250
monster.race = "blood"
monster.maxHealth = 3500
monster.health = 3500
monster.speed = 200
monster.manaCost = 0
monster.corpse = 13834
monster.outfit = { lookType = 416 }
monster.runHealth = 300
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -78,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 35,
        range = 7,
        minDamage = -100,
        maxDamage = -200,
        radius = 5,
        target = true,
        shootEffect = CONST_ANI_ENERGYBALL,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -100,
        maxDamage = -200,
    },
    {
        name = "energyfield",
        interval = 2000,
        chance = 15,
        range = 7,
        radius = 2,
        target = true,
        shootEffect = CONST_ANI_ENERGY,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 20,
        minDamage = -101,
        maxDamage = -300,
        length = 8,
        effect = CONST_ME_BIGCLOUDS,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = -25},
    {type = COMBAT_DEATHDAMAGE, percent = 15},
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "outfit", condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 1
monster.summons = {
    {name = "golden servant", interval = 2000, chance = 50, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Behold the all permeating powers I draw from this gate!!", yell = false},
    {text = "ENERGY!!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 97633,
        maxCount = 250,
    },
    {
        id = 2152,
        chance = 78107,
        maxCount = 12,
    },
    {
        id = 5911,
        chance = 30769,
    },
    {
        id = 7591,
        chance = 27219,
        maxCount = 5,
    },
    {
        id = 7590,
        chance = 23077,
        maxCount = 5,
    },
    {
        id = 2178,
        chance = 10651,
    },
    {
        id = 7443,
        chance = 6509,
        maxCount = 2,
    },
    {
        id = 8871,
        chance = 4734,
    },
    {
        id = 2165,
        chance = 4734,
    },
    {
        id = 12410,
        chance = 4142,
    },
    {
        id = 2792,
        chance = 3550,
        maxCount = 4,
    },
    {
        id = 2124,
        chance = 2367,
    },
    {
        id = 2146,
        chance = 2367,
        maxCount = 5,
    },
    {
        id = 7368,
        chance = 2367,
        maxCount = 10,
    },
    {
        id = 2123,
        chance = 1775,
    },
    {
        id = 13940,
        chance = 1183,
    },
    {
        id = 2114,
        chance = 1183,
    },
    {
        id = 9958,
        chance = 1183,
    },
    {
        id = 2195,
        chance = 592,
    },
    {
        id = 9980,
        chance = 592,
    },
    {
        id = 5741,
        chance = 592,
    },
    {
        id = 8902,
        chance = 592,
    },
}

mtype:register(monster)
