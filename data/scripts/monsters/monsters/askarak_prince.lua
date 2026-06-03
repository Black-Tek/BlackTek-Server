local mtype = Game.createMonsterType("Askarak Prince")
local monster = {}

monster.name = "Askarak Prince"
monster.description = "an askarak prince"
monster.experience = 1700
monster.race = "venom"
monster.maxHealth = 2600
monster.health = 2600
monster.speed = 240
monster.manaCost = 0
monster.corpse = 13957
monster.outfit = { lookType = 419 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
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
        maxDamage = -353,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -70,
        maxDamage = -250,
        radius = 6,
        target = false,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        minDamage = -100,
        maxDamage = -200,
        length = 4,
        spread = 3,
        target = false,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        radius = 1,
        target = true,
        speed = -700,
        duration = 15000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 50},
    {type = COMBAT_ICEDAMAGE, percent = 50},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "DEATH TO THE SHABRUAK!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 86,
    },
    {
        id = "small emerald",
        chance = 12500,
        maxCount = 5,
    },
    {
        id = "platinum coin",
        chance = 35333,
        maxCount = 4,
    },
    {
        id = "stealth ring",
        chance = 4545,
    },
    {
        id = "giant sword",
        chance = 320,
    },
    {
        id = "brown mushroom",
        chance = 6666,
        maxCount = 5,
    },
    {
        id = "magic sulphur",
        chance = 740,
    },
    {
        id = "mastermind potion",
        chance = 1315,
    },
    {
        id = "strong health potion",
        chance = 14285,
    },
    {
        id = "strong mana potion",
        chance = 14285,
    },
    {
        id = "terra mantle",
        chance = 840,
    },
    {
        id = "springsprout rod",
        chance = 950,
    },
    {
        id = "demonic finger",
        chance = 420,
    },
}

mtype:register(monster)
