local mtype = Game.createMonsterType("Rorc")
local monster = {}

monster.name = "Rorc"
monster.description = "a rorc"
monster.experience = 105
monster.race = "blood"
monster.maxHealth = 260
monster.health = 260
monster.speed = 200
monster.manaCost = 0
monster.corpse = 21223
monster.outfit = { lookType = 550 }
monster.changeTarget = {
    interval = 5000,
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
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
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
    defense = 25,
    armor = 25,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 40,
        maxDamage = 55,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 300,
        duration = 3000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_EARTHDAMAGE, percent = -20},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Kroaaah!!", yell = false},
}
monster.loot = {
    {
        id = "wolf tooth chain",
        chance = 7410,
    },
    {
        id = "gold coin",
        chance = 73640,
        maxCount = 25,
    },
    {
        id = "obsidian lance",
        chance = 930,
    },
    {
        id = "orcish axe",
        chance = 2040,
    },
    {
        id = "plate shield",
        chance = 6700,
    },
    {
        id = "rorc feather",
        chance = 12520,
    },
    {
        id = "rorc egg",
        chance = 790,
    },
    {
        id = "hatched rorc egg",
        chance = 12670,
    },
}

mtype:register(monster)
