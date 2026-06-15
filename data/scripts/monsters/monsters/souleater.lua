local mtype = Game.createMonsterType("Souleater")
local monster = {}

monster.name = "Souleater"
monster.description = "a souleater"
monster.experience = 1300
monster.race = "undead"
monster.maxHealth = 1100
monster.health = 1100
monster.speed = 200
monster.manaCost = 0
monster.corpse = 12631
monster.outfit = { lookType = 355 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
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
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -210,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 100,
        range = 7,
        minDamage = -50,
        maxDamage = -100,
        target = true,
        shootEffect = CONST_ANI_SMALLICE,
    },
    {
        name = "souleater drown",
        interval = 2000,
        chance = 10,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        minDamage = -100,
        maxDamage = -200,
        length = 4,
        spread = 3,
        target = false,
        effect = CONST_ME_SOUND_RED,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 25,
        minDamage = -30,
        maxDamage = -60,
        radius = 4,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "invisible",
        interval = 2000,
        chance = 5,
        duration = 3000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 120,
        maxDamage = 125,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = 50},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_PHYSICALDAMAGE, percent = 70},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "death", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Life is such a fickle thing!", yell = false},
    {text = "I will devour your soul.", yell = false},
    {text = "Souuuls!", yell = false},
    {text = "I will feed on you.", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 33400,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 33400,
        maxCount = 100,
    },
    {
        id = "platinum coin",
        chance = 49610,
        maxCount = 6,
    },
    {
        id = "necrotic rod",
        chance = 980,
    },
    {
        id = "wand of cosmic energy",
        chance = 910,
    },
    {
        id = "spirit container",
        chance = 140,
    },
    {
        id = 6300,
        chance = 300,
    },
    {
        id = "great mana potion",
        chance = 8000,
    },
    {
        id = "ultimate health potion",
        chance = 9400,
    },
    {
        id = "souleater trophy",
        chance = 20,
    },
    {
        id = "lizard essence",
        chance = 15250,
    },
    {
        id = "ectoplasmic sushi",
        chance = 2000,
    },
}

mtype:register(monster)
