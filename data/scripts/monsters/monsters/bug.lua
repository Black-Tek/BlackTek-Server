local mtype = Game.createMonsterType("Bug")
local monster = {}

monster.name = "Bug"
monster.experience = 18
monster.race = "venom"
monster.maxHealth = 29
monster.health = 29
monster.speed = 160
monster.manaCost = 250
monster.corpse = 5990
monster.outfit = { lookType = 45 }
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = true,
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
        maxDamage = -23,
    },
}
monster.defenses = {
    defense = 2,
    armor = 2,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 6,
    },
    {
        id = 2679,
        chance = 3000,
        maxCount = 3,
    },
}

mtype:register(monster)
