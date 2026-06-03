local mtype = Game.createMonsterType("Gravelord Oshuran")
local monster = {}

monster.name = "Gravelord Oshuran"
monster.description = "Gravelord Oshuran"
monster.experience = 2400
monster.race = "undead"
monster.maxHealth = 3100
monster.health = 3100
monster.speed = 440
monster.manaCost = 0
monster.corpse = 6028
monster.outfit = { lookType = 99 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
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
        interval = 2000,
        minDamage = 0,
        maxDamage = -250,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 25,
        range = 7,
        speed = -300,
        duration = 30000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -180,
        maxDamage = -300,
        length = 7,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        minDamage = -100,
        maxDamage = -350,
        length = 7,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        range = 1,
        minDamage = -200,
        maxDamage = -245,
        target = true,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "healing",
        interval = 3000,
        chance = 15,
        minDamage = 100,
        maxDamage = 120,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 35},
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 80},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 4
monster.summons = {
    {name = "Bonebeast", interval = 2000, chance = 10, max = 0},
}
monster.voices = {
    interval = 1000,
    chance = 2,
    {text = "Your mortality is disgusting!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 50000,
        maxCount = 100,
    },
    {
        id = 2148,
        chance = 50000,
        maxCount = 15,
    },
    {
        id = 7589,
        chance = 17500,
    },
    {
        id = 2144,
        chance = 15960,
    },
    {
        id = 2143,
        chance = 15000,
    },
    {
        id = 2214,
        chance = 15040,
    },
    {
        id = 2656,
        chance = 500,
    },
    {
        id = 7893,
        chance = 900,
    },
    {
        id = 8904,
        chance = 300,
    },
    {
        id = 2175,
        chance = 4650,
    },
}

mtype:register(monster)
