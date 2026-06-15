local mtype = Game.createMonsterType("Blood Priest")
local monster = {}

monster.name = "Blood Priest"
monster.description = "a blood priest"
monster.experience = 900
monster.race = "blood"
monster.maxHealth = 820
monster.health = 820
monster.speed = 210
monster.manaCost = 0
monster.corpse = 21262
monster.outfit = { lookType = 553 }
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
        maxDamage = -80,
        -- NOTE: melee condition (poison=100); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 100000,
        },
    },
    {
        name = "death",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -60,
        maxDamage = -100,
        target = true,
        shootEffect = CONST_ANI_DEATH,
        effect = CONST_ME_SMALLCLOUDS,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        minDamage = -40,
        maxDamage = -60,
        radius = 4,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "manadrain",
        interval = 3000,
        chance = 10,
        range = 1,
        minDamage = -80,
        maxDamage = -130,
        length = 7,
        target = true,
        effect = CONST_ME_HITAREA,
    },
    {
        name = "bleedcondition",
        interval = 2000,
        chance = 5,
        range = 1,
        minDamage = -160,
        maxDamage = -290,
        radius = 1,
        target = true,
        effect = CONST_ME_DRAWBLOOD,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 20,
        minDamage = 80,
        maxDamage = 120,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 5},
    {type = COMBAT_ICEDAMAGE, percent = 5},
    {type = COMBAT_DEATHDAMAGE, percent = 5},
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Blood for the dark god!", yell = false},
}
monster.loot = {
    {
        id = "small ruby",
        chance = 3510,
        maxCount = 2,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 71,
    },
    {
        id = "boots of haste",
        chance = 120,
    },
    {
        id = "skull staff",
        chance = 180,
    },
    {
        id = "mystic turban",
        chance = 2170,
    },
    {
        id = "white piece of cloth",
        chance = 3040,
    },
    {
        id = "red piece of cloth",
        chance = 640,
    },
    {
        id = "strong mana potion",
        chance = 6000,
    },
    {
        id = "spellbook of warding",
        chance = 290,
    },
    {
        id = "spellbook of mind control",
        chance = 180,
    },
    {
        id = "underworld rod",
        chance = 470,
    },
    {
        id = "book of necromantic rituals",
        chance = 13550,
    },
    {
        id = "rough red gem",
        chance = 880,
    },
    {
        id = "lancet",
        chance = 14410,
    },
    {
        id = "horoscope",
        chance = 9270,
    },
    {
        id = "blood tincture in a vial",
        chance = 14460,
    },
    {
        id = "incantation notes",
        chance = 14660,
    },
    {
        id = "pieces of magic chalk",
        chance = 7770,
    },
}

mtype:register(monster)
