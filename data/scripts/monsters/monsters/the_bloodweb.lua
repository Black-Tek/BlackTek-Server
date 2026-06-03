local mtype = Game.createMonsterType("The Bloodweb")
local monster = {}

monster.name = "The Bloodweb"
monster.description = "The Bloodweb"
monster.experience = 1450
monster.race = "undead"
monster.maxHealth = 1750
monster.health = 1750
monster.speed = 340
monster.manaCost = 0
monster.corpse = 7344
monster.outfit = { lookType = 263 }
monster.changeTarget = {
    interval = 4000,
    chance = 8,
}
monster.targetDistance = 1
monster.staticAttackChance = 60
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canWalkOnEnergy = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -250,
        -- NOTE: melee condition (poison=8); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 8000,
        },
    },
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        range = 7,
        radius = 7,
        target = false,
        speed = -850,
        duration = 8000,
        effect = CONST_ME_POFF,
    },
    {
        name = "energy",
        interval = 1000,
        chance = 25,
        range = 7,
        minDamage = -60,
        maxDamage = -150,
        target = true,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_ENERGYHIT,
    },
}
monster.defenses = {
    defense = 20,
    armor = 25,
    {
        name = "speed",
        interval = 3000,
        chance = 40,
        speed = 380,
        duration = 80000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "Screeech!", yell = false},
}
monster.loot = {
    {
        id = 2152,
        chance = 100000,
        maxCount = 9,
    },
    {
        id = 7441,
        chance = 99231,
    },
    {
        id = 7589,
        chance = 70769,
    },
    {
        id = 5879,
        chance = 65385,
    },
    {
        id = 7902,
        chance = 28462,
    },
    {
        id = 7896,
        chance = 26154,
    },
    {
        id = 11306,
        chance = 20769,
    },
    {
        id = 7437,
        chance = 12308,
    },
    {
        id = 2476,
        chance = 8462,
    },
    {
        id = 5801,
        chance = 5385,
    },
    {
        id = 2477,
        chance = 5385,
    },
    {
        id = 7290,
        chance = 1538,
    },
    {
        id = 2169,
        chance = 1538,
    },
}

mtype:register(monster)
