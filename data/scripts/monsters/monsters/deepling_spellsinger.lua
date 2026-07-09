local mtype = Game.createMonsterType("Deepling Spellsinger")
local monster = {}

monster.name = "Deepling Spellsinger"
monster.description = "a deepling spellsinger"
monster.experience = 1000
monster.race = "blood"
monster.maxHealth = 850
monster.health = 850
monster.speed = 190
monster.manaCost = 0
monster.corpse = 15208
monster.outfit = { lookType = 443 }
monster.runHealth = 60
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 60
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = true,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -152,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 10,
        minDamage = -180,
        maxDamage = -350,
        length = 10,
        spread = 3,
        effect = CONST_ME_ICETORNADO,
    },
    {
        name = "drown",
        interval = 2000,
        chance = 20,
        minDamage = -90,
        maxDamage = -130,
        radius = 4,
        target = true,
        effect = CONST_ME_BUBBLES,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -60,
        maxDamage = -140,
        effect = CONST_ME_SMALLPLANTS,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -60,
        maxDamage = -140,
        effect = CONST_ME_SMALLPLANTS,
    },
    {
        name = "deepling spellsinger skill reducer",
        interval = 2000,
        chance = 5,
        range = 5,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = -20},
    {type = COMBAT_ENERGYDAMAGE, percent = -20},
    {type = COMBAT_DEATHDAMAGE, percent = 50},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Jey Obu giotja!!", yell = false},
    {text = "Mmmmmoooaaaaaahaaa!!", yell = false},
}
monster.loot = {
    {
        id = "small sapphire",
        chance = 2854,
    },
    {
        id = "gold coin",
        chance = 70000,
        maxCount = 60,
    },
    {
        id = "platinum coin",
        chance = 80000,
    },
    {
        id = "life ring",
        chance = 2439,
    },
    {
        id = 2667,
        chance = 3448,
    },
    {
        id = "fish fin",
        chance = 498,
    },
    {
        id = "eye of a deepling",
        chance = 2500,
    },
    {
        id = "deepling staff",
        chance = 2000,
    },
    {
        id = "necklace of the deep",
        chance = 813,
    },
    {
        id = "spellsinger's seal",
        chance = 14285,
    },
    {
        id = "key to the Drowned Library",
        chance = 10000,
    },
    {
        id = "deepling filet",
        chance = 14285,
    },
    {
        id = "ornate crossbow",
        chance = 220,
    },
}

mtype:register(monster)
