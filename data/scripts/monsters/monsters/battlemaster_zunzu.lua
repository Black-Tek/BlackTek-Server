local mtype = Game.createMonsterType("Battlemaster Zunzu")
local monster = {}

monster.name = "Battlemaster Zunzu"
monster.description = "Battlemaster Zunzu"
monster.experience = 2500
monster.race = "blood"
monster.maxHealth = 4000
monster.health = 4000
monster.speed = 420
monster.manaCost = 0
monster.corpse = 11281
monster.outfit = { lookType = 343 }
monster.runHealth = 150
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
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -300,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 25,
        range = 1,
        minDamage = -115,
        maxDamage = -350,
        radius = 1,
        target = true,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 35,
    armor = 45,
    {
        name = "healing",
        interval = 1000,
        chance = 18,
        minDamage = 200,
        maxDamage = 400,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 15},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = 25},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 15},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "platinum coin",
        chance = 26800,
        maxCount = 3,
    },
    {
        id = "lizard leather",
        chance = 1825,
    },
    {
        id = "lizard scale",
        chance = 800,
    },
    {
        id = "strong health potion",
        chance = 2875,
        maxCount = 2,
    },
    {
        id = "great health potion",
        chance = 2775,
        maxCount = 2,
    },
    {
        id = "red lantern",
        chance = 100000,
    },
    {
        id = "Zaoan armor",
        chance = 1050,
    },
    {
        id = "Zaoan shoes",
        chance = 3150,
    },
    {
        id = "Zaoan legs",
        chance = 2625,
    },
    {
        id = "zaogun flag",
        chance = 11250,
    },
    {
        id = "zaogun shoulderplates",
        chance = 100000,
    },
}

mtype:register(monster)
