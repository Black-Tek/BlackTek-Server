local mtype = Game.createMonsterType("Tanjis")
local monster = {}

monster.name = "Tanjis"
monster.description = "Tanjis"
monster.experience = 15000
monster.race = "blood"
monster.maxHealth = 30000
monster.health = 30000
monster.speed = 560
monster.manaCost = 0
monster.corpse = 15228
monster.outfit = { lookType = 446 }
monster.runHealth = 60
monster.changeTarget = {
    interval = 2000,
    chance = 50,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = true,
    canPushCreatures = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -600,
    },
    {
        name = "manadrain",
        interval = 2500,
        chance = 20,
        range = 7,
        minDamage = -200,
        maxDamage = -600,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_CARNIPHILA,
    },
    {
        name = "ice",
        interval = 3500,
        chance = 27,
        range = 7,
        minDamage = -200,
        maxDamage = -400,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_ICE,
        effect = CONST_ME_ICEAREA,
    },
    {
        name = "ice",
        interval = 3500,
        chance = 15,
        range = 1,
        minDamage = -100,
        maxDamage = -400,
        radius = 1,
        target = true,
    },
    {
        name = "drown",
        interval = 2300,
        chance = 11,
        range = 7,
        minDamage = -200,
        maxDamage = -500,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_ICE,
        effect = CONST_ME_WATERSPLASH,
    },
    {
        name = "manadrain",
        interval = 2300,
        chance = 14,
        range = 7,
        minDamage = -200,
        maxDamage = -600,
        radius = 7,
        target = false,
        effect = CONST_ME_BUBBLES,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -100,
        maxDamage = -400,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_LARGEROCK,
    },
    {
        name = "death",
        interval = 1200,
        chance = 7,
        minDamage = -300,
        maxDamage = -800,
        length = 8,
        effect = CONST_ME_SMALLCLOUDS,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 14,
        minDamage = -200,
        maxDamage = -500,
        length = 8,
        spread = 3,
        effect = CONST_ME_PURPLEENERGY,
    },
    {
        name = "speed",
        interval = 2150,
        chance = 16,
        range = 7,
        radius = 1,
        target = true,
        speed = -600,
        duration = 15000,
    },
}
monster.defenses = {
    defense = 40,
    armor = 40,
    {
        name = "healing",
        interval = 3000,
        chance = 40,
        minDamage = 800,
        maxDamage = 1100,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = -20},
    {type = COMBAT_ENERGYDAMAGE, percent = -20},
    {type = COMBAT_PHYSICALDAMAGE, percent = -20},
    {type = COMBAT_DEATHDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "JAUL QJELL, JAKNH JEH KENH!!", yell = false},
}
monster.loot = {
    {
        id = "depth ocrea",
        chance = 50000,
    },
    {
        id = "ornate shield",
        chance = 20000,
    },
    {
        id = "ornate mace",
        chance = 25000,
    },
}

mtype:register(monster)
