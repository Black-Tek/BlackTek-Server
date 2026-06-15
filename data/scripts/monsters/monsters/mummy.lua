local mtype = Game.createMonsterType("Mummy")
local monster = {}

monster.name = "Mummy"
monster.description = "a mummy"
monster.experience = 150
monster.race = "undead"
monster.maxHealth = 240
monster.health = 240
monster.speed = 150
monster.manaCost = 0
monster.corpse = 6004
monster.outfit = { lookType = 65 }
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
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -85,
        -- NOTE: melee condition (poison=4); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 4000,
        },
    },
    {
        name = "death",
        interval = 2000,
        chance = 20,
        range = 1,
        minDamage = -30,
        maxDamage = -40,
        target = true,
        effect = CONST_ME_SMALLCLOUDS,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        target = true,
        speed = -226,
        duration = 10000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "I will ssswallow your sssoul!", yell = false},
    {text = "Mort ulhegh dakh visss.", yell = false},
    {text = "Flesssh to dussst!", yell = false},
    {text = "I will tassste life again!", yell = false},
    {text = "Ahkahra exura belil mort!", yell = false},
    {text = "Yohag Sssetham!", yell = false},
}
monster.loot = {
    {
        id = 2124,
        chance = 1500,
    },
    {
        id = "silver brooch",
        chance = 4000,
    },
    {
        id = "black pearl",
        chance = 1000,
    },
    {
        id = "gold coin",
        chance = 38000,
        maxCount = 80,
    },
    {
        id = "strange talisman",
        chance = 5000,
    },
    {
        id = 2162,
        chance = 5800,
    },
    {
        id = "silver amulet",
        chance = 100,
    },
    {
        id = "poison dagger",
        chance = 450,
    },
    {
        id = "black shield",
        chance = 170,
    },
    {
        id = "worm",
        chance = 19000,
        maxCount = 3,
    },
    {
        id = "yellow piece of cloth",
        chance = 900,
    },
    {
        id = "gauze bandage",
        chance = 10000,
    },
    {
        id = "mini mummy",
        chance = 10,
    },
    {
        id = "flask of embalming fluid",
        chance = 11690,
    },
}

mtype:register(monster)
