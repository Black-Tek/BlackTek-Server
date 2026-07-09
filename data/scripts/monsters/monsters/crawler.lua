local mtype = Game.createMonsterType("Crawler")
local monster = {}

monster.name = "Crawler"
monster.description = "a crawler"
monster.experience = 1000
monster.race = "venom"
monster.maxHealth = 1450
monster.health = 1450
monster.speed = 200
monster.manaCost = 0
monster.corpse = 15292
monster.outfit = { lookType = 456 }
monster.runHealth = 40
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
    pushable = true,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -228,
        -- NOTE: melee condition (poison=80); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 80000,
        },
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -100,
        maxDamage = -180,
        shootEffect = CONST_ANI_SMALLEARTH,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 300,
        duration = 3000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 5},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Sssschrchrsss!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 90,
    },
    {
        id = "yellow gem",
        chance = 530,
    },
    {
        id = "life ring",
        chance = 50000,
    },
    {
        id = "war hammer",
        chance = 2070,
    },
    {
        id = "great mana potion",
        chance = 9300,
    },
    {
        id = "great health potion",
        chance = 6200,
    },
    {
        id = "springsprout rod",
        chance = 710,
    },
    {
        id = "small topaz",
        chance = 10040,
        maxCount = 2,
    },
    {
        id = "crawler head plating",
        chance = 18430,
    },
    {
        id = "compound eye",
        chance = 14640,
    },
    {
        id = "grasshopper legs",
        chance = 100,
    },
}

mtype:register(monster)
