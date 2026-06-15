local mtype = Game.createMonsterType("Koshei The Deathless")
local monster = {}

monster.name = "Koshei The Deathless"
monster.description = "Koshei the Deathless"
monster.experience = 0
monster.race = "undead"
monster.maxHealth = 3000
monster.health = 3000
monster.speed = 390
monster.manaCost = 0
monster.corpse = 6028
monster.outfit = {
    lookType = 99,
    lookHead = 95,
    lookBody = 116,
    lookLegs = 119,
    lookFeet = 115,
}
monster.changeTarget = {
    interval = 5000,
    chance = 8,
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -170,
        -- NOTE: melee condition (poison=300); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 300000,
        },
    },
    {
        name = "lifedrain",
        interval = 3000,
        chance = 9,
        range = 1,
        minDamage = -60,
        maxDamage = -250,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "lifedrain",
        interval = 1000,
        chance = 11,
        minDamage = -70,
        maxDamage = -135,
        radius = 3,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "death",
        interval = 2000,
        chance = 9,
        minDamage = -50,
        maxDamage = -140,
        length = 8,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "cursecondition",
        interval = 3000,
        chance = 15,
        range = 1,
        minDamage = -54,
        maxDamage = -54,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        speed = -900,
        duration = 30000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "healing",
        interval = 1000,
        chance = 30,
        minDamage = 150,
        maxDamage = 300,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 80},
    {type = COMBAT_HOLYDAMAGE, percent = -15},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 1
monster.summons = {
    {name = "bonebeast", interval = 1000, chance = 16, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Your pain will be beyond imagination!", yell = false},
    {text = "You can't defeat me! I will ressurect and take your soul!", yell = false},
    {text = "Death is my ally!", yell = false},
    {text = "Welcome to my domain, visitor!", yell = false},
    {text = "You will be my toy on the other side!", yell = false},
    {text = "What a disgusting smell of life!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 80000,
        maxCount = 12,
    },
    {
        id = "stealth ring",
        chance = 888,
    },
    {
        id = "platinum amulet",
        chance = 1666,
    },
    {
        id = 2175,
        chance = 10000,
    },
    {
        id = "gold ring",
        chance = 900,
    },
    {
        id = 2237,
        chance = 10000,
    },
    {
        id = 2401,
        chance = 10000,
    },
    {
        id = "castle shield",
        chance = 588,
    },
    {
        id = "blue robe",
        chance = 709,
    },
    {
        id = "lich staff",
        chance = 1000,
    },
    {
        id = "lightning boots",
        chance = 688,
    },
}

mtype:register(monster)
