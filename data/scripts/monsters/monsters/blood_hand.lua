local mtype = Game.createMonsterType("Blood Hand")
local monster = {}

monster.name = "Blood Hand"
monster.description = "a blood hand"
monster.experience = 750
monster.race = "blood"
monster.maxHealth = 700
monster.health = 700
monster.speed = 220
monster.manaCost = 0
monster.corpse = 21257
monster.outfit = { lookType = 552 }
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
        maxDamage = -158,
        -- NOTE: melee condition (poison=80); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 80000,
        },
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 20,
        minDamage = -50,
        maxDamage = -100,
        radius = 4,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        radius = 4,
        target = true,
        speed = -600,
        duration = 15000,
        effect = CONST_ME_BLOCKHIT,
    },
    {
        name = "bleedcondition",
        interval = 2000,
        chance = 15,
        minDamage = -120,
        maxDamage = -160,
        radius = 6,
        target = false,
        effect = CONST_ME_HITAREA,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "healing",
        interval = 2000,
        chance = 20,
        minDamage = 70,
        maxDamage = 90,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "effect",
        interval = 2000,
        chance = 10,
        radius = 1,
        effect = CONST_ME_INSECTS,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 5},
    {type = COMBAT_ICEDAMAGE, percent = 5},
    {type = COMBAT_DEATHDAMAGE, percent = 5},
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
    {text = "Die, filth!", yell = false},
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
        maxCount = 30,
    },
    {
        id = "necrotic rod",
        chance = 3000,
    },
    {
        id = "boots of haste",
        chance = 210,
    },
    {
        id = "skull staff",
        chance = 130,
    },
    {
        id = "mystic turban",
        chance = 790,
    },
    {
        id = "white piece of cloth",
        chance = 840,
    },
    {
        id = "red piece of cloth",
        chance = 500,
    },
    {
        id = "noble axe",
        chance = 10,
    },
    {
        id = "strong mana potion",
        chance = 5590,
    },
    {
        id = "spellbook of enlightenment",
        chance = 790,
    },
    {
        id = "book of necromantic rituals",
        chance = 9340,
    },
    {
        id = "rough red gem",
        chance = 710,
    },
    {
        id = "lancet",
        chance = 10680,
    },
    {
        id = "horoscope",
        chance = 7950,
    },
    {
        id = "blood tincture in a vial",
        chance = 15460,
    },
    {
        id = "incantation notes",
        chance = 8820,
    },
    {
        id = "pieces of magic chalk",
        chance = 6120,
    },
}

mtype:register(monster)
