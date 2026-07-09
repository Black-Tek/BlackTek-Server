local mtype = Game.createMonsterType("Squidgy Slime")
local monster = {}

monster.name = "Squidgy Slime"
monster.description = "a squidgy slime"
monster.experience = 55
monster.race = "venom"
monster.maxHealth = 150
monster.health = 150
monster.speed = 120
monster.manaCost = 0
monster.corpse = 1496
monster.outfit = { lookType = 19 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -110,
    },
}
monster.defenses = {
    defense = 3,
    armor = 3,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Blubb", yell = false},
}

mtype:register(monster)
