local mtype = Game.createMonsterType("Shardhead")
local monster = {}

monster.name = "Shardhead"
monster.description = "Shardhead"
monster.experience = 650
monster.race = "undead"
monster.maxHealth = 800
monster.health = 800
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -275,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -90,
        target = true,
        shootEffect = CONST_ANI_ICE,
        effect = CONST_ME_ICEATTACK,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 12,
        length = 8,
        speed = -360,
        duration = 5000,
        effect = CONST_ME_ICEAREA,
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
    {text = "Chrr.", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 99700,
        maxCount = 87,
    },
    {
        id = "shard",
        chance = 40000,
    },
    {
        id = 7441,
        chance = 80000,
    },
    {
        id = "crystal sword",
        chance = 30000,
    },
    {
        id = "strong health potion",
        chance = 100000,
    },
    {
        id = "frosty heart",
        chance = 40000,
    },
}

mtype:register(monster)
