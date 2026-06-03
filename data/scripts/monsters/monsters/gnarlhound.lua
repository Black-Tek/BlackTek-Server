local mtype = Game.createMonsterType("Gnarlhound")
local monster = {}

monster.name = "Gnarlhound"
monster.description = "a gnarlhound"
monster.experience = 60
monster.race = "blood"
monster.maxHealth = 198
monster.health = 198
monster.speed = 280
monster.manaCost = 465
monster.corpse = 11250
monster.outfit = { lookType = 341 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = false,
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
        maxDamage = -70,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Gnarllll!", yell = false},
    {text = "Grrrrrr!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 48000,
        maxCount = 30,
    },
    {
        id = "meat",
        chance = 39075,
        maxCount = 3,
    },
    {
        id = "worm",
        chance = 33300,
        maxCount = 3,
    },
    {
        id = "shaggy tail",
        chance = 25550,
    },
}

mtype:register(monster)
