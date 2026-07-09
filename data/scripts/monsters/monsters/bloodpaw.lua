local mtype = Game.createMonsterType("Bloodpaw")
local monster = {}

monster.name = "Bloodpaw"
monster.description = "Bloodpaw"
monster.experience = 50
monster.race = "blood"
monster.maxHealth = 100
monster.health = 100
monster.speed = 156
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 42 }
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
        maxDamage = -40,
    },
}
monster.defenses = {
    defense = 8,
    armor = 3,
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
