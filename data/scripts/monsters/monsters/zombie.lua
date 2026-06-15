local mtype = Game.createMonsterType("Zombie")
local monster = {}

monster.name = "Zombie"
monster.description = "a zombie"
monster.experience = 280
monster.race = "blood"
monster.maxHealth = 500
monster.health = 500
monster.speed = 180
monster.manaCost = 0
monster.corpse = 9875
monster.outfit = { lookType = 311 }
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
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -130,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        range = 1,
        minDamage = -14,
        maxDamage = -23,
        target = true,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -15,
        maxDamage = -24,
        target = true,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "death",
        interval = 2000,
        chance = 15,
        range = 1,
        minDamage = -30,
        maxDamage = -49,
        target = true,
        effect = CONST_ME_SMALLCLOUDS,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 50},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "drunk", condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Mst.... klll....", yell = false},
    {text = "Whrrrr... ssss.... mmm.... grrrrl", yell = false},
    {text = "Dnnnt... cmmm... clsrrr....", yell = false},
    {text = "Httt.... hmnnsss...", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 82036,
        maxCount = 65,
    },
    {
        id = 2460,
        chance = 9793,
    },
    {
        id = 2398,
        chance = 7847,
    },
    {
        id = 2417,
        chance = 6856,
    },
    {
        id = 9808,
        chance = 5790,
    },
    {
        id = 3351,
        chance = 4799,
    },
    {
        id = 2381,
        chance = 3802,
    },
    {
        id = 2168,
        chance = 960,
    },
    {
        id = 7620,
        chance = 748,
    },
    {
        id = 2657,
        chance = 530,
    },
}

mtype:register(monster)
