local mtype = Game.createMonsterType("Minotaur Guard")
local monster = {}

monster.name = "Minotaur Guard"
monster.description = "a minotaur guard"
monster.experience = 160
monster.race = "blood"
monster.maxHealth = 185
monster.health = 185
monster.speed = 190
monster.manaCost = 550
monster.corpse = 5983
monster.outfit = { lookType = 29 }
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
        maxDamage = -100,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Kirrl Karrrl!", yell = false},
    {text = "Kaplar", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 59686,
        maxCount = 20,
    },
    {
        id = 12428,
        chance = 8195,
        maxCount = 2,
    },
    {
        id = 12438,
        chance = 4980,
    },
    {
        id = 2465,
        chance = 4125,
    },
    {
        id = 2464,
        chance = 2983,
    },
    {
        id = 2513,
        chance = 2093,
    },
    {
        id = 5878,
        chance = 945,
    },
    {
        id = 2580,
        chance = 481,
    },
    {
        id = 2387,
        chance = 434,
    },
    {
        id = 7618,
        chance = 413,
    },
    {
        id = 7401,
        chance = 94,
    },
}

mtype:register(monster)
