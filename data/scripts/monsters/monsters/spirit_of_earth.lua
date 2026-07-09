local mtype = Game.createMonsterType("Spirit of Earth")
local monster = {}

monster.name = "Spirit of Earth"
monster.description = "a spirit of earth"
monster.experience = 800
monster.race = "undead"
monster.maxHealth = 1200
monster.health = 1200
monster.speed = 180
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 67 }
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -180,
    },
}
monster.defenses = {
    defense = 32,
    armor = 30,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 50},
    {type = COMBAT_FIREDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 5,
    {text = "Show your strengh ... or perish.", yell = false},
}

mtype:register(monster)
