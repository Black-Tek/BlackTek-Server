local mtype = Game.createMonsterType("Horestis")
local monster = {}

monster.name = "Horestis"
monster.description = "Horestis"
monster.experience = 3500
monster.race = "undead"
monster.maxHealth = 6000
monster.health = 6000
monster.speed = 276
monster.manaCost = 0
monster.corpse = 6031
monster.outfit = {
    lookType = 88,
    lookHead = 20,
    lookBody = 30,
    lookLegs = 40,
    lookFeet = 50,
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
        maxDamage = -592,
        -- NOTE: melee condition (poison=165); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 165000,
        },
    },
    {
        name = "lifedrain",
        interval = 1000,
        chance = 8,
        range = 1,
        minDamage = -300,
        maxDamage = -500,
    },
    {
        name = "earth",
        interval = 1000,
        chance = 12,
        range = 7,
        minDamage = -300,
        maxDamage = -500,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 25,
        range = 7,
        speed = -850,
        duration = 50000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 30,
    armor = 25,
    {
        name = "healing",
        interval = 1000,
        chance = 20,
        minDamage = 200,
        maxDamage = 400,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 1000,
        chance = 7,
        duration = 2000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "outfit",
        interval = 1000,
        chance = 3,
        duration = 6000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 5},
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Feel my eternal wrath!", yell = false},
    {text = "I might be dead but I'm not gone!", yell = false},
    {text = "Receive a lesson of the past!", yell = false},
}
monster.loot = {
    {
        id = "silver brooch",
        chance = 12500,
    },
    {
        id = "gold coin",
        chance = 17000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 17000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 17000,
        maxCount = 43,
    },
    {
        id = "platinum coin",
        chance = 33333,
        maxCount = 5,
    },
    {
        id = "scarab coin",
        chance = 14285,
        maxCount = 5,
    },
    {
        id = "pharaoh sword",
        chance = 5000,
    },
    {
        id = "twin axe",
        chance = 2500,
    },
    {
        id = "great mana potion",
        chance = 10000,
        maxCount = 3,
    },
    {
        id = "great health potion",
        chance = 5000,
        maxCount = 4,
    },
    {
        id = 9813,
        chance = 2500,
    },
    {
        id = "mini mummy",
        chance = 2500,
    },
    {
        id = 13472,
        chance = 16666,
    },
    {
        id = "scorpion sceptre",
        chance = 100000,
    },
    {
        id = 13739,
        chance = 12500,
    },
}

mtype:register(monster)
