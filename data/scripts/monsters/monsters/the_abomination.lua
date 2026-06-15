local mtype = Game.createMonsterType("The Abomination")
local monster = {}

monster.name = "The Abomination"
monster.description = "The Abomination"
monster.experience = 8500
monster.race = "venom"
monster.maxHealth = 285000
monster.health = 285000
monster.speed = 340
monster.manaCost = 0
monster.corpse = 6532
monster.outfit = { lookType = 238 }
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -600,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 12,
        radius = 6,
        target = false,
        speed = -800,
        duration = 10000,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "earth",
        interval = 1000,
        chance = 9,
        minDamage = -200,
        maxDamage = -650,
        radius = 4,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "lifedrain",
        interval = 1000,
        chance = 11,
        minDamage = -400,
        maxDamage = -900,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_SOUND_GREEN,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 19,
        minDamage = -350,
        maxDamage = -850,
        length = 7,
        shootEffect = CONST_ANI_POISON,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
    {
        name = "healing",
        interval = 1000,
        chance = 75,
        minDamage = 505,
        maxDamage = 605,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 1000,
    chance = 2,
    {text = "DEATH IS INEVITABLE!", yell = false},
    {text = "I AM THE ESSENCE OF DEATH!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 100,
    },
    {
        id = "platinum coin",
        chance = 10000,
        maxCount = 3,
    },
    {
        id = "soul orb",
        chance = 2500,
    },
    {
        id = "demonic essence",
        chance = 2857,
    },
}

mtype:register(monster)
