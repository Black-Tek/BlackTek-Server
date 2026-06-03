local mtype = Game.createMonsterType("Brutus Bloodbeard")
local monster = {}

monster.name = "Brutus Bloodbeard"
monster.description = "Brutus Bloodbeard"
monster.experience = 795
monster.race = "blood"
monster.maxHealth = 1555
monster.health = 1555
monster.speed = 240
monster.manaCost = 0
monster.corpse = 20478
monster.outfit = { lookType = 98 }
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
        maxDamage = -500,
    },
}
monster.defenses = {
    defense = 50,
    armor = 35,
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 40,
    },
    {
        id = "skull of Ratha",
        chance = 100000,
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
        id = "Brutus Bloodbeard's hat",
        chance = 100000,
    },
}

mtype:register(monster)
