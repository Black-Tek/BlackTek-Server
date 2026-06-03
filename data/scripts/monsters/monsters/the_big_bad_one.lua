local mtype = Game.createMonsterType("The Big Bad One")
local monster = {}

monster.name = "The Big Bad One"
monster.description = "The Big Bad One"
monster.experience = 170
monster.race = "blood"
monster.maxHealth = 300
monster.health = 300
monster.speed = 264
monster.manaCost = 0
monster.corpse = 6009
monster.outfit = { lookType = 3 }
monster.changeTarget = {
    interval = 4000,
    chance = 5,
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
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -100,
    },
}
monster.defenses = {
    defense = 18,
    armor = 18,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Wolf", interval = 2000, chance = 40, max = 0},
}
monster.voices = {
    interval = 1000,
    chance = 2,
    {text = "Grrrrrrr", yell = false},
    {text = "Yoooohhuuuu!", yell = true},
}
monster.loot = {
    {
        id = 7394,
        chance = 50000,
    },
    {
        id = 2671,
        chance = 25000,
    },
    {
        id = "meat",
        chance = 25000,
        maxCount = 4,
    },
    {
        id = "wolf paw",
        chance = 15000,
        maxCount = 2,
    },
    {
        id = "warwolf fur",
        chance = 5000,
    },
}

mtype:register(monster)
