local mtype = Game.createMonsterType("Elder Wyrm")
local monster = {}

monster.name = "Elder Wyrm"
monster.description = "an elder wyrm"
monster.experience = 2500
monster.race = "blood"
monster.maxHealth = 2700
monster.health = 2700
monster.speed = 280
monster.manaCost = 0
monster.corpse = 21283
monster.outfit = { lookType = 561 }
monster.runHealth = 250
monster.changeTarget = {
    interval = 4000,
    chance = 15,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -350,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 15,
        minDamage = -90,
        maxDamage = -150,
        radius = 4,
        target = true,
        effect = CONST_ME_TELEPORT,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 20,
        minDamage = -140,
        maxDamage = -250,
        radius = 5,
        target = false,
        effect = CONST_ME_PURPLEENERGY,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        minDamage = 0,
        maxDamage = -180,
        length = 8,
        effect = CONST_ME_BLOCKHIT,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        minDamage = -200,
        maxDamage = -300,
        length = 5,
        spread = 2,
        target = true,
        effect = CONST_ME_BLACKSMOKE,
    },
}
monster.defenses = {
    defense = 48,
    armor = 48,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 100,
        maxDamage = 150,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 75},
    {type = COMBAT_FIREDAMAGE, percent = 30},
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "lifedrain", combat = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "GRRR!", yell = false},
    {text = "GRROARR", yell = false},
}
monster.loot = {
    {
        id = "small diamond",
        chance = 4000,
        maxCount = 5,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 74,
    },
    {
        id = "platinum coin",
        chance = 25150,
        maxCount = 3,
    },
    {
        id = "crossbow",
        chance = 9690,
    },
    {
        id = "dragon ham",
        chance = 32420,
        maxCount = 2,
    },
    {
        id = "soul orb",
        chance = 5980,
    },
    {
        id = "dragonbone staff",
        chance = 100,
    },
    {
        id = "shadow sceptre",
        chance = 310,
    },
    {
        id = "strong health potion",
        chance = 17710,
    },
    {
        id = "strong mana potion",
        chance = 20930,
    },
    {
        id = "lightning pendant",
        chance = 520,
    },
    {
        id = "lightning boots",
        chance = 310,
    },
    {
        id = "lightning legs",
        chance = 930,
    },
    {
        id = "lightning robe",
        chance = 310,
    },
    {
        id = "composite hornbow",
        chance = 310,
    },
    {
        id = "focus cape",
        chance = 100,
    },
    {
        id = "wand of starstorm",
        chance = 410,
    },
    {
        id = "wand of draconia",
        chance = 2000,
    },
    {
        id = "shockwave amulet",
        chance = 100,
    },
    {
        id = "wyrm scale",
        chance = 15980,
    },
}

mtype:register(monster)
