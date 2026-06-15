local mtype = Game.createMonsterType("World Bug")
local monster = {}

monster.name = "World Bug"
monster.experience = 50
monster.race = "venom"
monster.maxHealth = 270
monster.health = 270
monster.speed = 240
monster.manaCost = 0
monster.corpse = 5990
monster.outfit = { lookType = 45 }
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
    canPushItems = false,
    canPushCreatures = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -35,
    },
    {
        name = "energy",
        interval = 1000,
        chance = 13,
        minDamage = -5,
        maxDamage = -35,
        length = 4,
        effect = CONST_ME_ENERGYHIT,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "invisible",
        interval = 4000,
        chance = 27,
        duration = 4000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.maxSummons = 10
monster.summons = {
    {name = "world bug", interval = 1000, chance = 15, max = 0},
}
monster.voices = {
    interval = 2000,
    chance = 7,
    {text = "My father was a year 2k bug.", yell = false},
    {text = "Psst, I'll make you rich.", yell = false},
    {text = "You are bugged ... by me!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 11,
    },
    {
        id = 6570,
        chance = 6000,
    },
    {
        id = 6571,
        chance = 800,
    },
}

mtype:register(monster)
