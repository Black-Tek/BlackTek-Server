local mtype = Game.createMonsterType("Grimgor Guteater")
local monster = {}

monster.name = "Grimgor Guteater"
monster.description = "Grimgor Guteater"
monster.experience = 670
monster.race = "blood"
monster.maxHealth = 1115
monster.health = 1115
monster.speed = 240
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 2 }
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
        maxDamage = -230,
    },
    {
        name = "physical",
        interval = 4000,
        chance = 60,
        minDamage = -55,
        maxDamage = -130,
        shootEffect = CONST_ANI_THROWINGSTAR,
    },
}
monster.defenses = {
    defense = 45,
    armor = 45,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "Dont run, You're burning off precious fat.", yell = false},
}

mtype:register(monster)
