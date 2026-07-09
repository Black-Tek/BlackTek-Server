local mtype = Game.createMonsterType("Death Priest")
local monster = {}

monster.name = "Death Priest"
monster.description = "a death priest"
monster.experience = 750
monster.race = "undead"
monster.maxHealth = 800
monster.health = 800
monster.speed = 220
monster.manaCost = 0
monster.corpse = 13975
monster.outfit = {
    lookType = 99,
    lookHead = 95,
    lookBody = 116,
    lookLegs = 119,
    lookFeet = 115,
}
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 70
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
        maxDamage = -80,
    },
    {
        name = "death",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -120,
        maxDamage = -250,
        target = true,
        shootEffect = CONST_ANI_DEATH,
        effect = CONST_ME_MORTAREA,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "healing",
        interval = 2000,
        chance = 20,
        minDamage = 80,
        maxDamage = 200,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 75},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "white pearl",
        chance = 3000,
    },
    {
        id = "gold coin",
        chance = 35000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 35000,
        maxCount = 44,
    },
    {
        id = "scarab coin",
        chance = 10000,
        maxCount = 3,
    },
    {
        id = 2175,
        chance = 6800,
    },
    {
        id = "ring of healing",
        chance = 1000,
    },
    {
        id = "orichalcum pearl",
        chance = 6000,
        maxCount = 4,
    },
    {
        id = "health potion",
        chance = 15000,
    },
    {
        id = "mana potion",
        chance = 15000,
    },
    {
        id = 13739,
        chance = 27000,
    },
}

mtype:register(monster)
