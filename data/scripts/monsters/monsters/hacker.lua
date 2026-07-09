local mtype = Game.createMonsterType("Hacker")
local monster = {}

monster.name = "Hacker"
monster.description = "a hacker"
monster.experience = 45
monster.race = "blood"
monster.maxHealth = 430
monster.health = 430
monster.speed = 250
monster.manaCost = 0
monster.corpse = 5980
monster.outfit = { lookType = 8 }
monster.runHealth = 429
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -83,
    },
}
monster.defenses = {
    defense = 12,
    armor = 15,
    {
        name = "speed",
        interval = 1000,
        chance = 15,
        speed = 290,
        duration = 6000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "outfit",
        interval = 10000,
        chance = 15,
        duration = 500,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "drunk", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Feel the wrath of me dos attack!", yell = false},
    {text = "You're next!", yell = false},
    {text = "Gimme free gold!", yell = false},
    {text = "Me sooo smart!", yell = false},
    {text = "Me have a cheating link for you!", yell = false},
    {text = "Me is GM!", yell = false},
    {text = "Gimme your password!", yell = false},
    {text = "Me just need the code!", yell = false},
    {text = "Me not stink!", yell = false},
    {text = "Me other char is highlevel!", yell = false},
}
monster.loot = {
    {
        id = 2044,
        chance = 6666,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 12,
    },
    {
        id = "battle axe",
        chance = 5000,
    },
    {
        id = 2381,
        chance = 10000,
    },
    {
        id = "axe",
        chance = 10000,
    },
    {
        id = "war hammer",
        chance = 5000,
    },
    {
        id = 2671,
        chance = 50000,
    },
    {
        id = 6570,
        chance = 5538,
    },
    {
        id = 6571,
        chance = 1538,
    },
}

mtype:register(monster)
