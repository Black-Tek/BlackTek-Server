local mtype = Game.createMonsterType("Spirit of Water")
local monster = {}

monster.name = "Spirit of Water"
monster.description = "a spirit of water"
monster.experience = 850
monster.race = "undead"
monster.maxHealth = 1400
monster.health = 1400
monster.speed = 200
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 11 }
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
        maxDamage = -400,
    },
    {
        name = "poisonfield",
        interval = 1000,
        chance = 50,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 40,
        minDamage = -1,
        maxDamage = -120,
        spread = 3,
        effect = CONST_ME_LOSEENERGY,
    },
}
monster.defenses = {
    defense = 0,
    armor = 0,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 5,
    {text = "Blubb", yell = false},
}

mtype:register(monster)
