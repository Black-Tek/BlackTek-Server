local mtype = Game.createMonsterType("Waspoid")
local monster = {}

monster.name = "Waspoid"
monster.description = "Waspoid"
monster.experience = 830
monster.race = "venom"
monster.maxHealth = 1100
monster.health = 1100
monster.speed = 210
monster.manaCost = 0
monster.corpse = 15396
monster.outfit = { lookType = 462 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -248,
        -- NOTE: melee condition (poison=400); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 400000,
        },
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        minDamage = -110,
        maxDamage = -180,
        radius = 3,
        target = true,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -80,
        maxDamage = -100,
        shootEffect = CONST_ANI_POISON,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 5},
    {type = COMBAT_ENERGYDAMAGE, percent = 25},
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Peeex!", yell = false},
}
monster.loot = {
    {
        id = "emerald bangle",
        chance = 2040,
    },
    {
        id = "black pearl",
        chance = 4230,
    },
    {
        id = "gold coin",
        chance = 40000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 35,
    },
    {
        id = "platinum coin",
        chance = 40430,
    },
    {
        id = "yellow gem",
        chance = 1040,
    },
    {
        id = "waspoid claw",
        chance = 9096,
    },
    {
        id = "waspoid wing",
        chance = 13890,
    },
    {
        id = "compound eye",
        chance = 6060,
    },
    {
        id = "grasshopper legs",
        chance = 230,
    },
    {
        id = "carapace shield",
        chance = 120,
    },
    {
        id = "hive scythe",
        chance = 330,
    },
}

mtype:register(monster)
