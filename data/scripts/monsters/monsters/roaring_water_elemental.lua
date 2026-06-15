local mtype = Game.createMonsterType("Roaring Water Elemental")
local monster = {}

monster.name = "Roaring Water Elemental"
monster.description = "a roaring water elemental"
monster.experience = 1300
monster.race = "undead"
monster.maxHealth = 1750
monster.health = 1750
monster.speed = 390
monster.manaCost = 0
monster.corpse = 8965
monster.outfit = { lookType = 11 }
monster.changeTarget = {
    interval = 20000,
    chance = 15,
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
    canWalkOnEnergy = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -225,
    },
    {
        name = "ice",
        interval = 1000,
        chance = 15,
        minDamage = -240,
        maxDamage = -320,
        radius = 2,
        target = true,
        shootEffect = CONST_ANI_ICE,
        effect = CONST_ME_LOSEENERGY,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 90,
        maxDamage = 150,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 50},
    {type = COMBAT_HOLYDAMAGE, percent = 30},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "drown", combat = true, condition = true},
}
monster.loot = {
    {
        id = "small sapphire",
        chance = 4125,
        maxCount = 2,
    },
    {
        id = "gold coin",
        chance = 27000,
        maxCount = 90,
    },
    {
        id = "gold coin",
        chance = 27000,
        maxCount = 87,
    },
    {
        id = "shiver arrow",
        chance = 1000,
        maxCount = 2,
    },
    {
        id = "iced soil",
        chance = 9000,
    },
    {
        id = "northwind rod",
        chance = 750,
    },
}

mtype:register(monster)
