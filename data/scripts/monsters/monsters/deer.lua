local mtype = Game.createMonsterType("Deer")
local monster = {}

monster.name = "Deer"
monster.description = "a deer"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 25
monster.health = 25
monster.speed = 196
monster.manaCost = 260
monster.corpse = 5970
monster.outfit = { lookType = 31 }
monster.runHealth = 25
monster.changeTarget = {
    interval = 4000,
    chance = 20,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = false,
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
        maxDamage = -1,
    },
}
monster.defenses = {
    defense = 2,
    armor = 2,
}
monster.loot = {
    {
        id = 2666,
        chance = 80405,
    },
    {
        id = 2671,
        chance = 51110,
    },
    {
        id = 11214,
        chance = 1063,
    },
}

mtype:register(monster)
