local mtype = Game.createMonsterType("Dreadbeast")
local monster = {}

monster.name = "Dreadbeast"
monster.description = "a dreadbeast"
monster.experience = 250
monster.race = "undead"
monster.maxHealth = 800
monster.health = 800
monster.speed = 136
monster.manaCost = 800
monster.corpse = 6030
monster.outfit = { lookType = 101 }
monster.changeTarget = {
    interval = 60000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = true,
    pushable = false,
    canPushItems = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -50,
        -- NOTE: melee condition (poison=100); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 100000,
        },
    },
    {
        name = "drown",
        interval = 4000,
        chance = 30,
        range = 7,
        minDamage = -70,
        maxDamage = -90,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_LOSEENERGY,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 20,
        minDamage = -10,
        maxDamage = -80,
        radius = 3,
        target = false,
        effect = CONST_ME_PURPLEENERGY,
    },
}
monster.defenses = {
    defense = 40,
    armor = 40,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 35,
        maxDamage = 65,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -50},
    {type = COMBAT_FIREDAMAGE, percent = 55},
    {type = COMBAT_ICEDAMAGE, percent = 40},
    {type = COMBAT_PHYSICALDAMAGE, percent = 30},
    {type = COMBAT_ENERGYDAMAGE, percent = 15},
    {type = COMBAT_DROWNDAMAGE, percent = 75},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 11690,
        maxCount = 88,
    },
    {
        id = 2230,
        chance = 8230,
    },
    {
        id = "plate armor",
        chance = 2810,
    },
    {
        id = 2229,
        chance = 2810,
    },
    {
        id = 2231,
        chance = 1950,
    },
    {
        id = "bone club",
        chance = 1520,
    },
    {
        id = "bone shield",
        chance = 1520,
    },
    {
        id = "health potion",
        chance = 870,
    },
    {
        id = "green mushroom",
        chance = 650,
    },
    {
        id = "hardened bone",
        chance = 650,
    },
}

mtype:register(monster)
