local mtype = Game.createMonsterType("Sandcrawler")
local monster = {}

monster.name = "Sandcrawler"
monster.description = "a sandcrawler"
monster.experience = 20
monster.race = "venom"
monster.maxHealth = 30
monster.health = 30
monster.speed = 170
monster.manaCost = 250
monster.corpse = 11357
monster.outfit = { lookType = 350 }
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = false,
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
        maxDamage = -3,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -5},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Chrk chrk!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 33333,
        maxCount = 6,
    },
    {
        id = "sandcrawler shell",
        chance = 2173,
    },
}

mtype:register(monster)
