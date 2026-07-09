local mtype = Game.createMonsterType("Deepling Master Librarian")
local monster = {}

monster.name = "Deepling Master Librarian"
monster.description = "a deepling master librarian"
monster.experience = 1900
monster.race = "blood"
monster.maxHealth = 1700
monster.health = 1700
monster.speed = 190
monster.manaCost = 0
monster.corpse = 15211
monster.outfit = { lookType = 443 }
monster.runHealth = 250
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
    illusionable = false,
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
        maxDamage = -210,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 10,
        minDamage = -260,
        maxDamage = -450,
        length = 10,
        spread = 3,
        effect = CONST_ME_ICETORNADO,
    },
    {
        name = "drown",
        interval = 2000,
        chance = 20,
        minDamage = -150,
        maxDamage = -280,
        radius = 4,
        target = true,
        effect = CONST_ME_BUBBLES,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -80,
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
        chance = 10,
        range = 5,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 40,
        maxDamage = 80,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Mmmmmoooaaaaaahaaa!!!", yell = false},
}
monster.loot = {
    {
        id = "small sapphire",
        chance = 8440,
        maxCount = 3,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 100,
    },
    {
        id = "platinum coin",
        chance = 90000,
        maxCount = 3,
    },
    {
        id = "life ring",
        chance = 3200,
    },
    {
        id = 2667,
        chance = 9090,
        maxCount = 2,
    },
    {
        id = "fish fin",
        chance = 1950,
    },
    {
        id = "eye of a deepling",
        chance = 9380,
    },
    {
        id = "deepling staff",
        chance = 3130,
    },
    {
        id = "necklace of the deep",
        chance = 1330,
    },
    {
        id = "spellsinger's seal",
        chance = 25000,
    },
    {
        id = "key to the Drowned Library",
        chance = 20000,
    },
    {
        id = "deepling filet",
        chance = 20000,
    },
    {
        id = "ornate crossbow",
        chance = 39,
    },
}

mtype:register(monster)
