local mtype = Game.createMonsterType("Blightwalker")
local monster = {}

monster.name = "Blightwalker"
monster.description = "a blightwalker"
monster.experience = 5850
monster.race = "undead"
monster.maxHealth = 8900
monster.health = 8900
monster.speed = 250
monster.manaCost = 0
monster.corpse = 6354
monster.outfit = { lookType = 246 }
monster.runHealth = 800
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -490,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -220,
        maxDamage = -405,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        minDamage = -65,
        maxDamage = -135,
        radius = 4,
        target = false,
        effect = CONST_ME_MAGIC_GREEN,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        radius = 3,
        target = false,
        duration = 5000,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "blightwalker curse",
        interval = 2000,
        chance = 15,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        target = true,
        speed = -300,
        duration = 30000,
        shootEffect = CONST_ANI_POISON,
    },
}
monster.defenses = {
    defense = 50,
    armor = 50,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 50},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 50},
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -30},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "I can see you decaying!", yell = false},
    {text = "Let me taste your mortality!", yell = false},
    {text = "Your lifeforce is waning!", yell = false},
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
        maxCount = 97,
    },
    {
        id = "platinum coin",
        chance = 100000,
        maxCount = 5,
    },
    {
        id = "amulet of loss",
        chance = 120,
    },
    {
        id = "gold ring",
        chance = 1870,
    },
    {
        id = "hailstorm rod",
        chance = 10000,
    },
    {
        id = "garlic necklace",
        chance = 2050,
    },
    {
        id = "blank rune",
        chance = 26250,
        maxCount = 2,
    },
    {
        id = "golden sickle",
        chance = 350,
    },
    {
        id = "skull staff",
        chance = 1520,
    },
    {
        id = "scythe",
        chance = 3000,
    },
    {
        id = "bunch of wheat",
        chance = 50000,
    },
    {
        id = "soul orb",
        chance = 23720,
    },
    {
        id = 6300,
        chance = 1410,
    },
    {
        id = "demonic essence",
        chance = 28000,
    },
    {
        id = "assassin star",
        chance = 5900,
        maxCount = 10,
    },
    {
        id = "great mana potion",
        chance = 31360,
        maxCount = 3,
    },
    {
        id = 7632,
        chance = 4450,
    },
    {
        id = 7633,
        chance = 4450,
    },
    {
        id = "seeds",
        chance = 4300,
    },
    {
        id = "terra mantle",
        chance = 1050,
    },
    {
        id = "terra legs",
        chance = 2500,
    },
    {
        id = "ultimate health potion",
        chance = 14720,
        maxCount = 2,
    },
    {
        id = "gold ingot",
        chance = 5270,
    },
    {
        id = "bundle of cursed straw",
        chance = 15000,
    },
}

mtype:register(monster)
