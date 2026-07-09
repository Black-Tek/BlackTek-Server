local mtype = Game.createMonsterType("Medusa")
local monster = {}

monster.name = "Medusa"
monster.description = "a medusa"
monster.experience = 4050
monster.race = "blood"
monster.maxHealth = 4500
monster.health = 4500
monster.speed = 280
monster.manaCost = 0
monster.corpse = 10524
monster.outfit = { lookType = 330 }
monster.runHealth = 600
monster.changeTarget = {
    interval = 4000,
    chance = 20,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
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
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -450,
        -- NOTE: melee condition (poison=840); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 840000,
        },
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -75,
        maxDamage = -150,
        target = true,
        shootEffect = CONST_ANI_EARTH,
        effect = CONST_ME_CARNIPHILA,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -75,
        maxDamage = -150,
        target = true,
        shootEffect = CONST_ANI_EARTH,
        effect = CONST_ME_CARNIPHILA,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -75,
        maxDamage = -150,
        target = true,
        shootEffect = CONST_ANI_EARTH,
        effect = CONST_ME_CARNIPHILA,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -75,
        maxDamage = -150,
        target = true,
        shootEffect = CONST_ANI_EARTH,
        effect = CONST_ME_CARNIPHILA,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -75,
        maxDamage = -150,
        target = true,
        shootEffect = CONST_ANI_EARTH,
        effect = CONST_ME_CARNIPHILA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        minDamage = -250,
        maxDamage = -500,
        length = 8,
        spread = 3,
        effect = CONST_ME_CARNIPHILA,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 25,
        radius = 7,
        target = true,
        speed = -900,
        effect = CONST_ME_POFF,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 1,
        range = 7,
        target = true,
        duration = 3000,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 25,
        minDamage = 150,
        maxDamage = 300,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "lifedrain", combat = true},
    {type = "drown", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "You will make ssuch a fine ssstatue!", yell = false},
    {text = "There isss no chhhanccce of esscape", yell = false},
    {text = "Jusssst look at me!", yell = false},
    {text = "Are you tired or why are you moving thhat sslow<chuckle>", yell = false},
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
        maxCount = 90,
    },
    {
        id = "small emerald",
        chance = 3770,
        maxCount = 4,
    },
    {
        id = "platinum coin",
        chance = 74810,
        maxCount = 6,
    },
    {
        id = "knight armor",
        chance = 1840,
    },
    {
        id = "medusa shield",
        chance = 3040,
    },
    {
        id = "titan axe",
        chance = 1160,
    },
    {
        id = "great mana potion",
        chance = 10000,
        maxCount = 2,
    },
    {
        id = "terra mantle",
        chance = 870,
    },
    {
        id = "terra legs",
        chance = 420,
    },
    {
        id = "terra amulet",
        chance = 4060,
    },
    {
        id = "ultimate health potion",
        chance = 9290,
        maxCount = 2,
    },
    {
        id = 9810,
        chance = 500,
    },
    {
        id = "sacred tree amulet",
        chance = 850,
    },
    {
        id = "strand of medusa hair",
        chance = 9900,
    },
}

mtype:register(monster)
