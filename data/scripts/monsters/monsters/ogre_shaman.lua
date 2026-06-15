local mtype = Game.createMonsterType("Ogre Shaman")
local monster = {}

monster.name = "Ogre Shaman"
monster.description = "an ogre shaman"
monster.experience = 625
monster.race = "blood"
monster.maxHealth = 800
monster.health = 800
monster.speed = 105
monster.manaCost = 0
monster.corpse = 24795
monster.outfit = { lookType = 859 }
monster.changeTarget = {
    interval = 2000,
    chance = 20,
}
monster.targetDistance = 5
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        attack = 50,
        skill = 50,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 11,
        range = 7,
        minDamage = -80,
        maxDamage = -110,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_EARTH,
        effect = CONST_ME_CARNIPHILA,
    },
    {
        name = "terra wave",
        interval = 2000,
        chance = 13,
        minDamage = -90,
        maxDamage = -200,
        effect = CONST_ME_SMALLPLANTS,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 11,
        range = 7,
        duration = 2000,
        shootEffect = CONST_ANI_EARTH,
        effect = CONST_ME_MAGIC_GREEN,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -20,
        maxDamage = -40,
    },
}
monster.defenses = {
    defense = 24,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 25,
        minDamage = 0,
        maxDamage = 0,
        effect = CONST_ME_MORTAREA,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "poison", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
}
monster.maxSummons = 1
monster.summons = {
    {name = "Terror Bird", interval = 2000, chance = 10, max = 1},
    {name = "Dworc Voodoomaster", interval = 2000, chance = 10, max = 1},
    {name = "Dworc Fleshhunter", interval = 2000, chance = 10, max = 1},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "Stay here, soul food!", yell = false},
    {text = "You more delicate than clomp!", yell = false},
    {text = "The spirits hunger!", yell = false},
    {text = "A sacrifice for Great Hunger!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 90,
    },
    {
        id = "strong mana potion",
        chance = 15650,
    },
    {
        id = "ogre ear stud",
        chance = 14010,
    },
    {
        id = "skull fetish",
        chance = 10920,
    },
    {
        id = "shamanic talisman",
        chance = 10350,
    },
    {
        id = "ogre nose ring",
        chance = 10040,
    },
    {
        id = 2229,
        chance = 9700,
    },
    {
        id = "prickly pear",
        chance = 9240,
        maxCount = 2,
    },
    {
        id = "broken shamanic staff",
        chance = 6680,
    },
    {
        id = "bone fetish",
        chance = 5080,
    },
    {
        id = "small ruby",
        chance = 3130,
    },
    {
        id = "black pearl",
        chance = 2600,
    },
    {
        id = "necrotic rod",
        chance = 2600,
    },
    {
        id = "wand of decay",
        chance = 2330,
    },
    {
        id = "waterskin",
        chance = 2060,
    },
    {
        id = "opal",
        chance = 1640,
        maxCount = 2,
    },
    {
        id = "onyx chip",
        chance = 1340,
        maxCount = 2,
    },
    {
        id = "shamanic mask",
        chance = 880,
    },
    {
        id = "ogre scepta",
        chance = 500,
    },
    {
        id = "didgeridoo",
        chance = 460,
    },
    {
        id = 3955,
        chance = 40,
    },
}

mtype:register(monster)
