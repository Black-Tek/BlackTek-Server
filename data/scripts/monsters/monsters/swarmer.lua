local mtype = Game.createMonsterType("Swarmer")
local monster = {}

monster.name = "Swarmer"
monster.description = "a swarmer"
monster.experience = 350
monster.race = "venom"
monster.maxHealth = 460
monster.health = 460
monster.speed = 190
monster.manaCost = 0
monster.corpse = 15388
monster.outfit = { lookType = 460 }
monster.runHealth = 50
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
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
        maxDamage = -102,
        -- NOTE: melee condition (poison=80); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 80000,
        },
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -50,
        maxDamage = -110,
        target = true,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 220,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 75},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = -1},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Flzlzlzlzlzlzlz!", yell = false},
    {text = "Rzlrzlrzlrzlrzlrzl!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 75,
    },
    {
        id = "small emerald",
        chance = 920,
    },
    {
        id = "epee",
        chance = 450,
    },
    {
        id = "swarmer antenna",
        chance = 15300,
    },
    {
        id = "compound eye",
        chance = 12500,
    },
}

mtype:register(monster)
