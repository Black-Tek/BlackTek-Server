local mtype = Game.createMonsterType("mad mage")
local monster = {}

monster.name = "mad mage"
monster.description = "a mad mage"
monster.experience = 1800
monster.race = "blood"
monster.maxHealth = 2500
monster.health = 2500
monster.speed = 240
monster.manaCost = 0
monster.corpse = 13603
monster.outfit = { lookType = 394 }
monster.runHealth = 1000
monster.changeTarget = {
    interval = 5000,
    chance = 30,
}
monster.targetDistance = 4
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
        maxDamage = -78,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 12,
        range = 7,
        minDamage = -50,
        maxDamage = -185,
        target = true,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "manadrain",
        interval = 1400,
        chance = 24,
        range = 6,
        minDamage = -30,
        maxDamage = -90,
        shootEffect = CONST_ANI_ICE,
    },
    {
        name = "firefield",
        interval = 1600,
        chance = 20,
        range = 7,
        radius = 2,
        target = true,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "energy",
        interval = 1900,
        chance = 18,
        minDamage = -70,
        maxDamage = -150,
        length = 8,
        effect = CONST_ME_BIGCLOUDS,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "healing",
        interval = 1000,
        chance = 25,
        minDamage = 35,
        maxDamage = 80,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = -20},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
    {type = COMBAT_EARTHDAMAGE, percent = 50},
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 1
monster.summons = {
    {name = "stone golem", interval = 1000, chance = 10, max = 0},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 66,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 20,
    },
    {
        id = "dark mushroom",
        chance = 5000,
        maxCount = 5,
    },
    {
        id = "red piece of cloth",
        chance = 800,
    },
    {
        id = "assassin star",
        chance = 2400,
        maxCount = 4,
    },
    {
        id = "strong health potion",
        chance = 1300,
        maxCount = 5,
    },
    {
        id = "strong mana potion",
        chance = 1200,
        maxCount = 2,
    },
    {
        id = "lightning legs",
        chance = 800,
    },
}

mtype:register(monster)
