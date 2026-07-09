local mtype = Game.createMonsterType("Sibang")
local monster = {}

monster.name = "Sibang"
monster.description = "a sibang"
monster.experience = 105
monster.race = "blood"
monster.maxHealth = 225
monster.health = 225
monster.speed = 214
monster.manaCost = 0
monster.corpse = 6045
monster.outfit = { lookType = 118 }
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
        maxDamage = -40,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 35,
        range = 7,
        minDamage = 0,
        maxDamage = -55,
        shootEffect = CONST_ANI_SMALLSTONE,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 380,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 25},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = -15},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Eeeeek! Eeeeek", yell = false},
    {text = "Huh! Huh! Huh!", yell = false},
    {text = "Ahhuuaaa!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 88771,
        maxCount = 35,
    },
    {
        id = 2676,
        chance = 30452,
        maxCount = 12,
    },
    {
        id = 1294,
        chance = 29967,
        maxCount = 3,
    },
    {
        id = 2675,
        chance = 19971,
        maxCount = 5,
    },
    {
        id = 12467,
        chance = 4948,
    },
    {
        id = 2678,
        chance = 1968,
        maxCount = 3,
    },
    {
        id = 2682,
        chance = 1001,
    },
    {
        id = 5883,
        chance = 988,
    },
}

mtype:register(monster)
