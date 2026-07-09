local mtype = Game.createMonsterType("Crypt Defiler")
local monster = {}

monster.name = "Crypt Defiler"
monster.description = "a crypt defiler"
monster.experience = 70
monster.race = "blood"
monster.maxHealth = 180
monster.health = 180
monster.speed = 205
monster.manaCost = 0
monster.corpse = 20359
monster.outfit = {
    lookType = 146,
    lookHead = 62,
    lookBody = 132,
    lookLegs = 42,
    lookFeet = 75,
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
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
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
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -40,
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
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "I smell gold!", yell = false},
    {text = "You'll make a fine fake-mummy to be sold!", yell = false},
    {text = "Untold riches are awaiting me!", yell = false},
    {text = "I don't like competition", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50900,
        maxCount = 39,
    },
    {
        id = "axe",
        chance = 2730,
    },
    {
        id = "mace",
        chance = 1800,
    },
    {
        id = "iron helmet",
        chance = 450,
    },
    {
        id = "brass armor",
        chance = 2240,
    },
    {
        id = "steel shield",
        chance = 450,
    },
    {
        id = "nomad parchment",
        chance = 220,
    },
    {
        id = "potato",
        chance = 4840,
        maxCount = 3,
    },
    {
        id = "dirty turban",
        chance = 1570,
    },
    {
        id = "rope belt",
        chance = 4040,
    },
}

mtype:register(monster)
