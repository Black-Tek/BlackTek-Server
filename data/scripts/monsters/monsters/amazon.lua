local mtype = Game.createMonsterType("Amazon")
local monster = {}

monster.name = "Amazon"
monster.description = "an amazon"
monster.experience = 60
monster.race = "blood"
monster.maxHealth = 110
monster.health = 110
monster.speed = 172
monster.manaCost = 390
monster.corpse = 20323
monster.outfit = {
    lookType = 137,
    lookHead = 113,
    lookBody = 120,
    lookLegs = 95,
    lookFeet = 115,
}
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = true,
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
        maxDamage = -45,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -40,
        shootEffect = CONST_ANI_THROWINGKNIFE,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Yeeee ha!", yell = false},
    {text = "Your head shall be mine!", yell = false},
    {text = "Your head will be mine!", yell = false},
}
monster.loot = {
    {
        id = "dagger",
        chance = 80000,
    },
    {
        id = 2229,
        chance = 80000,
        maxCount = 2,
    },
    {
        id = "gold coin",
        chance = 40000,
        maxCount = 20,
    },
    {
        id = "brown bread",
        chance = 30333,
    },
    {
        id = "sabre",
        chance = 23000,
    },
    {
        id = "girlish hair decoration",
        chance = 10000,
    },
    {
        id = "protective charm",
        chance = 5000,
    },
    {
        id = 2050,
        chance = 1005,
    },
    {
        id = "crystal necklace",
        chance = 250,
    },
    {
        id = "small ruby",
        chance = 120,
    },
}

mtype:register(monster)
