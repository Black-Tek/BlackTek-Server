local mtype = Game.createMonsterType("Quara Constrictor")
local monster = {}

monster.name = "Quara Constrictor"
monster.description = "a quara constrictor"
monster.experience = 250
monster.race = "blood"
monster.maxHealth = 450
monster.health = 450
monster.speed = 380
monster.manaCost = 670
monster.corpse = 6065
monster.outfit = { lookType = 46 }
monster.runHealth = 30
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -150,
        -- NOTE: melee condition (poison=20); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 20000,
        },
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -50,
        maxDamage = -90,
        radius = 3,
        target = false,
        effect = CONST_ME_HITAREA,
    },
    {
        name = "quara constrictor freeze",
        interval = 2000,
        chance = 10,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -40,
        maxDamage = -70,
        radius = 4,
        target = false,
        effect = CONST_ME_ICEATTACK,
    },
    {
        name = "quara constrictor electrify",
        interval = 2000,
        chance = 10,
        range = 1,
    },
}
monster.defenses = {
    defense = 14,
    armor = 14,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Gaaahhh!", yell = false},
    {text = "Gluh! Gluh!", yell = false},
    {text = "Tssss!", yell = false},
    {text = "Boohaa!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 87662,
        maxCount = 100,
    },
    {
        id = 12443,
        chance = 14598,
    },
    {
        id = 2397,
        chance = 6951,
    },
    {
        id = 2670,
        chance = 5169,
    },
    {
        id = 2465,
        chance = 4804,
    },
    {
        id = 2150,
        chance = 3042,
    },
    {
        id = 5895,
        chance = 484,
    },
}

mtype:register(monster)
