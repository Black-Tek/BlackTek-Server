local mtype = Game.createMonsterType("Hairman The Huge")
local monster = {}

monster.name = "Hairman The Huge"
monster.description = "Hairman The Huge"
monster.experience = 335
monster.race = "blood"
monster.maxHealth = 600
monster.health = 600
monster.speed = 230
monster.manaCost = 0
monster.corpse = 6043
monster.outfit = { lookType = 116 }
monster.changeTarget = {
    interval = 5000,
    chance = 14,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -110,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "speed",
        interval = 1000,
        chance = 7,
        speed = 260,
        duration = 3000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.maxSummons = 1
monster.summons = {
    {name = "Kongra", interval = 2000, chance = 25, max = 0},
}
monster.voices = {
    interval = 1000,
    chance = 2,
    {text = "Hugah!", yell = false},
    {text = "Ungh! Ungh!", yell = false},
    {text = "Huaauaauaauaa!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 60,
    },
    {
        id = "power ring",
        chance = 7500,
    },
    {
        id = "protection amulet",
        chance = 3000,
    },
    {
        id = "plate armor",
        chance = 5000,
    },
    {
        id = 2676,
        chance = 7500,
        maxCount = 2,
    },
    {
        id = "ape fur",
        chance = 2500,
    },
}

mtype:register(monster)
