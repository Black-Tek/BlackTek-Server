local mtype = Game.createMonsterType("Obujos")
local monster = {}

monster.name = "Obujos"
monster.description = "Obujos"
monster.experience = 20000
monster.race = "blood"
monster.maxHealth = 35000
monster.health = 35000
monster.speed = 560
monster.manaCost = 0
monster.corpse = 15224
monster.outfit = { lookType = 445 }
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
        maxDamage = -1200,
        -- NOTE: melee condition (poison=360); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 360000,
        },
    },
    {
        name = "death",
        interval = 3000,
        chance = 23,
        range = 7,
        minDamage = -200,
        maxDamage = -800,
        radius = 7,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "manadrain",
        interval = 3500,
        chance = 20,
        range = 7,
        minDamage = -200,
        maxDamage = -600,
        radius = 6,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_BIGCLOUDS,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 25,
        range = 7,
        minDamage = -200,
        maxDamage = -800,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_POISONARROW,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "ice",
        interval = 1200,
        chance = 7,
        minDamage = -600,
        maxDamage = -1300,
        length = 8,
        effect = CONST_ME_GIANTICE,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 14,
        minDamage = -600,
        maxDamage = -1600,
        length = 8,
        spread = 3,
        effect = CONST_ME_ICETORNADO,
    },
}
monster.defenses = {
    defense = 40,
    armor = 40,
    {
        name = "healing",
        interval = 1000,
        chance = 15,
        minDamage = 1000,
        maxDamage = 2100,
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
    {text = "JN OBU!!", yell = false},
    {text = "QJELL KEJH!!", yell = false},
}
monster.loot = {
    {
        id = "deepling axe",
        chance = 14285,
    },
    {
        id = "depth scutum",
        chance = 100000,
    },
    {
        id = "ornate legs",
        chance = 14285,
    },
}

mtype:register(monster)
