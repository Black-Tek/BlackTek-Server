local mtype = Game.createMonsterType("Seagull")
local monster = {}

monster.name = "Seagull"
monster.description = "a seagull"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 25
monster.health = 25
monster.speed = 260
monster.manaCost = 250
monster.corpse = 6076
monster.outfit = { lookType = 223 }
monster.runHealth = 25
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 11
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
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
        maxDamage = -3,
    },
}
monster.defenses = {
    defense = 5,
    armor = 5,
}

mtype:register(monster)
