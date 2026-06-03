local mtype = Game.createMonsterType("Barbarian Bloodwalker")
local monster = {}

monster.name = "Barbarian Bloodwalker"
monster.description = "a barbarian bloodwalker"
monster.experience = 195
monster.race = "blood"
monster.maxHealth = 305
monster.health = 305
monster.speed = 236
monster.manaCost = 590
monster.corpse = 20335
monster.outfit = {
    lookType = 255,
    lookHead = 114,
    lookBody = 113,
    lookLegs = 132,
    lookFeet = 94,
}
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
    illusionable = true,
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
        maxDamage = -240,
    },
}
monster.defenses = {
    defense = 9,
    armor = 9,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 240,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_EARTHDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = 50},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "YAAAHEEE!", yell = false},
    {text = "SLAUGHTER!", yell = false},
    {text = "CARNAGE!", yell = false},
    {text = "You can run but you can't hide", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 55130,
        maxCount = 12,
    },
    {
        id = 2458,
        chance = 10639,
    },
    {
        id = 2464,
        chance = 10270,
    },
    {
        id = 2044,
        chance = 8135,
    },
    {
        id = 2381,
        chance = 7013,
    },
    {
        id = 2378,
        chance = 5934,
    },
    {
        id = 2671,
        chance = 4955,
    },
    {
        id = 7618,
        chance = 950,
    },
    {
        id = 5911,
        chance = 504,
    },
    {
        id = 3962,
        chance = 350,
    },
    {
        id = 7290,
        chance = 317,
    },
    {
        id = 7457,
        chance = 86,
    },
}

mtype:register(monster)
