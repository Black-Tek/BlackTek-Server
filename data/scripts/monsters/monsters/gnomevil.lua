local mtype = Game.createMonsterType("Gnomevil")
local monster = {}

monster.name = "Gnomevil"
monster.description = "Gnomevil"
monster.experience = 45000
monster.race = "blood"
monster.maxHealth = 250000
monster.health = 250000
monster.speed = 470
monster.manaCost = 0
monster.corpse = 18443
monster.outfit = { lookType = 504 }
monster.runHealth = 10000
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 1
monster.staticAttackChance = 50
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 1300,
        minDamage = 0,
        maxDamage = -630,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        range = 7,
        speed = -400,
        duration = 2500,
        shootEffect = CONST_ANI_WHIRLWINDAXE,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 100,
        range = 7,
        minDamage = -820,
        maxDamage = -950,
        effect = CONST_ME_ENERGYAREA,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 9,
        minDamage = -230,
        maxDamage = -500,
        length = 8,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "energy",
        interval = 1000,
        chance = 12,
        range = 3,
        minDamage = -350,
        maxDamage = -800,
        target = true,
        effect = CONST_ME_PURPLEENERGY,
    },
}
monster.defenses = {
    defense = 35,
    armor = 25,
    {
        name = "healing",
        interval = 1000,
        chance = 25,
        minDamage = 1000,
        maxDamage = 4000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "No more Mr. Nicegnome!", yell = false},
    {text = "Taste the power of the dark side!", yell = false},
    {text = "I've seen the light! And it was all engulfing fire!", yell = false},
    {text = "Tatatata!!!", yell = false},
    {text = "Muhahaha!", yell = false},
    {text = "I'm feeling grrrreat!", yell = false},
}
monster.loot = {
    {
        id = "decorative ribbon",
        chance = 1710,
    },
    {
        id = "crystalline sword",
        chance = 14100,
    },
    {
        id = "crystalline axe",
        chance = 9830,
    },
    {
        id = "mycological mace",
        chance = 8120,
    },
    {
        id = "crystal crossbow",
        chance = 8120,
    },
    {
        id = "mycological bow",
        chance = 9400,
    },
    {
        id = "shiny blade",
        chance = 7690,
    },
    {
        id = "Gnomevil's hat",
        chance = 99150,
    },
}

mtype:register(monster)
