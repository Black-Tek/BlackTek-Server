local mtype = Game.createMonsterType("Stampor")
local monster = {}

monster.name = "Stampor"
monster.description = "a stampor"
monster.experience = 780
monster.race = "blood"
monster.maxHealth = 1200
monster.health = 1200
monster.speed = 240
monster.manaCost = 0
monster.corpse = 13312
monster.outfit = { lookType = 381 }
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
        maxDamage = -130,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        minDamage = -150,
        maxDamage = -280,
        radius = 3,
        target = false,
        effect = CONST_ME_GROUNDSHAKER,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        minDamage = -75,
        maxDamage = -100,
        target = true,
        shootEffect = CONST_ANI_SMALLSTONE,
    },
    {
        name = "stampor skill reducer",
        interval = 2000,
        chance = 10,
        range = 5,
    },
}
monster.defenses = {
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 90,
        maxDamage = 120,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 50},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "KRRRRRNG", yell = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 10000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 10000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 10000,
        maxCount = 42,
    },
    {
        id = "platinum coin",
        chance = 9920,
        maxCount = 2,
    },
    {
        id = "war hammer",
        chance = 1010,
    },
    {
        id = "knight armor",
        chance = 870,
    },
    {
        id = "spiked squelcher",
        chance = 160,
    },
    {
        id = "strong health potion",
        chance = 5000,
        maxCount = 2,
    },
    {
        id = "strong mana potion",
        chance = 5000,
        maxCount = 2,
    },
    {
        id = "small topaz",
        chance = 7940,
        maxCount = 2,
    },
    {
        id = "stampor horn",
        chance = 4920,
    },
    {
        id = "stampor talons",
        chance = 9950,
        maxCount = 2,
    },
    {
        id = "hollow stampor hoof",
        chance = 3020,
    },
}

mtype:register(monster)
