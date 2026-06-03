local mtype = Game.createMonsterType("Vampire Pig")
local monster = {}

monster.name = "Vampire Pig"
monster.description = "a vampire pig"
monster.experience = 165
monster.race = "blood"
monster.maxHealth = 305
monster.health = 305
monster.speed = 300
monster.manaCost = 0
monster.corpse = 6000
monster.outfit = { lookType = 60 }
monster.runHealth = 30
monster.changeTarget = {
    interval = 2000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = true,
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
        maxDamage = -100,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 30,
        minDamage = -15,
        maxDamage = -25,
        radius = 4,
        target = true,
        effect = CONST_ME_BATS,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        range = 3,
        minDamage = -35,
        maxDamage = -55,
        target = true,
    },
}
monster.defenses = {
    defense = 35,
    armor = 30,
    {
        name = "outfit",
        interval = 2000,
        chance = 20,
        duration = 1500,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "drown", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Oink", yell = false},
    {text = "Oink oink", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 90000,
        maxCount = 40,
    },
}

mtype:register(monster)
