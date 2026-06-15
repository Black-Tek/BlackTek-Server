local mtype = Game.createMonsterType("Bog Raider")
local monster = {}

monster.name = "Bog Raider"
monster.description = "a bog raider"
monster.experience = 800
monster.race = "venom"
monster.maxHealth = 1300
monster.health = 1300
monster.speed = 250
monster.manaCost = 0
monster.corpse = 8951
monster.outfit = { lookType = 299 }
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -180,
        -- NOTE: melee condition (poison=80); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 80000,
        },
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -90,
        maxDamage = -140,
        target = true,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        minDamage = -100,
        maxDamage = -175,
        radius = 3,
        target = false,
        effect = CONST_ME_BUBBLES,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -96,
        maxDamage = -110,
        target = true,
        shootEffect = CONST_ANI_SMALLEARTH,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        target = true,
        speed = -600,
        duration = 15000,
        effect = CONST_ME_SMALLPLANTS,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 65,
        maxDamage = 95,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_EARTHDAMAGE, percent = 30},
    {type = COMBAT_ICEDAMAGE, percent = -5},
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = 5},
    {type = COMBAT_FIREDAMAGE, percent = 85},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Tchhh!", yell = false},
    {text = "Slurp!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50750,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50750,
        maxCount = 5,
    },
    {
        id = "boggy dreads",
        chance = 9870,
    },
    {
        id = "great health potion",
        chance = 2030,
    },
    {
        id = "plate legs",
        chance = 2020,
    },
    {
        id = "great spirit potion",
        chance = 2000,
    },
    {
        id = "springsprout rod",
        chance = 1030,
    },
    {
        id = "ultimate health potion",
        chance = 770,
    },
    {
        id = "belted cape",
        chance = 590,
    },
    {
        id = "paladin armor",
        chance = 140,
    },
}

mtype:register(monster)
