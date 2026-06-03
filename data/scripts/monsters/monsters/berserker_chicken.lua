local mtype = Game.createMonsterType("Berserker Chicken")
local monster = {}

monster.name = "Berserker Chicken"
monster.experience = 220
monster.race = "blood"
monster.maxHealth = 465
monster.health = 465
monster.speed = 300
monster.manaCost = 220
monster.corpse = 6042
monster.outfit = { lookType = 111 }
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 1200,
        minDamage = 0,
        maxDamage = -200,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 30,
        range = 1,
        minDamage = 0,
        maxDamage = -100,
    },
    {
        name = "drown",
        interval = 2000,
        chance = 15,
        minDamage = -41,
        maxDamage = -70,
        target = false,
        effect = CONST_ME_EXPLOSIONAREA,
    },
}
monster.defenses = {
    defense = 35,
    armor = 30,
    {
        name = "speed",
        interval = 1000,
        chance = 40,
        speed = 400,
        duration = 8000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_EARTHDAMAGE, percent = 10},
    {type = COMBAT_PHYSICALDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "drown", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Gokgoooook", yell = false},
    {text = "Cluck Cluck", yell = false},
    {text = "I will fill MY cushion with YOUR hair! CLUCK!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
}

mtype:register(monster)
