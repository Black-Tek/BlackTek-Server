local mtype = Game.createMonsterType("Retching Horror")
local monster = {}

monster.name = "Retching Horror"
monster.description = "a retching horror"
monster.experience = 4100
monster.race = "fire"
monster.maxHealth = 5300
monster.health = 5300
monster.speed = 360
monster.manaCost = 0
monster.corpse = 22508
monster.outfit = { lookType = 588 }
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
        maxDamage = -400,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        length = 4,
        spread = 3,
        target = true,
        duration = 5000,
        effect = CONST_ME_MAGIC_GREEN,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        radius = 4,
        target = true,
        speed = -700,
        duration = 15000,
        shootEffect = CONST_ANI_WHIRLWINDCLUB,
        effect = CONST_ME_STUN,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        minDamage = 0,
        maxDamage = -110,
        radius = 4,
        target = false,
        effect = CONST_ME_HITAREA,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        minDamage = 0,
        maxDamage = -200,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_SNIPERARROW,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 10,
        minDamage = -200,
        maxDamage = -350,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREATTACK,
    },
}
monster.defenses = {
    defense = 74,
    armor = 74,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_ICEDAMAGE, percent = 15},
    {type = COMBAT_DEATHDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Wait for us, little maggot...", yell = false},
    {text = "We will devour you...", yell = false},
    {text = "My little beetles, go forth, eat, feast!", yell = false},
}
monster.loot = {
    {
        id = 2152,
        chance = 82759,
        maxCount = 9,
    },
    {
        id = 2148,
        chance = 81103,
        maxCount = 100,
    },
    {
        id = 22539,
        chance = 13379,
    },
    {
        id = 2789,
        chance = 11310,
        maxCount = 2,
    },
    {
        id = 7590,
        chance = 11310,
    },
    {
        id = 22541,
        chance = 11034,
    },
    {
        id = 7591,
        chance = 10759,
    },
    {
        id = 22363,
        chance = 4000,
    },
    {
        id = 3962,
        chance = 3172,
    },
    {
        id = 7386,
        chance = 2207,
    },
    {
        id = 7452,
        chance = 2207,
    },
    {
        id = 2392,
        chance = 1793,
    },
    {
        id = 8910,
        chance = 1241,
    },
    {
        id = 7421,
        chance = 1103,
    },
    {
        id = 8920,
        chance = 1103,
    },
    {
        id = 2519,
        chance = 690,
    },
    {
        id = 7412,
        chance = 414,
    },
    {
        id = 2528,
        chance = 414,
    },
    {
        id = 22396,
        chance = 276,
    },
}

mtype:register(monster)
