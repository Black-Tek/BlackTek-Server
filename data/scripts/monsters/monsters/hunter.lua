local mtype = Game.createMonsterType("Hunter")
local monster = {}

monster.name = "Hunter"
monster.description = "a hunter"
monster.experience = 150
monster.race = "blood"
monster.maxHealth = 150
monster.health = 150
monster.speed = 210
monster.manaCost = 530
monster.corpse = 20419
monster.outfit = {
    lookType = 129,
    lookHead = 95,
    lookBody = 116,
    lookLegs = 120,
    lookFeet = 115,
}
monster.runHealth = 10
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
        maxDamage = -20,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 50,
        range = 7,
        minDamage = 0,
        maxDamage = -100,
        shootEffect = CONST_ANI_ARROW,
    },
}
monster.defenses = {
    defense = 8,
    armor = 8,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Guess who we're hunting, haha!", yell = false},
    {text = "Guess who we are hunting!", yell = false},
    {text = "Bullseye!", yell = false},
    {text = "You'll make a nice trophy!", yell = false},
}
monster.loot = {
    {
        id = 2544,
        chance = 82000,
        maxCount = 22,
    },
    {
        id = "orange",
        chance = 20300,
        maxCount = 2,
    },
    {
        id = "roll",
        chance = 11370,
        maxCount = 2,
    },
    {
        id = "hunter's quiver",
        chance = 10240,
    },
    {
        id = "bow",
        chance = 5770,
    },
    {
        id = 2546,
        chance = 5360,
        maxCount = 3,
    },
    {
        id = "brass armor",
        chance = 5070,
    },
    {
        id = "brass helmet",
        chance = 5050,
    },
    {
        id = "poison arrow",
        chance = 4500,
        maxCount = 4,
    },
    {
        id = "dragon necklace",
        chance = 3000,
    },
    {
        id = 2050,
        chance = 3300,
    },
    {
        id = "sniper gloves",
        chance = 610,
    },
    {
        id = 7397,
        chance = 520,
    },
    {
        id = "small ruby",
        chance = 170,
    },
    {
        id = 7400,
        chance = 140,
    },
    {
        id = 7394,
        chance = 130,
    },
    {
        id = "slingshot",
        chance = 120,
    },
}

mtype:register(monster)
