local mtype = Game.createMonsterType("Mad Technomancer")
local monster = {}

monster.name = "Mad Technomancer"
monster.description = "a mad technomancer"
monster.experience = 55
monster.race = "blood"
monster.maxHealth = 1800
monster.health = 1800
monster.speed = 200
monster.manaCost = 0
monster.corpse = 6015
monster.outfit = { lookType = 66 }
monster.runHealth = 150
monster.changeTarget = {
    interval = 500,
    chance = 25,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -120,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 10,
        range = 7,
        minDamage = -50,
        maxDamage = -120,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "physical",
        interval = 1000,
        chance = 34,
        range = 7,
        minDamage = -55,
        maxDamage = -105,
        shootEffect = CONST_ANI_LARGEROCK,
    },
    {
        name = "manadrain",
        interval = 1000,
        chance = 25,
        range = 7,
        minDamage = -50,
        maxDamage = -80,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 1000,
        chance = 50,
        minDamage = 75,
        maxDamage = 325,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 80},
    {type = COMBAT_FIREDAMAGE, percent = 60},
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "I'm going to make them an offer they can't refuse.", yell = false},
    {text = "My masterplan cannot fail!", yell = false},
    {text = "Gentlemen, you can't fight here! This is the War Room!", yell = false},
}
monster.loot = {
    {
        id = "technomancer beard",
        chance = 100000,
    },
}

mtype:register(monster)
