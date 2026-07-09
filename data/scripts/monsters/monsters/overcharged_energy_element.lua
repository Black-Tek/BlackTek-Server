local mtype = Game.createMonsterType("Overcharged Energy Element")
local monster = {}

monster.name = "Overcharged Energy Element"
monster.description = "an orvercharged energy element"
monster.experience = 1300
monster.race = "energy"
monster.maxHealth = 1700
monster.health = 1700
monster.speed = 300
monster.manaCost = 0
monster.corpse = 8966
monster.outfit = { lookType = 290 }
monster.changeTarget = {
    interval = 20000,
    chance = 15,
}
monster.targetDistance = 1
monster.staticAttackChance = 85
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -200,
    },
    {
        name = "energy",
        interval = 1000,
        chance = 11,
        minDamage = 0,
        maxDamage = -250,
        radius = 4,
        target = false,
        shootEffect = CONST_ANI_ENERGY,
        effect = CONST_ME_PURPLEENERGY,
    },
    {
        name = "energy",
        interval = 1000,
        chance = 12,
        range = 3,
        minDamage = 0,
        maxDamage = -300,
        target = true,
        effect = CONST_ME_PURPLEENERGY,
    },
    {
        name = "physical",
        interval = 1000,
        chance = 12,
        minDamage = 0,
        maxDamage = -200,
        radius = 4,
        target = false,
        effect = CONST_ME_POFF,
    },
}
monster.defenses = {
    defense = 35,
    armor = 35,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 90,
        maxDamage = 150,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "BZZZZZZZZZZ", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 56,
    },
    {
        id = "small amethyst",
        chance = 10000,
        maxCount = 2,
    },
    {
        id = "berserk potion",
        chance = 2173,
    },
    {
        id = "great health potion",
        chance = 10000,
    },
    {
        id = "energy soil",
        chance = 14285,
    },
}

mtype:register(monster)
