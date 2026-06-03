local mtype = Game.createMonsterType("Hide")
local monster = {}

monster.name = "Hide"
monster.description = "Hide"
monster.experience = 240
monster.race = "venom"
monster.maxHealth = 500
monster.health = 500
monster.speed = 280
monster.manaCost = 0
monster.corpse = 6060
monster.outfit = { lookType = 219 }
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
    canPushCreatures = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -140,
        -- NOTE: melee condition (poison=80); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 80000,
        },
    },
}
monster.defenses = {
    defense = 40,
    armor = 25,
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        speed = 340,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = -15},
    {type = COMBAT_ICEDAMAGE, percent = -15},
    {type = COMBAT_PHYSICALDAMAGE, percent = 40},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.loot = {
    {
        id = "platinum coin",
        chance = 100000,
        maxCount = 5,
    },
    {
        id = 2169,
        chance = 33333,
    },
    {
        id = "steel helmet",
        chance = 50000,
    },
    {
        id = "knight legs",
        chance = 50000,
    },
    {
        id = "spider silk",
        chance = 50000,
    },
    {
        id = "terra hood",
        chance = 50000,
    },
}

mtype:register(monster)
