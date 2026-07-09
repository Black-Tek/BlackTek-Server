local mtype = Game.createMonsterType("Shaburak Prince")
local monster = {}

monster.name = "Shaburak Prince"
monster.description = "a shaburak Prince"
monster.experience = 1700
monster.race = "fire"
monster.maxHealth = 2600
monster.health = 2600
monster.speed = 240
monster.manaCost = 0
monster.corpse = 13969
monster.outfit = { lookType = 418 }
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
        maxDamage = -319,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -40,
        maxDamage = -90,
        radius = 6,
        target = false,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREATTACK,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        minDamage = -100,
        maxDamage = -200,
        length = 4,
        spread = 3,
        target = false,
        effect = CONST_ME_FIREATTACK,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        minDamage = -150,
        maxDamage = -250,
        length = 4,
        target = false,
        effect = CONST_ME_HITBYFIRE,
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
    {
        name = "healing",
        interval = 2000,
        chance = 15,
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
        chance = 12500,
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
        maxCount = 4,
    },
    {
        id = "stealth ring",
        chance = 5000,
    },
    {
        id = "wand of inferno",
        chance = 892,
    },
    {
        id = "steel boots",
        chance = 89,
    },
    {
        id = "brown mushroom",
        chance = 4545,
    },
    {
        id = "magic sulphur",
        chance = 534,
    },
    {
        id = "butcher's axe",
        chance = 357,
    },
    {
        id = "bullseye potion",
        chance = 714,
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
        id = "magma coat",
        chance = 714,
    },
    {
        id = "demonic finger",
        chance = 178,
    },
}

mtype:register(monster)
