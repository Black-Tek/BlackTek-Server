local mtype = Game.createMonsterType("Corym Skirmisher")
local monster = {}

monster.name = "Corym Skirmisher"
monster.description = "a corym skirmisher"
monster.experience = 260
monster.race = "blood"
monster.maxHealth = 450
monster.health = 450
monster.speed = 200
monster.manaCost = 695
monster.corpse = 19726
monster.outfit = {
    lookType = 533,
    lookHead = 0,
    lookBody = 76,
    lookLegs = 83,
    lookFeet = 0,
}
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
    illusionable = true,
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -110,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -90,
        shootEffect = CONST_ANI_WHIRLWINDCLUB,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Squeak! Squeak!", yell = false},
    {text = "<sniff><sniff><sniff>", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 45,
    },
    {
        id = 2696,
        chance = 20000,
    },
    {
        id = "bola",
        chance = 7692,
    },
    {
        id = "spike shield",
        chance = 645,
    },
    {
        id = "ratana",
        chance = 2702,
    },
    {
        id = "life preserver",
        chance = 2941,
    },
    {
        id = "cheese cutter",
        chance = 8333,
    },
    {
        id = "cheesy figurine",
        chance = 854,
    },
    {
        id = "earflap",
        chance = 9090,
    },
    {
        id = "soft cheese",
        chance = 14285,
    },
    {
        id = "rat cheese",
        chance = 16666,
    },
    {
        id = "rat god doll",
        chance = 12,
    },
    {
        id = "leather harness",
        chance = 561,
    },
}

mtype:register(monster)
