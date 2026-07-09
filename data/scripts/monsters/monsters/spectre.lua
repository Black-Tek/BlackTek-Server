local mtype = Game.createMonsterType("Spectre")
local monster = {}

monster.name = "Spectre"
monster.description = "a spectre"
monster.experience = 2100
monster.race = "undead"
monster.maxHealth = 1350
monster.health = 1350
monster.speed = 230
monster.manaCost = 0
monster.corpse = 6348
monster.outfit = {
    lookType = 235,
    lookHead = 20,
}
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
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -308,
        -- NOTE: melee condition (poison=300); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 300000,
        },
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 15,
        radius = 4,
        target = false,
        duration = 6000,
        effect = CONST_ME_SOUND_PURPLE,
    },
    {
        name = "spectre drown",
        interval = 2000,
        chance = 15,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -100,
        maxDamage = -400,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -300,
        maxDamage = -550,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 35,
    armor = 35,
    {
        name = "healing",
        interval = 2000,
        chance = 25,
        minDamage = 100,
        maxDamage = 250,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 370,
        maxDamage = 700,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 290,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 90},
    {type = COMBAT_ICEDAMAGE, percent = 1},
    {type = COMBAT_FIREDAMAGE, percent = -8},
    {type = COMBAT_ENERGYDAMAGE, percent = -8},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "earth", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "drunk", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Revenge ... is so ... sweet!", yell = false},
    {text = "Life...force! Feed me your... lifeforce!", yell = false},
}
monster.loot = {
    {
        id = 2071,
        chance = 9620,
    },
    {
        id = "silver brooch",
        chance = 850,
    },
    {
        id = "silver brooch",
        chance = 110,
    },
    {
        id = 2136,
        chance = 110,
    },
    {
        id = "gold coin",
        chance = 33000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 33000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 33000,
        maxCount = 97,
    },
    {
        id = "platinum coin",
        chance = 3850,
        maxCount = 7,
    },
    {
        id = "stealth ring",
        chance = 190,
    },
    {
        id = "wand of cosmic energy",
        chance = 9800,
    },
    {
        id = "blank rune",
        chance = 30310,
        maxCount = 2,
    },
    {
        id = "white piece of cloth",
        chance = 3800,
    },
    {
        id = "soul orb",
        chance = 6005,
    },
    {
        id = 6300,
        chance = 280,
    },
    {
        id = "demonic essence",
        chance = 6270,
    },
    {
        id = "relic sword",
        chance = 700,
    },
    {
        id = "great mana potion",
        chance = 920,
    },
    {
        id = "shiny stone",
        chance = 1000,
    },
}

mtype:register(monster)
