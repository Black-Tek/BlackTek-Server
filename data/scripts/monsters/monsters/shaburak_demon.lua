local mtype = Game.createMonsterType("Shaburak Demon")
local monster = {}

monster.name = "Shaburak Demon"
monster.description = "a shaburak demon"
monster.experience = 900
monster.race = "fire"
monster.maxHealth = 1500
monster.health = 1500
monster.speed = 220
monster.manaCost = 0
monster.corpse = 13814
monster.outfit = { lookType = 417 }
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
        maxDamage = -113,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -20,
        maxDamage = -60,
        radius = 6,
        target = false,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREATTACK,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        minDamage = -70,
        maxDamage = -140,
        length = 4,
        spread = 3,
        target = false,
        effect = CONST_ME_HITBYFIRE,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        minDamage = -130,
        maxDamage = -170,
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
        speed = -600,
        duration = 15000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
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
}
monster.loot = {
    {
        id = "piggy bank",
        chance = 1030,
    },
    {
        id = "small ruby",
        chance = 4761,
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
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 50,
    },
    {
        id = "energy ring",
        chance = 1052,
    },
    {
        id = "wand of inferno",
        chance = 564,
    },
    {
        id = "brown mushroom",
        chance = 3846,
    },
    {
        id = "magic sulphur",
        chance = 81,
    },
    {
        id = "royal spear",
        chance = 10000,
        maxCount = 6,
    },
    {
        id = "bullseye potion",
        chance = 628,
    },
    {
        id = "strong health potion",
        chance = 4761,
    },
    {
        id = "strong mana potion",
        chance = 5000,
    },
    {
        id = "magma legs",
        chance = 155,
    },
}

mtype:register(monster)
