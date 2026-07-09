local mtype = Game.createMonsterType("Lizard Noble")
local monster = {}

monster.name = "Lizard Noble"
monster.description = "a lizard noble"
monster.experience = 2000
monster.race = "blood"
monster.maxHealth = 7000
monster.health = 7000
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
        maxDamage = -30,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 25,
        range = 7,
        minDamage = -120,
        maxDamage = -250,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -100,
        effect = CONST_ME_MAGIC_BLUE,
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
    {type = COMBAT_FIREDAMAGE, percent = 90},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Where are zhe guardz when you need zhem!", yell = false},
}
monster.loot = {
    {
        id = "small ruby",
        chance = 7100,
        maxCount = 5,
    },
    {
        id = "gold coin",
        chance = 91300,
        maxCount = 100,
    },
    {
        id = "platinum coin",
        chance = 10000,
        maxCount = 20,
    },
    {
        id = "lizard leather",
        chance = 220,
    },
    {
        id = "lizard scale",
        chance = 650,
    },
    {
        id = "strong health potion",
        chance = 2550,
    },
    {
        id = "great health potion",
        chance = 2900,
    },
}

mtype:register(monster)
