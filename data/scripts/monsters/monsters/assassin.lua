local mtype = Game.createMonsterType("Assassin")
local monster = {}

monster.name = "Assassin"
monster.description = "an assassin"
monster.experience = 105
monster.race = "blood"
monster.maxHealth = 175
monster.health = 175
monster.speed = 224
monster.manaCost = 450
monster.corpse = 20327
monster.outfit = {
    lookType = 152,
    lookHead = 95,
    lookBody = 95,
    lookLegs = 95,
    lookFeet = 95,
    lookAddons = 3,
}
monster.changeTarget = {
    interval = 4000,
    chance = 0,
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -120,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -40,
        shootEffect = CONST_ANI_THROWINGSTAR,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -120,
        maxDamage = -160,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 17,
    armor = 17,
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 2000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Die!", yell = false},
    {text = "Feel the hand of death!", yell = false},
    {text = "You are on my deathlist!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 83000,
        maxCount = 50,
    },
    {
        id = 2050,
        chance = 30000,
        maxCount = 2,
    },
    {
        id = "knife",
        chance = 9800,
    },
    {
        id = "Throwing Star",
        chance = 7000,
        maxCount = 14,
    },
    {
        id = "viper star",
        chance = 4200,
        maxCount = 7,
    },
    {
        id = "combat knife",
        chance = 4000,
    },
    {
        id = "steel helmet",
        chance = 3230,
    },
    {
        id = "plate shield",
        chance = 1900,
    },
    {
        id = "battle shield",
        chance = 1600,
    },
    {
        id = "steel shield",
        chance = 970,
    },
    {
        id = "leopard armor",
        chance = 480,
    },
    {
        id = "horseman helmet",
        chance = 240,
    },
    {
        id = "small diamond",
        chance = 220,
    },
}

mtype:register(monster)
