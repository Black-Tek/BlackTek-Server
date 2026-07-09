local mtype = Game.createMonsterType("Badger")
local monster = {}

monster.name = "Badger"
monster.experience = 5
monster.race = "blood"
monster.maxHealth = 23
monster.health = 23
monster.speed = 140
monster.manaCost = 200
monster.corpse = 6034
monster.outfit = { lookType = 105 }
monster.runHealth = 10
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
        maxDamage = -12,
    },
}
monster.defenses = {
    defense = 1,
    armor = 1,
}
monster.loot = {
    {
        id = "beetroot",
        chance = 40710,
    },
    {
        id = 11216,
        chance = 10230,
    },
    {
        id = "acorn",
        chance = 5130,
    },
}

mtype:register(monster)
