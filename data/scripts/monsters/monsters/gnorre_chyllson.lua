local mtype = Game.createMonsterType("Gnorre Chyllson")
local monster = {}

monster.name = "Gnorre Chyllson"
monster.description = "Gnorre Chyllson"
monster.experience = 4000
monster.race = "blood"
monster.maxHealth = 7150
monster.health = 7150
monster.speed = 370
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = {
    lookType = 251,
    lookHead = 11,
    lookBody = 9,
    lookLegs = 11,
    lookFeet = 85,
    lookAddons = 1,
}
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
        maxDamage = -455,
    },
    {
        name = "melee",
        interval = 3000,
        chance = 50,
        minDamage = -500,
        maxDamage = -800,
        target = false,
        effect = CONST_ME_HITAREA,
    },
    {
        name = "ice",
        interval = 1000,
        chance = 15,
        range = 7,
        minDamage = -170,
        maxDamage = -200,
        shootEffect = CONST_ANI_SNOWBALL,
    },
}
monster.defenses = {
    defense = 52,
    armor = 51,
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = -10},
    {type = COMBAT_PHYSICALDAMAGE, percent = -15},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "I am like the merciless northwind.", yell = false},
    {text = "Snow will be your death shroud.", yell = false},
    {text = "Feel the wrath of father chyll!", yell = false},
}

mtype:register(monster)
