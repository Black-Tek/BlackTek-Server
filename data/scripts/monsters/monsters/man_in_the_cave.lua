local mtype = Game.createMonsterType("Man in the Cave")
local monster = {}

monster.name = "Man in the Cave"
monster.description = "man in the cave"
monster.experience = 777
monster.race = "blood"
monster.maxHealth = 485
monster.health = 485
monster.speed = 210
monster.manaCost = 0
monster.corpse = 20446
monster.outfit = {
    lookType = 128,
    lookHead = 95,
    lookBody = 116,
    lookLegs = 95,
    lookFeet = 114,
    lookAddons = 2,
}
monster.changeTarget = {
    interval = 5000,
    chance = 8,
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
    canPushCreatures = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -105,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -10,
        maxDamage = -95,
        shootEffect = CONST_ANI_SMALLSTONE,
    },
    {
        name = "lifedrain",
        interval = 1000,
        chance = 100,
        range = 1,
        minDamage = -20,
        maxDamage = -30,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 10,
    armor = 14,
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Monk", interval = 2000, chance = 20, max = 1},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "THE MONKS ARE MINE", yell = true},
    {text = "I will rope you up! All of you!", yell = false},
    {text = "You have been roped up!", yell = false},
    {text = "A MIC to rule them all!", yell = false},
}
monster.loot = {
    {
        id = 2120,
        chance = 20000,
    },
    {
        id = 2120,
        chance = 6666,
    },
    {
        id = 2120,
        chance = 3333,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 27,
    },
    {
        id = "amulet of loss",
        chance = 500,
    },
    {
        id = "brown bread",
        chance = 4000,
    },
    {
        id = "brown piece of cloth",
        chance = 2000,
    },
    {
        id = "shard",
        chance = 4000,
    },
    {
        id = "mercenary sword",
        chance = 6666,
    },
    {
        id = "fur cap",
        chance = 1000,
    },
    {
        id = "mammoth fur cape",
        chance = 6666,
    },
}

mtype:register(monster)
