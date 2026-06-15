local mtype = Game.createMonsterType("Achad")
local monster = {}

monster.name = "Achad"
monster.description = "Achad"
monster.experience = 70
monster.race = "blood"
monster.maxHealth = 185
monster.health = 185
monster.speed = 185
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = {
    lookType = 146,
    lookHead = 76,
    lookBody = 93,
    lookLegs = 38,
    lookFeet = 97,
    lookAddons = 3,
}
monster.runHealth = 55
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
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -80,
    },
}
monster.defenses = {
    defense = 19,
    armor = 20,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "You won't pass me.", yell = false},
    {text = "I have travelled far to fight here.", yell = false},
}

mtype:register(monster)
