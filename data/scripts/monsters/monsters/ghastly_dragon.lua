local mtype = Game.createMonsterType("Ghastly Dragon")
local monster = {}

monster.name = "Ghastly Dragon"
monster.description = "a ghastly dragon"
monster.experience = 4600
monster.race = "undead"
monster.maxHealth = 7800
monster.health = 7800
monster.speed = 320
monster.manaCost = 0
monster.corpse = 11362
monster.outfit = { lookType = 351 }
monster.runHealth = 366
monster.changeTarget = {
    interval = 4000,
    chance = 5,
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -603,
    },
    {
        name = "ghastly dragon curse",
        interval = 2000,
        chance = 5,
        range = 5,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        range = 5,
        minDamage = -920,
        maxDamage = -1280,
        target = true,
        effect = CONST_ME_BATS,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -80,
        maxDamage = -230,
        target = true,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        minDamage = -120,
        maxDamage = -250,
        length = 8,
        spread = 3,
        effect = CONST_ME_LOSEENERGY,
    },
    {
        name = "death",
        interval = 2000,
        chance = 15,
        minDamage = -110,
        maxDamage = -180,
        radius = 4,
        target = false,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        range = 7,
        target = true,
        speed = -800,
        duration = 30000,
        effect = CONST_ME_SMALLCLOUDS,
    },
}
monster.defenses = {
    defense = 35,
    armor = 35,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = 50},
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -15},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "EMBRACE MY GIFTS!", yell = true},
    {text = "I WILL FEAST ON YOUR SOUL!", yell = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 33725,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 33725,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 33725,
        maxCount = 66,
    },
    {
        id = "platinum coin",
        chance = 29840,
        maxCount = 2,
    },
    {
        id = "soul orb",
        chance = 12170,
    },
    {
        id = "demonic essence",
        chance = 8920,
    },
    {
        id = "great mana potion",
        chance = 30560,
        maxCount = 2,
    },
    {
        id = "terra legs",
        chance = 3130,
    },
    {
        id = "terra boots",
        chance = 9510,
    },
    {
        id = "great spirit potion",
        chance = 29460,
        maxCount = 2,
    },
    {
        id = "ultimate health potion",
        chance = 24700,
    },
    {
        id = 9810,
        chance = 180,
    },
    {
        id = "shiny stone",
        chance = 860,
    },
    {
        id = "guardian boots",
        chance = 200,
    },
    {
        id = "Zaoan armor",
        chance = 870,
    },
    {
        id = "Zaoan helmet",
        chance = 150,
    },
    {
        id = "Zaoan shoes",
        chance = 870,
    },
    {
        id = "Zaoan legs",
        chance = 1400,
    },
    {
        id = "drakinata",
        chance = 1470,
    },
    {
        id = "Zaoan sword",
        chance = 100,
    },
    {
        id = "twin hooks",
        chance = 15100,
    },
    {
        id = "Zaoan halberd",
        chance = 15020,
    },
    {
        id = "spellweaver's robe",
        chance = 690,
    },
    {
        id = "ghastly dragon head",
        chance = 6650,
    },
    {
        id = "undead heart",
        chance = 19830,
    },
    {
        id = "jade hat",
        chance = 810,
    },
}

mtype:register(monster)
