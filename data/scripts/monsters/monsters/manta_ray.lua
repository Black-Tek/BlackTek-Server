local mtype = Game.createMonsterType("Manta Ray")
local monster = {}

monster.name = "Manta Ray"
monster.description = "a manta ray"
monster.experience = 125
monster.race = "blood"
monster.maxHealth = 680
monster.health = 680
monster.speed = 200
monster.manaCost = 0
monster.corpse = 15276
monster.outfit = { lookType = 449 }
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
    canPushItems = false,
    canPushCreatures = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -99,
        -- NOTE: melee condition (poison=120); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 120000,
        },
    },
    {
        name = "energy",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -15,
        maxDamage = -75,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 10,
        length = 4,
        target = false,
        effect = CONST_ME_ENERGYHIT,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = -1},
    {type = COMBAT_PHYSICALDAMAGE, percent = -1},
    {type = COMBAT_ENERGYDAMAGE, percent = 1},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "invisible", condition = true},
    {type = "drown", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Flap flap flap!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 38,
    },
}

mtype:register(monster)
