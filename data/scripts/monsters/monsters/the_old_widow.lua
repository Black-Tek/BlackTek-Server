local mtype = Game.createMonsterType("The Old Widow")
local monster = {}

monster.name = "The Old Widow"
monster.description = "The Old Widow"
monster.experience = 4200
monster.race = "venom"
monster.maxHealth = 3200
monster.health = 3200
monster.speed = 240
monster.manaCost = 0
monster.corpse = 5977
monster.outfit = { lookType = 208 }
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
    canWalkOnFire = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -700,
        -- NOTE: melee condition (poison=160); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 160000,
        },
    },
    {
        name = "earth",
        interval = 1000,
        chance = 15,
        range = 7,
        minDamage = -200,
        maxDamage = -350,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 20,
        range = 7,
        speed = -850,
        duration = 25000,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "poisonfield",
        interval = 1000,
        chance = 10,
        range = 7,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_POISON,
    },
}
monster.defenses = {
    defense = 45,
    armor = 45,
    {
        name = "healing",
        interval = 1000,
        chance = 17,
        minDamage = 225,
        maxDamage = 275,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 8,
        speed = 345,
        duration = 6000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_PHYSICALDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "giant spider", interval = 1000, chance = 13, max = 0},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 99500,
        maxCount = 100,
    },
    {
        id = "platinum coin",
        chance = 98888,
        maxCount = 10,
    },
    {
        id = "spider silk",
        chance = 87000,
        maxCount = 3,
    },
    {
        id = "steel helmet",
        chance = 77000,
    },
    {
        id = "great health potion",
        chance = 67000,
        maxCount = 4,
    },
    {
        id = "knight armor",
        chance = 42000,
    },
    {
        id = "energy ring",
        chance = 33333,
    },
    {
        id = "stealth ring",
        chance = 33333,
    },
    {
        id = 2169,
        chance = 33333,
    },
    {
        id = "knight legs",
        chance = 26000,
    },
    {
        id = "platinum amulet",
        chance = 21000,
    },
    {
        id = "sweet smelling bait",
        chance = 21000,
    },
    {
        id = "spool of yarn",
        chance = 10000,
    },
    {
        id = "dreaded cleaver",
        chance = 2800,
    },
    {
        id = "bloody edge",
        chance = 1120,
    },
}

mtype:register(monster)
