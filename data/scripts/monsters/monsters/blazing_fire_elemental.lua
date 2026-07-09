local mtype = Game.createMonsterType("Blazing Fire Elemental")
local monster = {}

monster.name = "Blazing Fire Elemental"
monster.description = "a blazing fire elemental"
monster.experience = 450
monster.race = "fire"
monster.maxHealth = 650
monster.health = 650
monster.speed = 190
monster.manaCost = 0
monster.corpse = 8964
monster.outfit = { lookType = 49 }
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
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -100,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 13,
        minDamage = -65,
        maxDamage = -205,
        radius = 5,
        target = false,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 12,
        range = 7,
        minDamage = -110,
        maxDamage = -150,
        radius = 5,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "firefield",
        interval = 1000,
        chance = 15,
        range = 7,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_FIRE,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = -25},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = 30},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 35000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 35000,
        maxCount = 25,
    },
    {
        id = "fiery heart",
        chance = 6300,
    },
    {
        id = "glimmering soil",
        chance = 4000,
    },
    {
        id = "flaming arrow",
        chance = 3600,
        maxCount = 3,
    },
    {
        id = 13757,
        chance = 740,
    },
}

mtype:register(monster)
