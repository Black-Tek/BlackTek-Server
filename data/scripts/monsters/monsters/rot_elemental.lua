local mtype = Game.createMonsterType("Rot Elemental")
local monster = {}

monster.name = "Rot Elemental"
monster.description = "a rot elemental"
monster.experience = 750
monster.race = "venom"
monster.maxHealth = 850
monster.health = 850
monster.speed = 230
monster.manaCost = 0
monster.corpse = 23481
monster.outfit = { lookType = 615 }
monster.changeTarget = {
    interval = 2000,
    chance = 2,
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
    canWalkOnPoison = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        attack = 50,
        skill = 58,
        -- NOTE: melee condition (poison=280); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 280000,
        },
    },
    {
        name = "earth",
        interval = 2000,
        chance = 13,
        range = 7,
        minDamage = -150,
        maxDamage = -250,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_GLOOTHSPEAR,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        minDamage = -150,
        maxDamage = -230,
        length = 6,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -200,
        maxDamage = -300,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 25,
        radius = 1,
        target = true,
        speed = -900,
        effect = CONST_ME_CARNIPHILA,
    },
}
monster.defenses = {
    defense = 15,
    armor = 10,
    {
        name = "healing",
        interval = 2000,
        chance = 7,
        minDamage = 40,
        maxDamage = 60,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 9,
        speed = 470,
        duration = 7000,
        effect = CONST_ME_SMOKE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = 30},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "*glibb*", yell = false},
    {text = "*splib*", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 80,
    },
    {
        id = 2152,
        chance = 10390,
    },
    {
        id = 7588,
        chance = 10310,
    },
    {
        id = 7589,
        chance = 9990,
    },
    {
        id = 23553,
        chance = 14320,
    },
    {
        id = 2168,
        chance = 1630,
    },
    {
        id = 2146,
        chance = 5110,
    },
    {
        id = 9970,
        chance = 4140,
        maxCount = 2,
    },
    {
        id = 2149,
        chance = 5340,
        maxCount = 2,
    },
    {
        id = 2155,
        chance = 80,
    },
    {
        id = 23554,
        chance = 130,
    },
    {
        id = 23551,
        chance = 60,
    },
    {
        id = 23529,
        chance = 5660,
    },
}

mtype:register(monster)
