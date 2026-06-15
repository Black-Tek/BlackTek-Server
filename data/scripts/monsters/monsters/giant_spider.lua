local mtype = Game.createMonsterType("Giant Spider")
local monster = {}

monster.name = "Giant Spider"
monster.description = "a giant spider"
monster.experience = 900
monster.race = "venom"
monster.maxHealth = 1300
monster.health = 1300
monster.speed = 230
monster.manaCost = 0
monster.corpse = 5977
monster.outfit = { lookType = 38 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 70
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
        maxDamage = -300,
        -- NOTE: melee condition (poison=160); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 160000,
        },
    },
    {
        name = "poisonfield",
        interval = 2000,
        chance = 10,
        range = 7,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -40,
        maxDamage = -70,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_POISON,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 390,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Poison Spider", interval = 2000, chance = 10, max = 0},
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
        maxCount = 95,
    },
    {
        id = "poison arrow",
        chance = 12500,
        maxCount = 12,
    },
    {
        id = "plate armor",
        chance = 10000,
    },
    {
        id = "plate legs",
        chance = 8000,
    },
    {
        id = "two handed sword",
        chance = 5000,
    },
    {
        id = "steel helmet",
        chance = 5000,
    },
    {
        id = "strong health potion",
        chance = 3571,
    },
    {
        id = "spider silk",
        chance = 2000,
    },
    {
        id = "knight legs",
        chance = 870,
    },
    {
        id = 2169,
        chance = 710,
    },
    {
        id = "knight armor",
        chance = 500,
    },
    {
        id = "platinum amulet",
        chance = 280,
    },
    {
        id = "lightning headband",
        chance = 270,
    },
}

mtype:register(monster)
