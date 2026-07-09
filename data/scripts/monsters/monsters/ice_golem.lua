local mtype = Game.createMonsterType("Ice Golem")
local monster = {}

monster.name = "Ice Golem"
monster.description = "an ice golem"
monster.experience = 295
monster.race = "undead"
monster.maxHealth = 385
monster.health = 385
monster.speed = 195
monster.manaCost = 0
monster.corpse = 7282
monster.outfit = { lookType = 261 }
monster.changeTarget = {
    interval = 2000,
    chance = 5,
}
monster.targetDistance = 1
monster.staticAttackChance = 50
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -220,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 13,
        length = 8,
        speed = -800,
        duration = 20000,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "ice",
        interval = 1000,
        chance = 15,
        range = 7,
        minDamage = -50,
        maxDamage = -85,
        shootEffect = CONST_ANI_SMALLICE,
        effect = CONST_ME_ICEATTACK,
    },
    {
        name = "ice golem skill reducer",
        interval = 2000,
        chance = 10,
    },
}
monster.defenses = {
    defense = 26,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 25},
    {type = COMBAT_ENERGYDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "holy", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "invisible", condition = true},
    {type = "paralyze", condition = true},
    {type = "drunk", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "Chrrr.", yell = false},
    {text = "Crrrrk.", yell = false},
    {text = "Gnarr.", yell = false},
}
monster.loot = {
    {
        id = "black pearl",
        chance = 1612,
    },
    {
        id = "small diamond",
        chance = 66,
    },
    {
        id = "small sapphire",
        chance = 578,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 3,
    },
    {
        id = "ice rapier",
        chance = 400,
    },
    {
        id = "strange helmet",
        chance = 444,
    },
    {
        id = "shard",
        chance = 266,
    },
    {
        id = 7441,
        chance = 5000,
    },
    {
        id = "crystal sword",
        chance = 177,
    },
    {
        id = "strong health potion",
        chance = 444,
    },
    {
        id = "glacier mask",
        chance = 111,
    },
    {
        id = "frosty heart",
        chance = 11111,
    },
}

mtype:register(monster)
