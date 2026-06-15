local mtype = Game.createMonsterType("Gravedigger")
local monster = {}

monster.name = "Gravedigger"
monster.description = "a gravedigger"
monster.experience = 950
monster.race = "blood"
monster.maxHealth = 1500
monster.health = 1500
monster.speed = 230
monster.manaCost = 0
monster.corpse = 21279
monster.outfit = { lookType = 558 }
monster.runHealth = 200
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
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -320,
        -- NOTE: melee condition (poison=180); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 180000,
        },
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        range = 1,
        minDamage = -40,
        maxDamage = -250,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "death",
        interval = 2000,
        chance = 15,
        range = 1,
        minDamage = -175,
        maxDamage = -300,
        shootEffect = CONST_ANI_DEATH,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        radius = 5,
        target = false,
        duration = 4000,
        effect = CONST_ME_SMALLCLOUDS,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "invisible",
        interval = 2000,
        chance = 15,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 20,
        minDamage = 100,
        maxDamage = 250,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 420,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = -5},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "High Five!", yell = false},
    {text = "scrabble", yell = false},
    {text = "Put it there!", yell = false},
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
        maxCount = 37,
    },
    {
        id = "platinum coin",
        chance = 24470,
    },
    {
        id = "yellow gem",
        chance = 800,
    },
    {
        id = "wand of inferno",
        chance = 5590,
    },
    {
        id = "sudden death rune",
        chance = 7300,
        maxCount = 9,
    },
    {
        id = "skull staff",
        chance = 130,
    },
    {
        id = "mysterious voodoo skull",
        chance = 100,
    },
    {
        id = "hardened bone",
        chance = 50,
    },
    {
        id = 6300,
        chance = 800,
    },
    {
        id = "strong health potion",
        chance = 2260,
        maxCount = 2,
    },
    {
        id = "strong mana potion",
        chance = 3600,
        maxCount = 2,
    },
    {
        id = "lump of dirt",
        chance = 100,
    },
    {
        id = "unholy bone",
        chance = 9570,
    },
    {
        id = "pile of grave earth",
        chance = 6650,
    },
    {
        id = "safety pin",
        chance = 6000,
    },
}

mtype:register(monster)
