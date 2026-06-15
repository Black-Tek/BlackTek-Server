local mtype = Game.createMonsterType("Armenius")
local monster = {}

monster.name = "Armenius"
monster.description = "Armenius"
monster.experience = 500
monster.race = "venom"
monster.maxHealth = 550
monster.health = 550
monster.speed = 330
monster.manaCost = 0
monster.corpse = 6021
monster.outfit = { lookType = 79 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -210,
        -- NOTE: melee condition (poison=100); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 100000,
        },
    },
    {
        name = "earth",
        interval = 1000,
        chance = 12,
        range = 7,
        minDamage = -15,
        maxDamage = -135,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 13,
        range = 7,
        speed = -700,
        duration = 25000,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "poisoncondition",
        interval = 3000,
        chance = 17,
        minDamage = -30,
        maxDamage = -100,
        radius = 5,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 20,
    armor = 25,
    {
        name = "speed",
        interval = 1000,
        chance = 8,
        speed = 350,
        duration = 9000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = -15},
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 3
monster.summons = {
    {name = "Larva", interval = 1000, chance = 15, max = 3},
}
monster.loot = {
    {
        id = "scarab amulet",
        chance = 2063,
    },
    {
        id = 2142,
        chance = 2564,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 90,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 90,
    },
    {
        id = "small emerald",
        chance = 4347,
        maxCount = 3,
    },
    {
        id = "small amethyst",
        chance = 4166,
        maxCount = 4,
    },
    {
        id = "scarab coin",
        chance = 7692,
        maxCount = 2,
    },
    {
        id = 2162,
        chance = 11111,
    },
    {
        id = "daramian waraxe",
        chance = 781,
    },
    {
        id = "plate armor",
        chance = 10000,
    },
    {
        id = "scarab shield",
        chance = 518,
    },
    {
        id = "strong health potion",
        chance = 1041,
    },
    {
        id = "terra hood",
        chance = 1298,
    },
    {
        id = "scarab pincers",
        chance = 3571,
    },
}

mtype:register(monster)
