local mtype = Game.createMonsterType("Barbarian Brutetamer")
local monster = {}

monster.name = "Barbarian Brutetamer"
monster.description = "a barbarian brutetamer"
monster.experience = 90
monster.race = "blood"
monster.maxHealth = 145
monster.health = 145
monster.speed = 190
monster.manaCost = 0
monster.corpse = 20339
monster.outfit = {
    lookType = 264,
    lookHead = 78,
    lookBody = 97,
    lookLegs = 95,
    lookFeet = 121,
}
monster.runHealth = 10
monster.changeTarget = {
    interval = 60000,
    chance = 0,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -20,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -34,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_SNOWBALL,
    },
    {
        name = "barbarian brutetamer skill reducer",
        interval = 2000,
        chance = 15,
        range = 5,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
    {
        name = "healing",
        interval = 2000,
        chance = 40,
        minDamage = 50,
        maxDamage = 80,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 50},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_PHYSICALDAMAGE, percent = -20},
    {type = COMBAT_DEATHDAMAGE, percent = -1},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "outfit", condition = true},
    {type = "invisible", condition = true},
    {type = "drunk", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "War Wolf", interval = 2000, chance = 10, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "To me, creatures of the wild!", yell = false},
    {text = "My instincts tell me about your cowardice.", yell = false},
}
monster.loot = {
    {
        id = 1958,
        chance = 5000,
    },
    {
        id = "gold coin",
        chance = 58000,
        maxCount = 15,
    },
    {
        id = 2401,
        chance = 6500,
    },
    {
        id = "chain armor",
        chance = 8900,
    },
    {
        id = "corncob",
        chance = 11000,
        maxCount = 2,
    },
    {
        id = "hunting spear",
        chance = 5250,
    },
    {
        id = "fur bag",
        chance = 7540,
    },
    {
        id = "brutetamer's staff",
        chance = 380,
    },
    {
        id = "fur boots",
        chance = 160,
    },
    {
        id = "mammoth fur cape",
        chance = 160,
    },
    {
        id = "mammoth fur shorts",
        chance = 90,
    },
    {
        id = "mana potion",
        chance = 630,
    },
}

mtype:register(monster)
