local mtype = Game.createMonsterType("Furious Troll")
local monster = {}

monster.name = "Furious Troll"
monster.description = "a furious troll"
monster.experience = 185
monster.race = "blood"
monster.maxHealth = 245
monster.health = 245
monster.speed = 190
monster.manaCost = 0
monster.corpse = 5960
monster.outfit = { lookType = 15 }
monster.changeTarget = {
    interval = 2000,
    chance = 5,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
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
    defense = 15,
    armor = 15,
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Mechanical Fighter", interval = 2000, chance = 90, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Slice! Slice!", yell = false},
    {text = "DIE!!!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 47000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 47000,
        maxCount = 46,
    },
    {
        id = "platinum coin",
        chance = 6000,
    },
    {
        id = "war hammer",
        chance = 750,
    },
    {
        id = "bunch of troll hair",
        chance = 4400,
    },
}

mtype:register(monster)
