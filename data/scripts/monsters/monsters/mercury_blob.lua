local mtype = Game.createMonsterType("Mercury Blob")
local monster = {}

monster.name = "Mercury Blob"
monster.description = "a mercury blob"
monster.experience = 180
monster.race = "undead"
monster.maxHealth = 150
monster.health = 150
monster.speed = 180
monster.manaCost = 0
monster.corpse = 9961
monster.outfit = { lookType = 316 }
monster.changeTarget = {
    interval = 5000,
    chance = 0,
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
        maxDamage = -75,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        range = 7,
        target = true,
        duration = 3000,
        shootEffect = CONST_ANI_HOLY,
        effect = CONST_ME_STUN,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -10,
        maxDamage = -30,
        target = true,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_ENERGYHIT,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 5,
        minDamage = 20,
        maxDamage = 30,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_EARTHDAMAGE, percent = 65},
    {type = COMBAT_ICEDAMAGE, percent = 15},
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_HOLYDAMAGE, percent = 65},
    {type = COMBAT_LIFEDRAIN, percent = 80},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
}
monster.maxSummons = 3
monster.summons = {
    {name = "Mercury Blob", interval = 2000, chance = 10, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Crackle", yell = false},
}
monster.loot = {
    {
        id = "glob of mercury",
        chance = 17966,
    },
}

mtype:register(monster)
