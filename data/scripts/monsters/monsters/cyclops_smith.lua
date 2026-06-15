local mtype = Game.createMonsterType("Cyclops Smith")
local monster = {}

monster.name = "Cyclops Smith"
monster.description = "a cyclops smith"
monster.experience = 255
monster.race = "blood"
monster.maxHealth = 435
monster.health = 435
monster.speed = 204
monster.manaCost = 695
monster.corpse = 7740
monster.outfit = { lookType = 277 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
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
        maxDamage = -150,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -70,
        shootEffect = CONST_ANI_WHIRLWINDCLUB,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        duration = 4000,
        shootEffect = CONST_ANI_WHIRLWINDCLUB,
        effect = CONST_ME_STUN,
    },
}
monster.defenses = {
    defense = 28,
    armor = 28,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Outis emoi g' onoma.", yell = false},
    {text = "Whack da humy!", yell = false},
    {text = "Ai humy phary ty kaynon", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 82761,
        maxCount = 70,
    },
    {
        id = 2666,
        chance = 50252,
    },
    {
        id = 10574,
        chance = 10011,
    },
    {
        id = 2513,
        chance = 6211,
    },
    {
        id = 2378,
        chance = 5139,
    },
    {
        id = 2417,
        chance = 5117,
    },
    {
        id = 2510,
        chance = 2099,
    },
    {
        id = 2442,
        chance = 2064,
    },
    {
        id = 2387,
        chance = 977,
    },
    {
        id = 7588,
        chance = 520,
    },
    {
        id = 2490,
        chance = 199,
    },
    {
        id = 7452,
        chance = 142,
    },
    {
        id = 7398,
        chance = 126,
    },
    {
        id = 2209,
        chance = 88,
    },
}

mtype:register(monster)
