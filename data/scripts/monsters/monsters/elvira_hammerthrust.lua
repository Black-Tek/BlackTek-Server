local mtype = Game.createMonsterType("Elvira Hammerthrust")
local monster = {}

monster.name = "Elvira Hammerthrust"
monster.description = "Elvira Hammerthrust"
monster.experience = 165
monster.race = "blood"
monster.maxHealth = 245
monster.health = 245
monster.speed = 103
monster.manaCost = 0
monster.corpse = 6013
monster.outfit = { lookType = 70 }
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
        maxDamage = -140,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Didn't think a dwarf girl could be so pretty and strong at the same time, did you?", yell = false},
    {text = "Hail no one except for me!", yell = false},
    {text = "You won't get this bag, I worked hard for this!", yell = false},
}
monster.loot = {
    {
        id = "bag with stolen gold",
        chance = 100000,
    },
    {
        id = "platinum coin",
        chance = 77000,
        maxCount = 100,
    },
}

mtype:register(monster)
