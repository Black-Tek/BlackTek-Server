local mtype = Game.createMonsterType("Carniphila")
local monster = {}

monster.name = "Carniphila"
monster.description = "a carniphila"
monster.experience = 150
monster.race = "venom"
monster.maxHealth = 255
monster.health = 255
monster.speed = 110
monster.manaCost = 0
monster.corpse = 6047
monster.outfit = { lookType = 120 }
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -100,
        -- NOTE: melee condition (poison=100); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 100000,
        },
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -60,
        maxDamage = -95,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        speed = -800,
        duration = 30000,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        minDamage = -40,
        maxDamage = -130,
        radius = 3,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 22,
    armor = 22,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 87000,
        maxCount = 40,
    },
    {
        id = "dark mushroom",
        chance = 8000,
    },
    {
        id = "carniphila seeds",
        chance = 4000,
    },
    {
        id = "shadow herb",
        chance = 1000,
    },
    {
        id = "corncob",
        chance = 1000,
    },
    {
        id = "sling herb",
        chance = 500,
        maxCount = 2,
    },
    {
        id = "seeds",
        chance = 500,
    },
    {
        id = "carrot on a stick",
        chance = 150,
    },
}

mtype:register(monster)
