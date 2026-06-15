local mtype = Game.createMonsterType("Grave Robber")
local monster = {}

monster.name = "Grave Robber"
monster.description = "a grave robber"
monster.experience = 65
monster.race = "blood"
monster.maxHealth = 165
monster.health = 165
monster.speed = 205
monster.manaCost = 355
monster.corpse = 20411
monster.outfit = {
    lookType = 146,
    lookHead = 57,
    lookBody = 95,
    lookLegs = 57,
    lookFeet = 19,
    lookAddons = 3,
}
monster.runHealth = 15
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
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
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -100,
        maxDamage = -160,
        radius = 1,
        target = true,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "This crypt is taken!", yell = false},
    {text = "Leave or you are hunted!", yell = false},
    {text = "The treasure will be mine!", yell = false},
    {text = "The X marks the spot!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 54260,
        maxCount = 38,
    },
    {
        id = "axe",
        chance = 2500,
    },
    {
        id = "mace",
        chance = 1700,
    },
    {
        id = "iron helmet",
        chance = 500,
    },
    {
        id = "brass armor",
        chance = 1700,
    },
    {
        id = "steel shield",
        chance = 500,
    },
    {
        id = "nomad parchment",
        chance = 170,
    },
    {
        id = "potato",
        chance = 5680,
        maxCount = 3,
    },
    {
        id = "dirty turban",
        chance = 1700,
    },
    {
        id = "rope belt",
        chance = 7510,
    },
}

mtype:register(monster)
