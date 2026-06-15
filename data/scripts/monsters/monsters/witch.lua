local mtype = Game.createMonsterType("Witch")
local monster = {}

monster.name = "Witch"
monster.description = "a witch"
monster.experience = 120
monster.race = "blood"
monster.maxHealth = 300
monster.health = 300
monster.speed = 204
monster.manaCost = 0
monster.corpse = 20535
monster.outfit = { lookType = 54 }
monster.runHealth = 30
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -20,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -30,
        maxDamage = -75,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_HITBYFIRE,
    },
    {
        name = "firefield",
        interval = 2000,
        chance = 10,
        range = 7,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 1,
        range = 5,
        target = true,
        duration = 2000,
    },
}
monster.defenses = {
    defense = 8,
    armor = 8,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "energy", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Horax pokti!", yell = false},
    {text = "Herba budinia ex!", yell = false},
    {text = "Hihihihi!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 63913,
        maxCount = 40,
    },
    {
        id = 2687,
        chance = 30001,
        maxCount = 8,
    },
    {
        id = 2129,
        chance = 10121,
    },
    {
        id = 10569,
        chance = 10009,
    },
    {
        id = 2800,
        chance = 9005,
    },
    {
        id = 2643,
        chance = 5017,
    },
    {
        id = 2654,
        chance = 4958,
    },
    {
        id = 2405,
        chance = 3892,
    },
    {
        id = 2199,
        chance = 2515,
    },
    {
        id = 2651,
        chance = 2008,
    },
    {
        id = 2185,
        chance = 1085,
    },
    {
        id = 13537,
        chance = 895,
    },
    {
        id = 2402,
        chance = 556,
    },
    {
        id = 10570,
        chance = 90,
    },
    {
        id = 11211,
        chance = 6,
    },
}

mtype:register(monster)
