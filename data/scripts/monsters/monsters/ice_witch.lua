local mtype = Game.createMonsterType("Ice Witch")
local monster = {}

monster.name = "Ice Witch"
monster.description = "an ice witch"
monster.experience = 580
monster.race = "blood"
monster.maxHealth = 650
monster.health = 650
monster.speed = 200
monster.manaCost = 0
monster.corpse = 20423
monster.outfit = {
    lookType = 149,
    lookHead = 0,
    lookBody = 9,
    lookLegs = 86,
    lookFeet = 86,
    lookAddons = 0,
}
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
monster.staticAttackChance = 70
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -60,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 1,
        range = 7,
        duration = 4000,
        shootEffect = CONST_ANI_SNOWBALL,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 10,
        minDamage = -60,
        maxDamage = -130,
        length = 5,
        spread = 2,
        effect = CONST_ME_ICETORNADO,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -55,
        maxDamage = -115,
        target = true,
        shootEffect = CONST_ANI_SMALLICE,
        effect = CONST_ME_ICEATTACK,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        range = 7,
        speed = -600,
        duration = 20000,
        shootEffect = CONST_ANI_SMALLICE,
        effect = CONST_ME_ICETORNADO,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "healing",
        interval = 2000,
        chance = 25,
        minDamage = 90,
        maxDamage = 120,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 50},
    {type = COMBAT_EARTHDAMAGE, percent = 40},
    {type = COMBAT_HOLYDAMAGE, percent = 30},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "So you think you are cool?", yell = false},
    {text = "I hope it is not too cold for you! HeHeHe.", yell = false},
    {text = "Freeze!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 29630,
        maxCount = 90,
    },
    {
        id = "clerical mace",
        chance = 920,
    },
    {
        id = "mystic turban",
        chance = 430,
    },
    {
        id = "green mushroom",
        chance = 1310,
    },
    {
        id = "shard",
        chance = 530,
    },
    {
        id = "diamond sceptre",
        chance = 330,
    },
    {
        id = 7441,
        chance = 10000,
    },
    {
        id = "crystal sword",
        chance = 400,
    },
    {
        id = "pair of earmuffs",
        chance = 90,
    },
    {
        id = "strong mana potion",
        chance = 820,
    },
    {
        id = "glacier shoes",
        chance = 280,
    },
}

mtype:register(monster)
