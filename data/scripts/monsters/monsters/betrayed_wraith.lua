local mtype = Game.createMonsterType("Betrayed Wraith")
local monster = {}

monster.name = "Betrayed Wraith"
monster.description = "a betrayed wraith"
monster.experience = 3500
monster.race = "undead"
monster.maxHealth = 4200
monster.health = 4200
monster.speed = 346
monster.manaCost = 0
monster.corpse = 6316
monster.outfit = { lookType = 233 }
monster.runHealth = 300
monster.changeTarget = {
    interval = 4000,
    chance = 15,
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -450,
    },
    {
        name = "betrayed wraith skill reducer",
        chance = 10,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        range = 7,
        target = true,
        speed = -600,
        duration = 3000,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
}
monster.defenses = {
    defense = 42,
    armor = 42,
    {
        name = "healing",
        interval = 2000,
        chance = 30,
        minDamage = 350,
        maxDamage = 600,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 5000,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 460,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 50},
    {type = COMBAT_HOLYDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Rrrah!", yell = false},
    {text = "Gnarr!", yell = false},
    {text = "Tcharrr!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 200,
    },
    {
        id = 2152,
        chance = 100000,
        maxCount = 8,
    },
    {
        id = 6558,
        chance = 65436,
    },
    {
        id = 2547,
        chance = 49940,
        maxCount = 15,
    },
    {
        id = 6500,
        chance = 20426,
    },
    {
        id = 11233,
        chance = 19612,
    },
    {
        id = 8473,
        chance = 15177,
    },
    {
        id = 7590,
        chance = 14707,
        maxCount = 3,
    },
    {
        id = 2145,
        chance = 11835,
        maxCount = 4,
    },
    {
        id = 7368,
        chance = 9631,
        maxCount = 5,
    },
    {
        id = 5944,
        chance = 9412,
    },
    {
        id = 5022,
        chance = 6374,
        maxCount = 2,
    },
    {
        id = 7386,
        chance = 1503,
    },
    {
        id = 6300,
        chance = 404,
    },
    {
        id = 5741,
        chance = 377,
    },
    {
        id = 5799,
        chance = 126,
    },
    {
        id = 7416,
        chance = 99,
    },
}

mtype:register(monster)
