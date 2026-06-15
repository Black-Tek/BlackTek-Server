local mtype = Game.createMonsterType("Young Sea Serpent")
local monster = {}

monster.name = "Young Sea Serpent"
monster.description = "a young sea serpent"
monster.experience = 1000
monster.race = "blood"
monster.maxHealth = 1050
monster.health = 1050
monster.speed = 320
monster.manaCost = 0
monster.corpse = 9879
monster.outfit = { lookType = 317 }
monster.runHealth = 400
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 85
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
        maxDamage = -200,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        minDamage = -10,
        maxDamage = -250,
        length = 7,
        spread = 2,
        effect = CONST_ME_SMALLPLANTS,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 10,
        minDamage = -80,
        maxDamage = -250,
        length = 7,
        spread = 2,
        effect = CONST_ME_ICEATTACK,
    },
    {
        name = "young sea serpent drown",
        interval = 2000,
        chance = 15,
        range = 5,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 30,
        minDamage = 25,
        maxDamage = 175,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 400,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 30},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_PHYSICALDAMAGE, percent = -15},
    {type = COMBAT_DEATHDAMAGE, percent = -15},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "drown", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "CHHHRRRR", yell = false},
    {text = "HISSSS", yell = false},
}
monster.loot = {
    {
        id = "small sapphire",
        chance = 1900,
        maxCount = 2,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 48000,
        maxCount = 74,
    },
    {
        id = "stealth ring",
        chance = 1000,
    },
    {
        id = "life crystal",
        chance = 300,
    },
    {
        id = "battle axe",
        chance = 8000,
    },
    {
        id = "morning star",
        chance = 40000,
    },
    {
        id = "battle hammer",
        chance = 5000,
    },
    {
        id = "strong health potion",
        chance = 5000,
    },
    {
        id = "strong mana potion",
        chance = 4000,
    },
    {
        id = 9808,
        chance = 7940,
    },
    {
        id = 9809,
        chance = 7940,
    },
    {
        id = "sea serpent scale",
        chance = 5000,
    },
}

mtype:register(monster)
