local mtype = Game.createMonsterType("Bonelord")
local monster = {}

monster.name = "Bonelord"
monster.description = "a bonelord"
monster.experience = 170
monster.race = "venom"
monster.maxHealth = 260
monster.health = 260
monster.speed = 150
monster.manaCost = 0
monster.corpse = 5992
monster.outfit = { lookType = 17 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -5,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 5,
        range = 7,
        minDamage = -15,
        maxDamage = -45,
        shootEffect = CONST_ANI_ENERGY,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 5,
        range = 7,
        minDamage = -25,
        maxDamage = -45,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "death",
        interval = 2000,
        chance = 5,
        range = 7,
        minDamage = -30,
        maxDamage = -50,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 5,
        range = 7,
        minDamage = -5,
        maxDamage = -45,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "death",
        interval = 2000,
        chance = 5,
        range = 7,
        minDamage = -5,
        maxDamage = -50,
        shootEffect = CONST_ANI_DEATH,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 5,
        range = 7,
        minDamage = 0,
        maxDamage = -45,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 5,
        range = 7,
        minDamage = -5,
        maxDamage = -35,
    },
}
monster.defenses = {
    defense = 5,
    armor = 5,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
}
monster.maxSummons = 6
monster.summons = {
    {name = "Skeleton", interval = 2000, chance = 20, max = 6},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Eye for eye!", yell = false},
    {text = "Here's looking at you!", yell = false},
    {text = "Let me take a look at you!", yell = false},
    {text = "You've got the look!", yell = false},
    {text = "I've got to look!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 99000,
        maxCount = 48,
    },
    {
        id = "longsword",
        chance = 9000,
    },
    {
        id = "morning star",
        chance = 7000,
    },
    {
        id = "small flask of eyedrops",
        chance = 5000,
    },
    {
        id = 2175,
        chance = 5000,
    },
    {
        id = "steel shield",
        chance = 4000,
    },
    {
        id = "two handed sword",
        chance = 3950,
    },
    {
        id = "bonelord eye",
        chance = 1000,
    },
    {
        id = "terra rod",
        chance = 510,
    },
    {
        id = "mana potion",
        chance = 300,
    },
    {
        id = "bonelord shield",
        chance = 110,
    },
}

mtype:register(monster)
