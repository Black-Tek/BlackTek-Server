local mtype = Game.createMonsterType("Humongous Fungus")
local monster = {}

monster.name = "Humongous Fungus"
monster.description = "a humongous fungus"
monster.experience = 2600
monster.race = "blood"
monster.maxHealth = 3400
monster.health = 3400
monster.speed = 270
monster.manaCost = 0
monster.corpse = 18382
monster.outfit = { lookType = 488 }
monster.changeTarget = {
    interval = 4000,
    chance = 15,
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -330,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -180,
        maxDamage = -350,
        shootEffect = CONST_ANI_SMALLEARTH,
        effect = CONST_ME_SMALLPLANTS,
    },
    {
        name = "poisonfield",
        interval = 2000,
        chance = 20,
        radius = 4,
        target = false,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        minDamage = -500,
        maxDamage = -1000,
        length = 8,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -130,
        maxDamage = -260,
        length = 5,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -400,
        maxDamage = -640,
        radius = 3,
        target = false,
        effect = CONST_ME_HITBYPOISON,
    },
}
monster.defenses = {
    defense = 45,
    armor = 45,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 225,
        maxDamage = 380,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 15,
        duration = 4000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 15},
    {type = COMBAT_ICEDAMAGE, percent = 15},
    {type = COMBAT_FIREDAMAGE, percent = 5},
    {type = COMBAT_HOLYDAMAGE, percent = 5},
    {type = COMBAT_DEATHDAMAGE, percent = 15},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Munch munch munch!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 60000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 60000,
        maxCount = 100,
    },
    {
        id = "platinum coin",
        chance = 100000,
        maxCount = 6,
    },
    {
        id = "white piece of cloth",
        chance = 10000,
    },
    {
        id = "red piece of cloth",
        chance = 2941,
    },
    {
        id = "blue piece of cloth",
        chance = 4545,
    },
    {
        id = "brown piece of cloth",
        chance = 15285,
    },
    {
        id = "angelic axe",
        chance = 1265,
    },
    {
        id = "strong health potion",
        chance = 5000,
        maxCount = 2,
    },
    {
        id = "strong mana potion",
        chance = 5000,
        maxCount = 2,
    },
    {
        id = "great mana potion",
        chance = 5000,
        maxCount = 2,
    },
    {
        id = "great health potion",
        chance = 5000,
        maxCount = 2,
    },
    {
        id = "mana potion",
        chance = 5000,
        maxCount = 3,
    },
    {
        id = "terra mantle",
        chance = 869,
    },
    {
        id = "terra legs",
        chance = 1123,
    },
    {
        id = "terra boots",
        chance = 1851,
    },
    {
        id = "terra amulet",
        chance = 2127,
    },
    {
        id = "mushroom backpack",
        chance = 20,
    },
    {
        id = "mushroom pie",
        chance = 16666,
        maxCount = 3,
    },
    {
        id = "muck rod",
        chance = 680,
    },
    {
        id = "humongous chunk",
        chance = 10000,
    },
    {
        id = "drill bolt",
        chance = 14285,
        maxCount = 15,
    },
    {
        id = "mycological bow",
        chance = 150,
    },
}

mtype:register(monster)
