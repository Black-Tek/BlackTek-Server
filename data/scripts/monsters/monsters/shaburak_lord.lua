local mtype = Game.createMonsterType("Shaburak Lord")
local monster = {}

monster.name = "Shaburak Lord"
monster.description = "a shaburak lord"
monster.experience = 1200
monster.race = "fire"
monster.maxHealth = 2100
monster.health = 2100
monster.speed = 230
monster.manaCost = 0
monster.corpse = 13958
monster.outfit = { lookType = 409 }
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
        maxDamage = -251,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -30,
        maxDamage = -75,
        radius = 6,
        target = false,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREATTACK,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        minDamage = -80,
        maxDamage = -170,
        length = 4,
        spread = 3,
        target = false,
        effect = CONST_ME_HITBYFIRE,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        minDamage = -140,
        maxDamage = -200,
        length = 4,
        target = false,
        effect = CONST_ME_FIREATTACK,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
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
        chance = 5,
        minDamage = 100,
        maxDamage = 150,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 50},
    {type = COMBAT_ICEDAMAGE, percent = 50},
    {type = COMBAT_EARTHDAMAGE, percent = -30},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "GREEN IS MEAN!", yell = true},
    {text = "WE RULE!", yell = true},
    {text = "POWER TO THE SHABURAK!", yell = true},
    {text = "DEATH TO THE ASKARAK!", yell = false},
    {text = "ONLY WE ARE TRUE DEMONS!", yell = false},
}
monster.loot = {
    {
        id = "small ruby",
        chance = 6120,
        maxCount = 5,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 78,
    },
    {
        id = "platinum coin",
        chance = 33333,
        maxCount = 2,
    },
    {
        id = "energy ring",
        chance = 1652,
    },
    {
        id = "wand of inferno",
        chance = 800,
    },
    {
        id = "steel boots",
        chance = 160,
    },
    {
        id = "brown mushroom",
        chance = 5346,
    },
    {
        id = "magic sulphur",
        chance = 620,
    },
    {
        id = "bullseye potion",
        chance = 400,
    },
    {
        id = "strong health potion",
        chance = 7000,
    },
    {
        id = "strong mana potion",
        chance = 7000,
    },
    {
        id = "magma coat",
        chance = 100,
    },
}

mtype:register(monster)
