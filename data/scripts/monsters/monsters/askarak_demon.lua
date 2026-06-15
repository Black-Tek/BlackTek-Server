local mtype = Game.createMonsterType("Askarak Demon")
local monster = {}

monster.name = "Askarak Demon"
monster.description = "an askarak demon"
monster.experience = 900
monster.race = "venom"
monster.maxHealth = 1500
monster.health = 1500
monster.speed = 220
monster.manaCost = 0
monster.corpse = 13815
monster.outfit = { lookType = 420 }
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
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -143,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -20,
        maxDamage = -60,
        radius = 6,
        target = false,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        minDamage = -75,
        maxDamage = -140,
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
        maxDamage = -170,
        length = 4,
        target = false,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        radius = 1,
        target = true,
        speed = -600,
        duration = 15000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 50},
    {type = COMBAT_ICEDAMAGE, percent = 50},
    {type = COMBAT_FIREDAMAGE, percent = -30},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "POWER TO THE ASKARAK!", yell = false},
    {text = "GREEN WILL RULE!", yell = false},
    {text = "GREEN IS MEAN!", yell = false},
    {text = "ONLY WE ARE TRUE DEMONS!", yell = false},
    {text = "RED IS MAD", yell = false},
}
monster.loot = {
    {
        id = "piggy bank",
        chance = 1052,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 40000,
        maxCount = 35,
    },
    {
        id = "small emerald",
        chance = 6250,
        maxCount = 4,
    },
    {
        id = "energy ring",
        chance = 961,
    },
    {
        id = "brown mushroom",
        chance = 3846,
        maxCount = 5,
    },
    {
        id = "magic sulphur",
        chance = 102,
    },
    {
        id = "assassin star",
        chance = 4761,
        maxCount = 5,
    },
    {
        id = "mastermind potion",
        chance = 431,
    },
    {
        id = "strong health potion",
        chance = 5263,
    },
    {
        id = "strong mana potion",
        chance = 5263,
    },
    {
        id = "terra legs",
        chance = 123,
    },
    {
        id = "springsprout rod",
        chance = 512,
    },
}

mtype:register(monster)
