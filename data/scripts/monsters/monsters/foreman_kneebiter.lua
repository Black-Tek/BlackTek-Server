local mtype = Game.createMonsterType("Foreman Kneebiter")
local monster = {}

monster.name = "Foreman Kneebiter"
monster.description = "Foreman Kneebiter"
monster.experience = 445
monster.race = "blood"
monster.maxHealth = 570
monster.health = 570
monster.speed = 200
monster.manaCost = 0
monster.corpse = 6013
monster.outfit = { lookType = 70 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    attackable = true,
    hostile = true,
    canPushItems = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -200,
    },
}
monster.defenses = {
    defense = 22,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 90},
    {type = COMBAT_PHYSICALDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 90000,
        maxCount = 100,
    },
    {
        id = "battle shield",
        chance = 6666,
    },
    {
        id = "iron ore",
        chance = 2500,
        maxCount = 2,
    },
}

mtype:register(monster)
