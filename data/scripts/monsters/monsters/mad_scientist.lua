local mtype = Game.createMonsterType("Mad Scientist")
local monster = {}

monster.name = "Mad Scientist"
monster.description = "a mad scientist"
monster.experience = 205
monster.race = "blood"
monster.maxHealth = 325
monster.health = 325
monster.speed = 190
monster.manaCost = 0
monster.corpse = 20439
monster.outfit = {
    lookType = 133,
    lookHead = 39,
    lookBody = 0,
    lookLegs = 19,
    lookFeet = 20,
    lookAddons = 1,
}
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
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -35,
    },
    {
        name = "drown",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -20,
        maxDamage = -56,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_SMALLEARTH,
        effect = CONST_ME_POFF,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        minDamage = -20,
        maxDamage = -36,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        range = 7,
        target = true,
        speed = -300,
        duration = 2000,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_SMALLPLANTS,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 10,
        maxDamage = 30,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "drown", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Die in the name of Science!", yell = false},
    {text = "You will regret interrupting my studies!", yell = false},
    {text = "Let me test this!", yell = false},
    {text = "I will study your corpse!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 30000,
        maxCount = 65,
    },
    {
        id = "gold coin",
        chance = 30000,
        maxCount = 50,
    },
    {
        id = 2162,
        chance = 2000,
    },
    {
        id = "life crystal",
        chance = 2000,
    },
    {
        id = "soulfire rune",
        chance = 6000,
    },
    {
        id = "cookie",
        chance = 1200,
        maxCount = 5,
    },
    {
        id = "white mushroom",
        chance = 8000,
        maxCount = 3,
    },
    {
        id = 6324,
        chance = 820,
    },
    {
        id = "mastermind potion",
        chance = 130,
    },
    {
        id = "health potion",
        chance = 19000,
    },
    {
        id = "mana potion",
        chance = 19000,
    },
    {
        id = "small enchanted amethyst",
        chance = 470,
    },
}

mtype:register(monster)
