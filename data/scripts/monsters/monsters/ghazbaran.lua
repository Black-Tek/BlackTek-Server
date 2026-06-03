local mtype = Game.createMonsterType("Ghazbaran")
local monster = {}

monster.name = "Ghazbaran"
monster.description = "Ghazbaran"
monster.experience = 15000
monster.race = "fire"
monster.maxHealth = 77000
monster.health = 77000
monster.speed = 400
monster.manaCost = 0
monster.corpse = 6068
monster.outfit = {
    lookType = 12,
    lookHead = 0,
    lookBody = 66,
    lookLegs = 98,
    lookFeet = 75,
}
monster.runHealth = 3500
monster.changeTarget = {
    interval = 10000,
    chance = 20,
}
monster.targetDistance = 1
monster.staticAttackChance = 98
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
        maxDamage = -2191,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 40,
        range = 7,
        minDamage = -250,
        maxDamage = -500,
        radius = 6,
        target = false,
        effect = CONST_ME_HITAREA,
    },
    {
        name = "physical",
        interval = 3000,
        chance = 34,
        range = 7,
        minDamage = -120,
        maxDamage = -500,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_WHIRLWINDSWORD,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "energy",
        interval = 4000,
        chance = 30,
        minDamage = -100,
        maxDamage = -800,
        length = 8,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "physical",
        interval = 3000,
        chance = 20,
        range = 14,
        minDamage = -200,
        maxDamage = -480,
        radius = 5,
        target = false,
        effect = CONST_ME_POFF,
    },
    {
        name = "physical",
        interval = 4000,
        chance = 15,
        range = 7,
        minDamage = -100,
        maxDamage = -650,
        radius = 13,
        target = false,
        effect = CONST_ME_BLOCKHIT,
    },
    {
        name = "physical",
        interval = 4000,
        chance = 18,
        minDamage = -200,
        maxDamage = -600,
        radius = 14,
        target = false,
        effect = CONST_ME_LOSEENERGY,
    },
    {
        name = "physical",
        interval = 3000,
        chance = 15,
        range = 7,
        minDamage = -200,
        maxDamage = -750,
        radius = 4,
        target = false,
        effect = CONST_ME_ENERGYAREA,
    },
}
monster.defenses = {
    defense = 65,
    armor = 55,
    {
        name = "healing",
        interval = 3000,
        chance = 35,
        minDamage = 300,
        maxDamage = 800,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 4000,
        chance = 80,
        speed = 440,
        duration = 6000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 30},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = 1},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 4
monster.summons = {
    {name = "Deathslicer", interval = 4000, chance = 20, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 30,
    {text = "COME AND GIVE ME SOME AMUSEMENT", yell = false},
    {text = "IS THAT THE BEST YOU HAVE TO OFFER?", yell = true},
    {text = "I AM GHAZBARAN OF THE TRIANGLE... AND I AM HERE TO CHALLENGE YOU ALL.", yell = true},
    {text = "FLAWLESS VICTORY!", yell = true},
}
monster.loot = {
    {
        id = "blue tome",
        chance = 20000,
    },
    {
        id = "teddy bear",
        chance = 12500,
    },
    {
        id = 2124,
        chance = 8333,
    },
    {
        id = "white pearl",
        chance = 25000,
        maxCount = 15,
    },
    {
        id = "black pearl",
        chance = 11111,
        maxCount = 14,
    },
    {
        id = "small diamond",
        chance = 25000,
        maxCount = 5,
    },
    {
        id = "small sapphire",
        chance = 25000,
        maxCount = 10,
    },
    {
        id = "small emerald",
        chance = 25000,
        maxCount = 10,
    },
    {
        id = "small amethyst",
        chance = 25000,
        maxCount = 17,
    },
    {
        id = "talon",
        chance = 12500,
        maxCount = 7,
    },
    {
        id = "platinum coin",
        chance = 100000,
        maxCount = 69,
    },
    {
        id = "green gem",
        chance = 20000,
    },
    {
        id = "blue gem",
        chance = 14285,
    },
    {
        id = "might ring",
        chance = 12500,
    },
    {
        id = "stealth ring",
        chance = 12500,
    },
    {
        id = "strange symbol",
        chance = 11111,
    },
    {
        id = "life crystal",
        chance = 12500,
    },
    {
        id = 2178,
        chance = 20000,
    },
    {
        id = 2178,
        chance = 20000,
    },
    {
        id = "gold ring",
        chance = 20000,
    },
    {
        id = "ring of healing",
        chance = 20000,
    },
    {
        id = "twin axe",
        chance = 11111,
    },
    {
        id = "golden armor",
        chance = 8333,
    },
    {
        id = "magic plate armor",
        chance = 8333,
    },
    {
        id = "demon shield",
        chance = 12500,
    },
    {
        id = "golden boots",
        chance = 8333,
    },
    {
        id = "demon horn",
        chance = 33333,
        maxCount = 2,
    },
    {
        id = 6300,
        chance = 25000,
    },
    {
        id = "demonic essence",
        chance = 100000,
    },
    {
        id = "ruthless axe",
        chance = 14285,
    },
    {
        id = "assassin star",
        chance = 12500,
        maxCount = 44,
    },
    {
        id = "havoc blade",
        chance = 16666,
    },
    {
        id = "ravenwing",
        chance = 14285,
    },
    {
        id = "scrying crystal ball",
        chance = 20000,
    },
    {
        id = "great mana potion",
        chance = 20000,
    },
    {
        id = "great health potion",
        chance = 20000,
    },
    {
        id = "glacier kilt",
        chance = 8333,
    },
    {
        id = "great spirit potion",
        chance = 25000,
    },
    {
        id = "ultimate health potion",
        chance = 25000,
    },
    {
        id = "oceanborn leviathan armor",
        chance = 16666,
    },
    {
        id = "frozen plate",
        chance = 8333,
    },
    {
        id = "spellbook of warding",
        chance = 20000,
    },
    {
        id = "spellbook of mind control",
        chance = 11111,
    },
    {
        id = "spellbook of lost souls",
        chance = 16666,
    },
    {
        id = "spellscroll of prophecies",
        chance = 25000,
    },
    {
        id = "spellbook of dark mysteries",
        chance = 20000,
    },
}

mtype:register(monster)
