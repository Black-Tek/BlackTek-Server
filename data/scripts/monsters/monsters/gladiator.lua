local mtype = Game.createMonsterType("Gladiator")
local monster = {}

monster.name = "Gladiator"
monster.description = "a gladiator"
monster.experience = 90
monster.race = "blood"
monster.maxHealth = 185
monster.health = 185
monster.speed = 200
monster.manaCost = 470
monster.corpse = 20407
monster.outfit = {
    lookType = 131,
    lookHead = 78,
    lookBody = 3,
    lookLegs = 79,
    lookFeet = 114,
}
monster.runHealth = 10
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = true,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -90,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 215,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_PHYSICALDAMAGE, percent = 15},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "You are no match for me!", yell = false},
    {text = "Feel my prowess", yell = false},
    {text = "Fight!", yell = false},
    {text = "Take this!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 49500,
        maxCount = 30,
    },
    {
        id = 2376,
        chance = 12620,
    },
    {
        id = "mace",
        chance = 11160,
    },
    {
        id = "chain helmet",
        chance = 5200,
    },
    {
        id = "iron helmet",
        chance = 590,
    },
    {
        id = "brass armor",
        chance = 2750,
    },
    {
        id = "steel shield",
        chance = 840,
    },
    {
        id = "plate shield",
        chance = 9950,
    },
    {
        id = "meat",
        chance = 19000,
    },
    {
        id = "belted cape",
        chance = 340,
    },
}

mtype:register(monster)
