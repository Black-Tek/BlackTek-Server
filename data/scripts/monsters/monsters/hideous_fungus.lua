local mtype = Game.createMonsterType("Hideous Fungus")
local monster = {}

monster.name = "Hideous Fungus"
monster.description = "a hideous fungus"
monster.experience = 2900
monster.race = "venom"
monster.maxHealth = 4600
monster.health = 4600
monster.speed = 260
monster.manaCost = 0
monster.corpse = 17428
monster.outfit = { lookType = 499 }
monster.runHealth = 275
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
        maxDamage = -450,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -250,
        maxDamage = -430,
        shootEffect = CONST_ANI_SMALLEARTH,
        effect = CONST_ME_SMALLPLANTS,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 15,
        minDamage = -250,
        maxDamage = -550,
        length = 8,
        spread = 3,
        shootEffect = CONST_ANI_SNOWBALL,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        radius = 1,
        target = true,
        speed = -600,
        duration = 60000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        range = 7,
        radius = 5,
        target = true,
        duration = 4000,
        shootEffect = CONST_ANI_SMALLSTONE,
        effect = CONST_ME_STUN,
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
    defense = 35,
    armor = 35,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 275,
        maxDamage = 350,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 6000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 5},
    {type = COMBAT_DEATHDAMAGE, percent = 1},
    {type = COMBAT_FIREDAMAGE, percent = 5},
    {type = COMBAT_ENERGYDAMAGE, percent = 15},
    {type = COMBAT_ICEDAMAGE, percent = 15},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "humorless fungus", interval = 2000, chance = 10, max = 2},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Munch munch munch!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 70000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 70000,
        maxCount = 97,
    },
    {
        id = "platinum coin",
        chance = 100000,
        maxCount = 7,
    },
    {
        id = "war hammer",
        chance = 4010,
    },
    {
        id = "white piece of cloth",
        chance = 6430,
    },
    {
        id = "green piece of cloth",
        chance = 3003,
    },
    {
        id = "red piece of cloth",
        chance = 1820,
    },
    {
        id = "blue piece of cloth",
        chance = 3700,
    },
    {
        id = "great mana potion",
        chance = 7692,
    },
    {
        id = "great health potion",
        chance = 7220,
    },
    {
        id = "mana potion",
        chance = 9090,
    },
    {
        id = "terra mantle",
        chance = 800,
    },
    {
        id = "terra legs",
        chance = 920,
    },
    {
        id = "terra boots",
        chance = 2000,
    },
    {
        id = "terra amulet",
        chance = 2000,
    },
    {
        id = "mushroom backpack",
        chance = 20,
    },
    {
        id = "mushroom pie",
        chance = 14500,
        maxCount = 3,
    },
    {
        id = "muck rod",
        chance = 590,
    },
    {
        id = "hideous chunk",
        chance = 14285,
    },
    {
        id = "envenomed arrow",
        chance = 14500,
        maxCount = 15,
    },
    {
        id = "mycological bow",
        chance = 60,
    },
}

mtype:register(monster)
