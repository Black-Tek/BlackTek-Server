local mtype = Game.createMonsterType("Ancient Scarab")
local monster = {}

monster.name = "Ancient Scarab"
monster.description = "an ancient scarab"
monster.experience = 720
monster.race = "venom"
monster.maxHealth = 1000
monster.health = 1000
monster.speed = 218
monster.manaCost = 0
monster.corpse = 6021
monster.outfit = { lookType = 79 }
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -210,
        -- NOTE: melee condition (poison=56); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 56000,
        },
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -15,
        maxDamage = -145,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        speed = -700,
        duration = 25000,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 30,
        minDamage = -440,
        maxDamage = -520,
        radius = 5,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 36,
    armor = 36,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 260,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = -20},
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 3
monster.summons = {
    {name = "Larva", interval = 2000, chance = 10, max = 0},
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
        maxCount = 87,
    },
    {
        id = 2162,
        chance = 10300,
    },
    {
        id = "scarab coin",
        chance = 8100,
        maxCount = 2,
    },
    {
        id = "scarab pincers",
        chance = 7140,
    },
    {
        id = "small emerald",
        chance = 6000,
        maxCount = 3,
    },
    {
        id = "small amethyst",
        chance = 6000,
        maxCount = 4,
    },
    {
        id = "plate armor",
        chance = 5000,
    },
    {
        id = "scarab amulet",
        chance = 3510,
    },
    {
        id = 2142,
        chance = 2670,
    },
    {
        id = "strong health potion",
        chance = 1660,
    },
    {
        id = "springsprout rod",
        chance = 1000,
    },
    {
        id = "scarab shield",
        chance = 500,
    },
    {
        id = "terra hood",
        chance = 400,
    },
    {
        id = "daramian waraxe",
        chance = 300,
    },
}

mtype:register(monster)
