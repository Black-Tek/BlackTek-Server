local mtype = Game.createMonsterType("Infernalist")
local monster = {}

monster.name = "Infernalist"
monster.description = "an infernalist"
monster.experience = 4000
monster.race = "blood"
monster.maxHealth = 3650
monster.health = 3650
monster.speed = 220
monster.manaCost = 0
monster.corpse = 20427
monster.outfit = {
    lookType = 130,
    lookHead = 78,
    lookBody = 76,
    lookLegs = 94,
    lookFeet = 39,
    lookAddons = 2,
}
monster.runHealth = 900
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
monster.staticAttackChance = 95
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
        minDamage = 0,
        maxDamage = -100,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 40,
        range = 7,
        minDamage = -65,
        maxDamage = -180,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_HITBYFIRE,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -90,
        maxDamage = -180,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -53,
        maxDamage = -120,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_ENERGYBALL,
        effect = CONST_ME_TELEPORT,
    },
    {
        name = "firefield",
        interval = 2000,
        chance = 15,
        range = 7,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        minDamage = -150,
        maxDamage = -250,
        length = 8,
        effect = CONST_ME_FIREATTACK,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 5,
        minDamage = -100,
        maxDamage = -150,
        radius = 2,
        target = false,
        effect = CONST_ME_EXPLOSIONAREA,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 60,
        maxDamage = 230,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 15,
        duration = 8000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 95},
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = -5},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = 5},
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 1
monster.summons = {
    {name = "fire elemental", interval = 2000, chance = 20, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Nothing will remain but your scorched bones!", yell = false},
    {text = "Some like it hot!", yell = false},
    {text = "It's cooking time!", yell = false},
    {text = "Feel the heat of battle!", yell = false},
}
monster.loot = {
    {
        id = "red tome",
        chance = 300,
    },
    {
        id = "piggy bank",
        chance = 220,
    },
    {
        id = "gold coin",
        chance = 56500,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 40000,
        maxCount = 47,
    },
    {
        id = "energy ring",
        chance = 1800,
    },
    {
        id = "skull staff",
        chance = 6500,
    },
    {
        id = "magic sulphur",
        chance = 600,
    },
    {
        id = "red piece of cloth",
        chance = 1420,
    },
    {
        id = "great mana potion",
        chance = 19700,
    },
    {
        id = "great health potion",
        chance = 1900,
    },
    {
        id = "small enchanted ruby",
        chance = 4250,
    },
    {
        id = "magma boots",
        chance = 300,
    },
    {
        id = "raspberry",
        chance = 8500,
        maxCount = 5,
    },
    {
        id = "spellbook of mind control",
        chance = 370,
    },
    {
        id = "royal tapestry",
        chance = 520,
    },
    {
        id = "black skull",
        chance = 820,
    },
    {
        id = "gold ingot",
        chance = 70,
    },
    {
        id = "crystal of power",
        chance = 220,
    },
}

mtype:register(monster)
