local mtype = Game.createMonsterType("Spitter")
local monster = {}

monster.name = "Spitter"
monster.description = "a spitter"
monster.experience = 1100
monster.race = "venom"
monster.maxHealth = 1500
monster.health = 1500
monster.speed = 190
monster.manaCost = 0
monster.corpse = 15392
monster.outfit = { lookType = 461 }
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
    illusionable = false,
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
        maxDamage = -150,
        -- NOTE: melee condition (poison=240); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 240000,
        },
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -100,
        maxDamage = -160,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        target = true,
        speed = -600,
        duration = 15000,
        shootEffect = CONST_ANI_POISON,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 400,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 5},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = 5},
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
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
        id = "small amethyst",
        chance = 8000,
        maxCount = 2,
    },
    {
        id = "platinum coin",
        chance = 75250,
    },
    {
        id = "green gem",
        chance = 210,
    },
    {
        id = 2169,
        chance = 2400,
    },
    {
        id = "platinum amulet",
        chance = 260,
    },
    {
        id = "crusader helmet",
        chance = 230,
    },
    {
        id = "brown mushroom",
        chance = 7500,
        maxCount = 3,
    },
    {
        id = "mastermind potion",
        chance = 310,
    },
    {
        id = "crystal sword",
        chance = 2000,
    },
    {
        id = "great mana potion",
        chance = 8000,
    },
    {
        id = "great health potion",
        chance = 5000,
    },
    {
        id = "spitter nose",
        chance = 18000,
    },
    {
        id = "compound eye",
        chance = 15000,
    },
    {
        id = "calopteryx cape",
        chance = 240,
    },
    {
        id = "grasshopper legs",
        chance = 130,
    },
}

mtype:register(monster)
