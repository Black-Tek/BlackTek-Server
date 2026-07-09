local mtype = Game.createMonsterType("Jaul")
local monster = {}

monster.name = "Jaul"
monster.description = "Jaul"
monster.experience = 30000
monster.race = "blood"
monster.maxHealth = 90000
monster.health = 90000
monster.speed = 560
monster.manaCost = 0
monster.corpse = 15220
monster.outfit = { lookType = 444 }
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
        maxDamage = -2000,
        -- NOTE: melee condition (poison=870); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 870000,
        },
    },
    {
        name = "fire",
        interval = 2200,
        chance = 19,
        range = 7,
        minDamage = 0,
        maxDamage = -1000,
        radius = 7,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "manadrain",
        interval = 3000,
        chance = 32,
        range = 7,
        minDamage = -200,
        maxDamage = -800,
        radius = 7,
        target = true,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "earth",
        interval = 1300,
        chance = 27,
        minDamage = 0,
        maxDamage = -600,
        radius = 3,
        target = false,
        effect = CONST_ME_EXPLOSIONAREA,
    },
    {
        name = "death",
        interval = 1200,
        chance = 6,
        minDamage = -300,
        maxDamage = -900,
        length = 8,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 5,
        minDamage = -500,
        maxDamage = -1000,
        length = 8,
        effect = CONST_ME_HITBYFIRE,
    },
    {
        name = "ice",
        interval = 1000,
        chance = 5,
        minDamage = -1000,
        maxDamage = -2000,
        length = 8,
        effect = CONST_ME_ICEAREA,
    },
    {
        name = "lifedrain",
        interval = 4000,
        chance = 10,
        minDamage = -500,
        maxDamage = -1000,
        length = 8,
        spread = 3,
        effect = CONST_ME_WATERSPLASH,
    },
    {
        name = "speed",
        interval = 1900,
        chance = 14,
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
        interval = 2000,
        chance = 25,
        minDamage = 1000,
        maxDamage = 2300,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "healing",
        interval = 5500,
        chance = 10,
        minDamage = 14000,
        maxDamage = 21000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = -1},
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
    {text = "QJELL AFAR GOU JEY!!", yell = false},
    {text = "DIE!! KENH!!", yell = false},
}
monster.loot = {
    {
        id = "deepling axe",
        chance = 25000,
    },
    {
        id = "ornate chestplate",
        chance = 25000,
    },
    {
        id = "depth lorica",
        chance = 20000,
    },
    {
        id = 15408,
        chance = 33333,
    },
    {
        id = "depth calcei",
        chance = 14285,
    },
    {
        id = "ornate legs",
        chance = 7142,
    },
    {
        id = "ornate shield",
        chance = 50000,
    },
    {
        id = "ornate mace",
        chance = 14285,
    },
}

mtype:register(monster)
