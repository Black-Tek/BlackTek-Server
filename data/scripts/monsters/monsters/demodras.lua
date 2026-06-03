local mtype = Game.createMonsterType("Demodras")
local monster = {}

monster.name = "Demodras"
monster.description = "Demodras"
monster.experience = 6000
monster.race = "blood"
monster.maxHealth = 4500
monster.health = 4500
monster.speed = 230
monster.manaCost = 0
monster.corpse = 5984
monster.outfit = { lookType = 204 }
monster.runHealth = 300
monster.changeTarget = {
    interval = 5000,
    chance = 8,
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
        maxDamage = -235,
    },
    {
        name = "fire",
        interval = 3000,
        chance = 20,
        range = 7,
        minDamage = -350,
        maxDamage = -400,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "firefield",
        interval = 1000,
        chance = 10,
        range = 7,
        radius = 6,
        target = true,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "fire",
        interval = 4000,
        chance = 20,
        minDamage = -300,
        maxDamage = -550,
        length = 8,
        spread = 3,
        effect = CONST_ME_FIREAREA,
    },
}
monster.defenses = {
    defense = 45,
    armor = 45,
    {
        name = "healing",
        interval = 2000,
        chance = 25,
        minDamage = 400,
        maxDamage = 600,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Dragon", interval = 1000, chance = 17, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "I WILL SET THE WORLD ON FIRE!", yell = true},
    {text = "I WILL PROTECT MY BROOD!", yell = true},
}
monster.loot = {
    {
        id = 5919,
        chance = 100000,
    },
    {
        id = 2152,
        chance = 99145,
        maxCount = 10,
    },
    {
        id = 2672,
        chance = 75214,
    },
    {
        id = 2796,
        chance = 25641,
        maxCount = 7,
    },
    {
        id = 2547,
        chance = 19658,
        maxCount = 10,
    },
    {
        id = 5948,
        chance = 13675,
    },
    {
        id = 2146,
        chance = 11966,
    },
    {
        id = 1976,
        chance = 10256,
    },
    {
        id = 2167,
        chance = 10256,
    },
    {
        id = 7591,
        chance = 9402,
    },
    {
        id = 7590,
        chance = 9402,
    },
    {
        id = 2033,
        chance = 5983,
    },
    {
        id = 7365,
        chance = 4274,
        maxCount = 5,
    },
    {
        id = 2492,
        chance = 1709,
    },
    {
        id = 2392,
        chance = 1709,
    },
    {
        id = 2177,
        chance = 855,
    },
}

mtype:register(monster)
