local mtype = Game.createMonsterType("Chakoya Windcaller")
local monster = {}

monster.name = "Chakoya Windcaller"
monster.description = "a chakoya windcaller"
monster.experience = 48
monster.race = "blood"
monster.maxHealth = 84
monster.health = 84
monster.speed = 200
monster.manaCost = 305
monster.corpse = 7320
monster.outfit = { lookType = 260 }
monster.changeTarget = {
    interval = 60000,
    chance = 0,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = true,
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
        maxDamage = -22,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -16,
        maxDamage = -32,
        shootEffect = CONST_ANI_ICE,
    },
    {
        name = "freezecondition",
        interval = 2000,
        chance = 10,
        minDamage = -130,
        maxDamage = -160,
        radius = 3,
        target = false,
        effect = CONST_ME_ICEAREA,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 10,
        minDamage = -9,
        maxDamage = -30,
        length = 5,
        spread = 2,
        target = false,
        effect = CONST_ME_ICEAREA,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
    {
        name = "invisible",
        interval = 2000,
        chance = 15,
        duration = 3000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 50},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = -15},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Mupi! Si siyoqua jinuma!", yell = false},
    {text = "Siqsiq ji jusipa!", yell = false},
    {text = "Jagura taluka taqua!", yell = false},
    {text = "Quatu nguraka!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 26,
    },
    {
        id = "brass helmet",
        chance = 3250,
    },
    {
        id = "bone shield",
        chance = 950,
    },
    {
        id = 2667,
        chance = 29000,
        maxCount = 3,
    },
    {
        id = "northern pike",
        chance = 60,
    },
    {
        id = "rainbow trout",
        chance = 60,
    },
    {
        id = "green perch",
        chance = 60,
    },
}

mtype:register(monster)
