local mtype = Game.createMonsterType("Walker")
local monster = {}

monster.name = "Walker"
monster.description = "a walker"
monster.experience = 2200
monster.race = "venom"
monster.maxHealth = 3000
monster.health = 3000
monster.speed = 300
monster.manaCost = 0
monster.corpse = 23363
monster.outfit = { lookType = 605 }
monster.changeTarget = {
    interval = 5000,
    chance = 5,
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -175,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 17,
        minDamage = -125,
        maxDamage = -245,
        length = 8,
        effect = CONST_ME_EXPLOSIONHIT,
    },
}
monster.defenses = {
    defense = 47,
    armor = 47,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 35},
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_DEATHDAMAGE, percent = 15},
    {type = COMBAT_ICEDAMAGE, percent = 5},
    {type = COMBAT_EARTHDAMAGE, percent = 50},
    {type = COMBAT_HOLYDAMAGE, percent = 40},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "paralyze", condition = true},
    {type = "lifedrain", combat = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "?", yell = false},
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
        maxCount = 99,
    },
    {
        id = "platinum coin",
        chance = 52000,
        maxCount = 3,
    },
    {
        id = "metal toe",
        chance = 20000,
    },
    {
        id = "small topaz",
        chance = 10000,
        maxCount = 6,
    },
    {
        id = "great spirit potion",
        chance = 8000,
    },
    {
        id = "great health potion",
        chance = 8000,
    },
    {
        id = "small amethyst",
        chance = 4500,
        maxCount = 3,
    },
    {
        id = "small emerald",
        chance = 4500,
        maxCount = 3,
    },
    {
        id = "metal spats",
        chance = 3500,
    },
    {
        id = "gearwheel chain",
        chance = 2200,
    },
    {
        id = "crystal mace",
        chance = 670,
    },
    {
        id = "steel boots",
        chance = 520,
    },
}

mtype:register(monster)
