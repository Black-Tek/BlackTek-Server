local mtype = Game.createMonsterType("Gargoyle")
local monster = {}

monster.name = "Gargoyle"
monster.description = "a gargoyle"
monster.experience = 150
monster.race = "undead"
monster.maxHealth = 250
monster.health = 250
monster.speed = 200
monster.manaCost = 0
monster.corpse = 6027
monster.outfit = { lookType = 95 }
monster.runHealth = 30
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -65,
    },
}
monster.defenses = {
    defense = 26,
    armor = 26,
    {
        name = "healing",
        interval = 2000,
        chance = 20,
        minDamage = 5,
        maxDamage = 15,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = 40},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Harrrr Harrrr!", yell = false},
    {text = "Stone sweet stone.", yell = false},
    {text = "Feel my claws, softskin.", yell = false},
    {text = "Chhhhhrrrrk!", yell = false},
    {text = "There is a stone in your shoe!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 88000,
        maxCount = 30,
    },
    {
        id = "small stone",
        chance = 15800,
        maxCount = 10,
    },
    {
        id = "stone wing",
        chance = 11730,
    },
    {
        id = "potato",
        chance = 9220,
        maxCount = 2,
    },
    {
        id = "strawberry",
        chance = 1910,
        maxCount = 5,
    },
    {
        id = "morning star",
        chance = 1500,
    },
    {
        id = "battle shield",
        chance = 1300,
    },
    {
        id = "wolf tooth chain",
        chance = 1180,
    },
    {
        id = "steel helmet",
        chance = 850,
    },
    {
        id = "piece of marble rock",
        chance = 450,
    },
    {
        id = "dark armor",
        chance = 300,
    },
    {
        id = "club ring",
        chance = 260,
    },
    {
        id = "shiny stone",
        chance = 190,
    },
}

mtype:register(monster)
