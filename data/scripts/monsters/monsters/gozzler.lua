local mtype = Game.createMonsterType("Gozzler")
local monster = {}

monster.name = "Gozzler"
monster.description = "a gozzler"
monster.experience = 180
monster.race = "undead"
monster.maxHealth = 240
monster.health = 240
monster.speed = 190
monster.manaCost = 800
monster.corpse = 9938
monster.outfit = { lookType = 313 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 70
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -110,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 1,
        minDamage = -32,
        maxDamage = -135,
        effect = CONST_ME_DRAWBLOOD,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 30,
        maxDamage = 50,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 210,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 50},
    {type = COMBAT_HOLYDAMAGE, percent = 50},
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Huhuhuhuuu!", yell = false},
    {text = "Let the fun begin!", yell = false},
    {text = "Yihahaha!", yell = false},
    {text = "I'll bite you! Nyehehehehe!", yell = false},
}
monster.loot = {
    {
        id = "brown flask",
        chance = 8750,
    },
    {
        id = "small sapphire",
        chance = 360,
    },
    {
        id = "gold coin",
        chance = 52500,
        maxCount = 70,
    },
    {
        id = "dwarven ring",
        chance = 190,
    },
    {
        id = "battle axe",
        chance = 3100,
    },
    {
        id = "sabre",
        chance = 8250,
    },
    {
        id = "morning star",
        chance = 5000,
    },
    {
        id = "serpent sword",
        chance = 250,
    },
    {
        id = "clerical mace",
        chance = 900,
    },
    {
        id = "plate shield",
        chance = 10000,
    },
}

mtype:register(monster)
