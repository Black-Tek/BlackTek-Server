local mtype = Game.createMonsterType("Cat")
local monster = {}

monster.name = "Cat"
monster.description = "a cat"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 20
monster.health = 20
monster.speed = 190
monster.manaCost = 220
monster.corpse = 7637
monster.outfit = { lookType = 276 }
monster.runHealth = 8
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = false,
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
        maxDamage = 0,
    },
}
monster.defenses = {
    defense = 5,
    armor = 5,
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Mew!", yell = false},
    {text = "Meow!", yell = false},
    {text = "Meow meow!", yell = false},
}

mtype:register(monster)
