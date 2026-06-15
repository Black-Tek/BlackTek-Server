local mtype = Game.createMonsterType("Chakoya Toolshaper")
local monster = {}

monster.name = "Chakoya Toolshaper"
monster.description = "a chakoya toolshaper"
monster.experience = 40
monster.race = "blood"
monster.maxHealth = 80
monster.health = 80
monster.speed = 136
monster.manaCost = 0
monster.corpse = 7320
monster.outfit = { lookType = 259 }
monster.changeTarget = {
    interval = 60000,
    chance = 0,
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
        name = "physical",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -45,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_SMALLSTONE,
    },
}
monster.defenses = {
    defense = 7,
    armor = 7,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -15},
    {type = COMBAT_FIREDAMAGE, percent = 40},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Chikuva!!", yell = false},
    {text = "Jinuma jamjam!", yell = false},
    {text = "Suvituka siq chuqua!", yell = false},
    {text = "Kiyosa sipaju!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 78354,
        maxCount = 20,
    },
    {
        id = 2667,
        chance = 25021,
        maxCount = 2,
    },
    {
        id = 2398,
        chance = 5209,
    },
    {
        id = 3456,
        chance = 1016,
    },
    {
        id = 2541,
        chance = 874,
    },
    {
        id = 22727,
        chance = 461,
    },
    {
        id = 7441,
        chance = 404,
    },
    {
        id = 7381,
        chance = 179,
    },
    {
        id = 7158,
        chance = 122,
    },
    {
        id = 7159,
        chance = 75,
    },
    {
        id = 2669,
        chance = 66,
    },
}

mtype:register(monster)
