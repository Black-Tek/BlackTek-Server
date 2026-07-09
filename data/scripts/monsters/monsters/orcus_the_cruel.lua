local mtype = Game.createMonsterType("Orcus the Cruel")
local monster = {}

monster.name = "Orcus the Cruel"
monster.description = "Orcus the cruel"
monster.experience = 280
monster.race = "blood"
monster.maxHealth = 480
monster.health = 480
monster.speed = 230
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 59 }
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
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -180,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 40,
        range = 5,
        minDamage = 0,
        maxDamage = -70,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_THROWINGKNIFE,
    },
}
monster.defenses = {
    defense = 40,
    armor = 39,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}

mtype:register(monster)
