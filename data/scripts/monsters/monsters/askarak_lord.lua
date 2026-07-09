local mtype = Game.createMonsterType("Askarak Lord")
local monster = {}

monster.name = "Askarak Lord"
monster.description = "an askarak lord"
monster.experience = 1200
monster.race = "venom"
monster.maxHealth = 2100
monster.health = 2100
monster.speed = 230
monster.manaCost = 0
monster.corpse = 13956
monster.outfit = { lookType = 410 }
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
        maxDamage = -186,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -40,
        maxDamage = -80,
        radius = 6,
        target = false,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        minDamage = -95,
        maxDamage = -180,
        length = 4,
        spread = 3,
        target = false,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        minDamage = -130,
        maxDamage = -180,
        length = 4,
        target = false,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        radius = 1,
        target = true,
        speed = -650,
        duration = 15000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 120,
        maxDamage = 160,
        effect = CONST_ME_MAGIC_BLUE,
    },
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
    {text = "WE RULE!", yell = false},
    {text = "RED IS MAD", yell = false},
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
        maxCount = 90,
    },
    {
        id = "small emerald",
        chance = 5882,
        maxCount = 5,
    },
    {
        id = "platinum coin",
        chance = 35333,
        maxCount = 2,
    },
    {
        id = "energy ring",
        chance = 606,
    },
    {
        id = "brown mushroom",
        chance = 5263,
        maxCount = 5,
    },
    {
        id = "magic sulphur",
        chance = 620,
    },
    {
        id = "assassin star",
        chance = 70,
        maxCount = 5,
    },
    {
        id = "dreaded cleaver",
        chance = 30,
    },
    {
        id = "mastermind potion",
        chance = 750,
    },
    {
        id = "strong health potion",
        chance = 6250,
    },
    {
        id = "strong mana potion",
        chance = 7692,
    },
    {
        id = "terra mantle",
        chance = 70,
    },
    {
        id = "springsprout rod",
        chance = 1010,
    },
}

mtype:register(monster)
