local mtype = Game.createMonsterType("Giant Spider")
local monster = {}

monster.name = "Giant Spider"
monster.description = "a giant spider"
monster.experience = 12
monster.race = "venom"
monster.maxHealth = 20
monster.health = 20
monster.speed = 152
monster.manaCost = 0
monster.corpse = 5977
monster.outfit = { lookType = 38 }
monster.runHealth = 6
monster.changeTarget = {
    interval = 2000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
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
        maxDamage = -9,
    },
}
monster.defenses = {
    defense = 2,
    armor = 2,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -20},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 65000,
        maxCount = 5,
    },
    {
        id = "spider fangs",
        chance = 950,
    },
}

mtype:register(monster)
