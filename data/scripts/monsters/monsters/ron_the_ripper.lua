local mtype = Game.createMonsterType("Ron the Ripper")
local monster = {}

monster.name = "Ron the Ripper"
monster.description = "Ron the Ripper"
monster.experience = 500
monster.race = "blood"
monster.maxHealth = 1500
monster.health = 1500
monster.speed = 240
monster.manaCost = 0
monster.corpse = 20502
monster.outfit = {
    lookType = 151,
    lookHead = 95,
    lookBody = 94,
    lookLegs = 117,
    lookFeet = 97,
    lookAddons = 1,
}
monster.runHealth = 250
monster.changeTarget = {
    interval = 60000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 50
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -250,
    },
}
monster.defenses = {
    defense = 50,
    armor = 35,
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 50,
    },
    {
        id = "skull of Ratha",
        chance = 100000,
        maxCount = 2,
    },
    {
        id = "dagger",
        chance = 8500,
    },
    {
        id = "double axe",
        chance = 1500,
    },
    {
        id = "plate armor",
        chance = 4000,
    },
    {
        id = "knight armor",
        chance = 1200,
    },
    {
        id = "Ron the Ripper's sabre",
        chance = 100000,
    },
}

mtype:register(monster)
