local mtype = Game.createMonsterType("Nightmare Scion")
local monster = {}

monster.name = "Nightmare Scion"
monster.description = "a nightmare scion"
monster.experience = 1350
monster.race = "blood"
monster.maxHealth = 1400
monster.health = 1400
monster.speed = 440
monster.manaCost = 0
monster.corpse = 9919
monster.outfit = { lookType = 321 }
monster.runHealth = 300
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -140,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -115,
        maxDamage = -180,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -70,
        maxDamage = -130,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "healing",
        interval = 2000,
        chance = 5,
        minDamage = 60,
        maxDamage = 95,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Weeeheeheee!", yell = false},
    {text = "Pffffrrrrrrrrrrrr.", yell = false},
    {text = "Peak a boo, I killed you!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 97006,
        maxCount = 155,
    },
    {
        id = 2666,
        chance = 50392,
    },
    {
        id = 11223,
        chance = 8074,
    },
    {
        id = 11229,
        chance = 4977,
    },
    {
        id = 2152,
        chance = 1074,
        maxCount = 3,
    },
    {
        id = 2491,
        chance = 728,
    },
    {
        id = 7387,
        chance = 367,
    },
    {
        id = 8871,
        chance = 330,
    },
    {
        id = 6300,
        chance = 284,
    },
    {
        id = 6574,
        chance = 248,
    },
    {
        id = 7451,
        chance = 248,
    },
    {
        id = 9941,
        chance = 114,
    },
}

mtype:register(monster)
