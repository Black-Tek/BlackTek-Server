local mtype = Game.createMonsterType("Abyssador")
local monster = {}

monster.name = "Abyssador"
monster.description = "Abyssador"
monster.experience = 50000
monster.race = "blood"
monster.maxHealth = 300000
monster.health = 300000
monster.speed = 470
monster.manaCost = 0
monster.corpse = 17413
monster.outfit = { lookType = 495 }
monster.runHealth = 100
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
        interval = 2500,
        minDamage = 0,
        maxDamage = -960,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 20,
        range = 7,
        speed = -400,
        duration = 2500,
        shootEffect = CONST_ANI_EARTH,
    },
    {
        name = "fire",
        interval = 800,
        chance = 10,
        range = 7,
        minDamage = -820,
        maxDamage = -1250,
        radius = 10,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "energy",
        interval = 750,
        chance = 10,
        range = 3,
        minDamage = -650,
        maxDamage = -1300,
        radius = 12,
        target = true,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "lifedrain",
        interval = 700,
        chance = 10,
        range = 3,
        minDamage = -600,
        maxDamage = -1230,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "earth",
        interval = 700,
        chance = 12,
        minDamage = -40,
        maxDamage = -130,
        radius = 3,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 20,
    armor = 15,
    {
        name = "healing",
        interval = 1000,
        chance = 1,
        minDamage = 0,
        maxDamage = 300000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "healing",
        interval = 5000,
        chance = 30,
        minDamage = 1000,
        maxDamage = 1000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 25,
        duration = 3000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = -20},
    {type = COMBAT_ENERGYDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "drown", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "drunk", condition = true},
    {type = "outfit", condition = true},
}
monster.voices = {
    interval = 3000,
    chance = 10,
    {text = "*** BRAINS *** SMALL ***", yell = false},
    {text = "*** LIVE *** DRY ***", yell = false},
    {text = "*** IMPORTANT ***", yell = false},
    {text = "*** FIRE *** HOME *** VICTORY ***", yell = false},
    {text = "*** EXISTENCE *** FUTILE ***", yell = false},
    {text = "*** TIME ***", yell = false},
    {text = "*** STEALTH ***", yell = false},
    {text = "*** DEATH ***", yell = false},
}
monster.loot = {
    {
        id = "decorative ribbon",
        chance = 10000,
    },
    {
        id = "crystalline sword",
        chance = 5500,
    },
    {
        id = "crystalline axe",
        chance = 5500,
    },
    {
        id = "mycological mace",
        chance = 4500,
    },
    {
        id = "crystal crossbow",
        chance = 4500,
    },
    {
        id = "mycological bow",
        chance = 10000,
    },
    {
        id = "shiny blade",
        chance = 8000,
    },
    {
        id = "Abyssador's lash",
        chance = 100000,
    },
}

mtype:register(monster)
