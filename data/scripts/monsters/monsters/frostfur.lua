local mtype = Game.createMonsterType("Frostfur")
local monster = {}

monster.name = "Frostfur"
monster.description = "Frostfur"
monster.experience = 35
monster.race = "blood"
monster.maxHealth = 65
monster.health = 65
monster.speed = 170
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 52 }
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
        maxDamage = -30,
    },
}
monster.defenses = {
    defense = 3,
    armor = 2,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}

mtype:register(monster)
