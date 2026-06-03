local mtype = Game.createMonsterType("The Evil Eye")
local monster = {}

monster.name = "The Evil Eye"
monster.description = "The Evil Eye"
monster.experience = 750
monster.race = "blood"
monster.maxHealth = 1200
monster.health = 1200
monster.speed = 240
monster.manaCost = 0
monster.corpse = 6037
monster.outfit = { lookType = 210 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 4
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -90,
    },
    {
        name = "energy",
        interval = 1000,
        chance = 15,
        range = 7,
        minDamage = -60,
        maxDamage = -130,
        shootEffect = CONST_ANI_ENERGY,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 13,
        range = 7,
        minDamage = -85,
        maxDamage = -115,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "physical",
        interval = 1000,
        chance = 17,
        range = 7,
        minDamage = -135,
        maxDamage = -175,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "earth",
        interval = 1000,
        chance = 15,
        range = 7,
        minDamage = -40,
        maxDamage = -120,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "lifedrain",
        interval = 1000,
        chance = 12,
        range = 7,
        minDamage = -110,
        maxDamage = -130,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 10,
        range = 7,
        speed = -850,
        duration = 20000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "earth",
        interval = 1000,
        chance = 8,
        minDamage = -35,
        maxDamage = -85,
        length = 8,
        spread = 3,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "lifedrain",
        interval = 1000,
        chance = 6,
        minDamage = -75,
        maxDamage = -85,
        length = 8,
        spread = 3,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "manadrain",
        interval = 1000,
        chance = 9,
        minDamage = -150,
        maxDamage = -250,
        length = 8,
        spread = 3,
        effect = CONST_ME_LOSEENERGY,
    },
}
monster.defenses = {
    defense = 23,
    armor = 19,
    {
        name = "healing",
        interval = 1000,
        chance = 9,
        minDamage = 1,
        maxDamage = 219,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 5
monster.summons = {
    {name = "demon skeleton", interval = 1000, chance = 13, max = 5},
    {name = "ghost", interval = 1000, chance = 12, max = 4},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Inferior creatures, bow before my power!", yell = false},
    {text = "653768764!", yell = false},
}
monster.loot = {
    {
        id = "bonelord eye",
        chance = 100000,
    },
    {
        id = "gold coin",
        chance = 33000,
        maxCount = 29,
    },
    {
        id = "gold coin",
        chance = 33000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 33000,
        maxCount = 100,
    },
    {
        id = "small flask of eyedrops",
        chance = 100000,
    },
    {
        id = "morning star",
        chance = 50000,
    },
    {
        id = "steel shield",
        chance = 50000,
    },
    {
        id = "two handed sword",
        chance = 34000,
    },
    {
        id = "longsword",
        chance = 23000,
    },
    {
        id = 2175,
        chance = 23000,
    },
    {
        id = "great mana potion",
        chance = 15000,
    },
    {
        id = "bonelord shield",
        chance = 11000,
    },
    {
        id = "terra mantle",
        chance = 11000,
    },
    {
        id = "terra rod",
        chance = 500,
    },
    {
        id = "bonelord helmet",
        chance = 100,
    },
}

mtype:register(monster)
