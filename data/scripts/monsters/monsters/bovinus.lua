local mtype = Game.createMonsterType("Bovinus")
local monster = {}

monster.name = "Bovinus"
monster.description = "Bovinus"
monster.experience = 60
monster.race = "blood"
monster.maxHealth = 150
monster.health = 150
monster.speed = 170
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 25 }
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
        maxDamage = -50,
    },
}
monster.defenses = {
    defense = 11,
    armor = 9,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}

mtype:register(monster)
