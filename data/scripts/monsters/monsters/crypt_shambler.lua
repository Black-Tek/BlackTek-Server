local mtype = Game.createMonsterType("Crypt Shambler")
local monster = {}

monster.name = "Crypt Shambler"
monster.description = "a crypt shambler"
monster.experience = 195
monster.race = "undead"
monster.maxHealth = 330
monster.health = 330
monster.speed = 140
monster.manaCost = 580
monster.corpse = 6029
monster.outfit = { lookType = 100 }
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
    convinceable = true,
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
        maxDamage = -140,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 1,
        minDamage = -28,
        maxDamage = -55,
        target = true,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Aaaaahhhh!", yell = false},
    {text = "Hoooohhh!", yell = false},
    {text = "Uhhhhhhh!", yell = false},
    {text = "Chhhhhhh!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 58000,
        maxCount = 55,
    },
    {
        id = "worm",
        chance = 9000,
        maxCount = 10,
    },
    {
        id = "half-digested piece of meat",
        chance = 5000,
    },
    {
        id = 2230,
        chance = 5000,
    },
    {
        id = "iron helmet",
        chance = 2000,
    },
    {
        id = "rotten meat",
        chance = 1950,
    },
    {
        id = "two handed sword",
        chance = 1950,
    },
    {
        id = "bone shield",
        chance = 1000,
    },
    {
        id = "throwing star",
        chance = 1000,
        maxCount = 3,
    },
    {
        id = "bone sword",
        chance = 1000,
    },
    {
        id = "small diamond",
        chance = 500,
    },
}

mtype:register(monster)
