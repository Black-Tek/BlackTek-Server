local mtype = Game.createMonsterType("Shiversleep")
local monster = {}

monster.name = "Shiversleep"
monster.description = "Shiversleep"
monster.experience = 1900
monster.race = "blood"
monster.maxHealth = 4600
monster.health = 4600
monster.speed = 270
monster.manaCost = 0
monster.corpse = 22497
monster.outfit = { lookType = 592 }
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
        maxDamage = -450,
    },
    {
        name = "earth",
        interval = 1800,
        chance = 10,
        minDamage = -300,
        maxDamage = -685,
        length = 7,
        effect = CONST_ME_STONES,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 9,
        minDamage = -250,
        maxDamage = -590,
        radius = 6,
        effect = CONST_ME_BIGPLANTS,
    },
}
monster.defenses = {
    defense = 55,
    armor = 55,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Knorrrr!", yell = false},
}
monster.loot = {
    {
        id = "white pearl",
        chance = 866,
    },
    {
        id = "black pearl",
        chance = 866,
    },
    {
        id = "gold coin",
        chance = 30000,
        maxCount = 100,
    },
    {
        id = "platinum coin",
        chance = 33333,
        maxCount = 10,
    },
    {
        id = "giant sword",
        chance = 422,
    },
    {
        id = "knight legs",
        chance = 444,
    },
    {
        id = 2487,
        chance = 632,
    },
    {
        id = "iron ore",
        chance = 2195,
        maxCount = 2,
    },
    {
        id = "magic sulphur",
        chance = 632,
    },
    {
        id = "great mana potion",
        chance = 3100,
        maxCount = 4,
    },
    {
        id = "ultimate health potion",
        chance = 2195,
        maxCount = 2,
    },
    {
        id = "crystal of power",
        chance = 1066,
    },
    {
        id = "shiny stone",
        chance = 1032,
    },
    {
        id = "wand of defiance",
        chance = 1766,
    },
    {
        id = "glacial rod",
        chance = 1066,
    },
    {
        id = "prismatic bolt",
        chance = 366,
        maxCount = 8,
    },
    {
        id = "crystalline sword",
        chance = 603,
    },
    {
        id = "crystal crossbow",
        chance = 603,
    },
}

mtype:register(monster)
