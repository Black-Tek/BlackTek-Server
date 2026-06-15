local mtype = Game.createMonsterType("Haunted Treeling")
local monster = {}

monster.name = "Haunted Treeling"
monster.description = "a haunted treeling"
monster.experience = 310
monster.race = "undead"
monster.maxHealth = 450
monster.health = 450
monster.speed = 230
monster.manaCost = 0
monster.corpse = 9867
monster.outfit = { lookType = 310 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 85
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
        maxDamage = -150,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 5,
        minDamage = -30,
        maxDamage = -100,
        radius = 4,
        target = false,
        effect = CONST_ME_GREEN_RINGS,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        length = 5,
        speed = -700,
        duration = 15000,
        effect = CONST_ME_SMALLPLANTS,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -100,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_SMALLEARTH,
        effect = CONST_ME_CARNIPHILA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        minDamage = -55,
        maxDamage = -100,
        radius = 4,
        target = true,
        effect = CONST_ME_HITBYPOISON,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        radius = 1,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Knarrrz", yell = false},
    {text = "Huuhuuhuuuhuuaarrr", yell = false},
    {text = "Knorrrrrr", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 92016,
        maxCount = 95,
    },
    {
        id = 4860,
        chance = 29201,
    },
    {
        id = 2788,
        chance = 7814,
    },
    {
        id = 7618,
        chance = 5051,
    },
    {
        id = 2787,
        chance = 5033,
        maxCount = 2,
    },
    {
        id = 10600,
        chance = 4983,
    },
    {
        id = 2790,
        chance = 1817,
    },
    {
        id = 7588,
        chance = 1081,
    },
    {
        id = 2213,
        chance = 655,
    },
    {
        id = 2149,
        chance = 646,
    },
    {
        id = 7443,
        chance = 103,
    },
}

mtype:register(monster)
