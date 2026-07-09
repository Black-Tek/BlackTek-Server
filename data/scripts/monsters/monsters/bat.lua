local mtype = Game.createMonsterType("Bat")
local monster = {}

monster.name = "Bat"
monster.experience = 10
monster.race = "blood"
monster.maxHealth = 30
monster.health = 30
monster.speed = 200
monster.manaCost = 250
monster.corpse = 6053
monster.outfit = { lookType = 122 }
monster.runHealth = 3
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
    {type = COMBAT_EARTHDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Flap! Flap!", yell = false},
}
monster.loot = {
    {
        id = "bat wing",
        chance = 1000,
    },
}

mtype:register(monster)
