local mtype = Game.createMonsterType("Lizard Magistratus")
local monster = {}

monster.name = "Lizard Magistratus"
monster.description = "a lizard magistratus"
monster.experience = 2000
monster.race = "blood"
monster.maxHealth = 8000
monster.health = 8000
monster.speed = 200
monster.manaCost = 0
monster.corpse = 6041
monster.outfit = { lookType = 115 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = true,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -60,
    },
    {
        name = "lizard magistratus curse",
        interval = 2000,
        chance = 10,
        range = 5,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 50,
        minDamage = 200,
        maxDamage = 250,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 80},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Shhhhhhhh", yell = false},
}
monster.loot = {
    {
        id = "small ruby",
        chance = 8970,
        maxCount = 5,
    },
    {
        id = "gold coin",
        chance = 77230,
        maxCount = 50,
    },
    {
        id = "platinum coin",
        chance = 13400,
        maxCount = 19,
    },
    {
        id = "lizard leather",
        chance = 220,
    },
    {
        id = "lizard scale",
        chance = 450,
    },
    {
        id = "strong mana potion",
        chance = 6280,
    },
    {
        id = "great mana potion",
        chance = 4480,
    },
}

mtype:register(monster)
