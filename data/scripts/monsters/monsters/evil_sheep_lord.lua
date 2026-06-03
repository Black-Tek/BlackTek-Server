local mtype = Game.createMonsterType("Evil Sheep Lord")
local monster = {}

monster.name = "Evil Sheep Lord"
monster.description = "an evil sheep lord"
monster.experience = 340
monster.race = "blood"
monster.maxHealth = 400
monster.health = 400
monster.speed = 300
monster.manaCost = 0
monster.corpse = 5994
monster.outfit = { lookType = 13 }
monster.changeTarget = {
    interval = 2000,
    chance = 20,
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
        interval = 2000,
        minDamage = 0,
        maxDamage = -118,
    },
    {
        name = "outfit",
        interval = 3000,
        chance = 20,
        range = 7,
        target = true,
        duration = 4000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.defenses = {
    defense = 35,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 20,
        minDamage = 50,
        maxDamage = 100,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "outfit",
        interval = 1500,
        chance = 50,
        duration = 3000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_PHYSICALDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.maxSummons = 3
monster.summons = {
    {name = "Evil Sheep", interval = 2000, chance = 30, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "You can COUNT on us!", yell = false},
    {text = "Maeh!", yell = false},
    {text = "I feel you're getting sleepy! Maeh!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 75000,
        maxCount = 60,
    },
}

mtype:register(monster)
