local mtype = Game.createMonsterType("Thalas")
local monster = {}

monster.name = "Thalas"
monster.description = "Thalas"
monster.experience = 2950
monster.race = "undead"
monster.maxHealth = 4100
monster.health = 4100
monster.speed = 320
monster.manaCost = 0
monster.corpse = 6025
monster.outfit = { lookType = 90 }
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -900,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 25,
        range = 7,
        minDamage = -150,
        maxDamage = -650,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "melee",
        interval = 3000,
        chance = 20,
        range = 7,
        minDamage = -150,
        maxDamage = -650,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 6,
        range = 7,
        speed = -800,
        duration = 20000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "poisoncondition",
        interval = 1000,
        chance = 15,
        minDamage = -34,
        maxDamage = -35,
        radius = 5,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "earth",
        interval = 3000,
        chance = 17,
        minDamage = -55,
        maxDamage = -550,
        length = 8,
        spread = 3,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 40,
    armor = 40,
    {
        name = "healing",
        interval = 1000,
        chance = 20,
        minDamage = 150,
        maxDamage = 450,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -20},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 8
monster.summons = {
    {name = "Slime", interval = 2000, chance = 100, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "You will become a feast for my maggots!", yell = false},
}
monster.loot = {
    {
        id = "cobrafang dagger",
        chance = 100000,
    },
    {
        id = "gold coin",
        chance = 30000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 30000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 30000,
        maxCount = 38,
    },
    {
        id = "poison dagger",
        chance = 21000,
    },
    {
        id = "small emerald",
        chance = 9000,
        maxCount = 3,
    },
    {
        id = "great health potion",
        chance = 8200,
    },
    {
        id = 2169,
        chance = 5200,
    },
    {
        id = "serpent sword",
        chance = 2700,
    },
    {
        id = "djinn blade",
        chance = 1000,
    },
    {
        id = "green gem",
        chance = 100,
    },
}

mtype:register(monster)
