local mtype = Game.createMonsterType("Water Buffalo")
local monster = {}

monster.name = "Water Buffalo"
monster.description = "a water buffalo"
monster.experience = 20
monster.race = "blood"
monster.maxHealth = 390
monster.health = 390
monster.speed = 150
monster.manaCost = 0
monster.corpse = 19701
monster.outfit = { lookType = 523 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
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
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Gnorrr!", yell = false},
    {text = "Gnarrr!", yell = false},
    {text = "<snort>", yell = false},
    {text = "Mrrr!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 100000,
        maxCount = 20,
    },
    {
        id = 2666,
        chance = 34573,
    },
    {
        id = 2671,
        chance = 33042,
    },
}

mtype:register(monster)
