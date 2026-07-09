local mtype = Game.createMonsterType("Ghoulish Hyaena")
local monster = {}

monster.name = "Ghoulish Hyaena"
monster.description = "a ghoulish hyaena"
monster.experience = 195
monster.race = "blood"
monster.maxHealth = 400
monster.health = 400
monster.speed = 200
monster.manaCost = 275
monster.corpse = 6026
monster.outfit = { lookType = 94 }
monster.runHealth = 30
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = true,
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
        maxDamage = -112,
        -- NOTE: melee condition (poison=10); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 10000,
        },
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 15,
        length = 3,
        spread = 2,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 3000,
        duration = 2000,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 70},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Grawrrr!!", yell = false},
    {text = "Hoouu!", yell = false},
}
monster.loot = {
    {
        id = "small ruby",
        chance = 2700,
        maxCount = 2,
    },
    {
        id = "gold coin",
        chance = 89000,
        maxCount = 40,
    },
    {
        id = "meat",
        chance = 51060,
        maxCount = 3,
    },
    {
        id = "worm",
        chance = 65000,
        maxCount = 7,
    },
    {
        id = "health potion",
        chance = 19840,
    },
}

mtype:register(monster)
