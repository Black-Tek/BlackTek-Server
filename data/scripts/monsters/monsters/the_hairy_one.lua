local mtype = Game.createMonsterType("The Hairy One")
local monster = {}

monster.name = "The Hairy One"
monster.description = "The Hairy One"
monster.experience = 115
monster.race = "blood"
monster.maxHealth = 325
monster.health = 325
monster.speed = 240
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 116 }
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
        maxDamage = -70,
    },
}
monster.defenses = {
    defense = 5,
    armor = 3,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 5},
    {type = COMBAT_FIREDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "Hugah!", yell = false},
    {text = "Ungh! Ungh!", yell = false},
}

mtype:register(monster)
