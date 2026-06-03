local mtype = Game.createMonsterType("The Plasmother")
local monster = {}

monster.name = "The Plasmother"
monster.description = "The Plasmother"
monster.experience = 12000
monster.race = "venom"
monster.maxHealth = 7500
monster.health = 7500
monster.speed = 310
monster.manaCost = 0
monster.corpse = 6532
monster.outfit = { lookType = 238 }
monster.changeTarget = {
    interval = 5500,
    chance = 10,
}
monster.targetDistance = 1
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -100,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 8,
        radius = 6,
        target = false,
        speed = -800,
        duration = 10000,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 15,
        minDamage = -200,
        maxDamage = -350,
        radius = 4,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "earth",
        interval = 3000,
        chance = 15,
        minDamage = -200,
        maxDamage = -530,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_HITBYPOISON,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 1000,
        chance = 75,
        minDamage = 505,
        maxDamage = 605,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -20},
    {type = COMBAT_ENERGYDAMAGE, percent = 15},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Blubb", yell = false},
}
monster.loot = {
    {
        id = "black pearl",
        chance = 5000,
        maxCount = 3,
    },
    {
        id = "small sapphire",
        chance = 5000,
        maxCount = 3,
    },
    {
        id = "gold coin",
        chance = 20000,
        maxCount = 70,
    },
    {
        id = "gold coin",
        chance = 20000,
        maxCount = 60,
    },
    {
        id = "gold coin",
        chance = 20000,
        maxCount = 45,
    },
    {
        id = "platinum coin",
        chance = 25000,
        maxCount = 13,
    },
    {
        id = "soul orb",
        chance = 35000,
    },
    {
        id = "demonic essence",
        chance = 45000,
    },
    {
        id = "the Plasmother's remains",
        chance = 100000,
    },
}

mtype:register(monster)
