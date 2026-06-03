local mtype = Game.createMonsterType("Nomad")
local monster = {}

monster.name = "Nomad"
monster.description = "a nomad"
monster.experience = 60
monster.race = "blood"
monster.maxHealth = 160
monster.health = 160
monster.speed = 205
monster.manaCost = 420
monster.corpse = 20466
monster.outfit = {
    lookType = 146,
    lookHead = 97,
    lookBody = 39,
    lookLegs = 40,
    lookFeet = 3,
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
    pushable = false,
    canPushItems = true,
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
        maxDamage = -80,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        radius = 1,
        target = false,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "We are the true sons of the desert!", yell = false},
    {text = "I will leave your remains to the vultures!", yell = false},
    {text = "We are swift as the wind of the desert!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 56250,
        maxCount = 40,
    },
    {
        id = "axe",
        chance = 2730,
    },
    {
        id = "mace",
        chance = 2120,
    },
    {
        id = "iron helmet",
        chance = 650,
    },
    {
        id = "brass armor",
        chance = 2350,
    },
    {
        id = "steel shield",
        chance = 920,
    },
    {
        id = "nomad parchment",
        chance = 200,
    },
    {
        id = "potato",
        chance = 4840,
        maxCount = 3,
    },
    {
        id = "dirty turban",
        chance = 2160,
    },
    {
        id = "rope belt",
        chance = 6420,
    },
}

mtype:register(monster)
