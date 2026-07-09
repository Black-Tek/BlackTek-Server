local mtype = Game.createMonsterType("Undead Minion")
local monster = {}

monster.name = "Undead Minion"
monster.description = "Undead Minion"
monster.experience = 550
monster.race = "undead"
monster.maxHealth = 850
monster.health = 850
monster.speed = 230
monster.manaCost = 620
monster.corpse = 5963
monster.outfit = { lookType = 37 }
monster.changeTarget = {
    interval = 5000,
    chance = 1,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 1000,
        minDamage = 0,
        maxDamage = -248,
    },
    {
        name = "lifedrain",
        interval = 1000,
        chance = 13,
        minDamage = -100,
        maxDamage = -160,
        radius = 4,
        target = false,
        effect = CONST_ME_MORTAREA,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "drunk", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 40,
    },
    {
        id = "blank rune",
        chance = 10000,
    },
    {
        id = "battle hammer",
        chance = 5000,
    },
    {
        id = "battle shield",
        chance = 1000,
    },
    {
        id = "guardian shield",
        chance = 5000,
    },
    {
        id = 6570,
        chance = 5538,
    },
    {
        id = 6571,
        chance = 1538,
    },
}

mtype:register(monster)
