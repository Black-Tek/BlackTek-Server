local mtype = Game.createMonsterType("Deepling Brawler")
local monster = {}

monster.name = "Deepling Brawler"
monster.description = "a deepling brawler"
monster.experience = 260
monster.race = "blood"
monster.maxHealth = 380
monster.health = 380
monster.speed = 190
monster.manaCost = 0
monster.corpse = 13840
monster.outfit = { lookType = 470 }
monster.runHealth = 40
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = true,
    canPushCreatures = true,
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
        name = "drown",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -60,
        maxDamage = -120,
        target = true,
        shootEffect = CONST_ANI_SPEAR,
        effect = CONST_ME_LOSEENERGY,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_EARTHDAMAGE, percent = -1},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "paralyze", condition = true},
    {type = "drown", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 61000,
        maxCount = 44,
    },
    {
        id = 2667,
        chance = 19120,
        maxCount = 3,
    },
    {
        id = "fish fin",
        chance = 740,
    },
    {
        id = "heavy trident",
        chance = 2940,
    },
    {
        id = "eye of a deepling",
        chance = 6600,
    },
    {
        id = "deepling scales",
        chance = 14500,
    },
}

mtype:register(monster)
