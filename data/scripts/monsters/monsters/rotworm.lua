local mtype = Game.createMonsterType("Rotworm")
local monster = {}

monster.name = "Rotworm"
monster.description = "a rotworm"
monster.experience = 40
monster.race = "blood"
monster.maxHealth = 65
monster.health = 65
monster.speed = 116
monster.manaCost = 305
monster.corpse = 5967
monster.outfit = { lookType = 26 }
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 70
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = true,
    pushable = false,
    canPushItems = false,
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
}
monster.defenses = {
    defense = 8,
    armor = 8,
}
monster.loot = {
    {
        id = 2148,
        chance = 71834,
        maxCount = 17,
    },
    {
        id = 2671,
        chance = 20104,
    },
    {
        id = 2666,
        chance = 20006,
    },
    {
        id = 10609,
        chance = 10004,
    },
    {
        id = 2398,
        chance = 4441,
    },
    {
        id = 3976,
        chance = 3057,
        maxCount = 3,
    },
    {
        id = 2376,
        chance = 3047,
    },
}

mtype:register(monster)
