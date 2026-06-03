local mtype = Game.createMonsterType("Winter Wolf")
local monster = {}

monster.name = "Winter Wolf"
monster.description = "a winter wolf"
monster.experience = 20
monster.race = "blood"
monster.maxHealth = 30
monster.health = 30
monster.speed = 170
monster.manaCost = 260
monster.corpse = 5997
monster.outfit = { lookType = 52 }
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -20,
    },
}
monster.defenses = {
    defense = 2,
    armor = 2,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Yoooohhuuuu!", yell = false},
}
monster.loot = {
    {
        id = "meat",
        chance = 30000,
        maxCount = 2,
    },
    {
        id = "winter wolf fur",
        chance = 10000,
    },
}

mtype:register(monster)
