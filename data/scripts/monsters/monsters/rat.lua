local mtype = Game.createMonsterType("Rat")
local monster = {}

monster.name = "Rat"
monster.description = "a rat"
monster.experience = 5
monster.race = "blood"
monster.maxHealth = 20
monster.health = 20
monster.speed = 134
monster.manaCost = 200
monster.corpse = 5964
monster.outfit = { lookType = 21 }
monster.runHealth = 5
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
        maxDamage = -8,
    },
}
monster.defenses = {
    defense = 1,
    armor = 1,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Meep!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 4,
    },
    {
        id = 2696,
        chance = 39410,
    },
}

mtype:register(monster)
