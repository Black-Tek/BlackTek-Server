local mtype = Game.createMonsterType("Stone Devourer")
local monster = {}

monster.name = "Stone Devourer"
monster.description = "a stone devourer"
monster.experience = 2900
monster.race = "undead"
monster.maxHealth = 4200
monster.health = 4200
monster.speed = 250
monster.manaCost = 0
monster.corpse = 18375
monster.outfit = { lookType = 486 }
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -990,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -230,
        maxDamage = -460,
        target = true,
        shootEffect = CONST_ANI_SMALLEARTH,
        effect = CONST_ME_STONES,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -650,
        target = true,
        shootEffect = CONST_ANI_LARGEROCK,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        minDamage = -150,
        maxDamage = -260,
        length = 5,
        target = false,
        effect = CONST_ME_STONES,
    },
}
monster.defenses = {
    defense = 35,
    armor = 35,
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 1},
    {type = COMBAT_ICEDAMAGE, percent = 30},
    {type = COMBAT_PHYSICALDAMAGE, percent = 1},
    {type = COMBAT_ENERGYDAMAGE, percent = 30},
    {type = COMBAT_HOLYDAMAGE, percent = 30},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Rumble!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 100,
    },
    {
        id = "platinum coin",
        chance = 92000,
        maxCount = 7,
    },
    {
        id = "stone skin amulet",
        chance = 2270,
    },
    {
        id = "dwarven ring",
        chance = 2840,
    },
    {
        id = "giant sword",
        chance = 570,
    },
    {
        id = "crystal mace",
        chance = 850,
    },
    {
        id = "war axe",
        chance = 920,
    },
    {
        id = "sapphire hammer",
        chance = 1490,
    },
    {
        id = "spiked squelcher",
        chance = 1490,
    },
    {
        id = "glorious axe",
        chance = 3340,
    },
    {
        id = "strong health potion",
        chance = 13840,
        maxCount = 2,
    },
    {
        id = "strong mana potion",
        chance = 14900,
        maxCount = 2,
    },
    {
        id = "great mana potion",
        chance = 15610,
        maxCount = 2,
    },
    {
        id = "mana potion",
        chance = 15050,
        maxCount = 2,
    },
    {
        id = "ultimate health potion",
        chance = 14410,
    },
    {
        id = 8748,
        chance = 11360,
    },
    {
        id = "ancient stone",
        chance = 12850,
    },
    {
        id = "crystalline arrow",
        chance = 9940,
        maxCount = 10,
    },
    {
        id = "green crystal splinter",
        chance = 6960,
    },
    {
        id = "cyan crystal fragment",
        chance = 6810,
    },
    {
        id = "stone nose",
        chance = 18679,
    },
    {
        id = "crystalline spikes",
        chance = 16320,
    },
}

mtype:register(monster)
