local mtype = Game.createMonsterType("Diseased Fred")
local monster = {}

monster.name = "Diseased Fred"
monster.description = "a diseased Fred"
monster.experience = 300
monster.race = "venom"
monster.maxHealth = 1100
monster.health = 1100
monster.speed = 250
monster.manaCost = 0
monster.corpse = 8951
monster.outfit = { lookType = 299 }
monster.runHealth = 150
monster.changeTarget = {
    interval = 4000,
    chance = 10,
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -150,
        -- NOTE: melee condition (poison=60); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 60000,
        },
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -65,
        maxDamage = -80,
        target = true,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 10,
        minDamage = -100,
        maxDamage = -170,
        radius = 3,
        target = false,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "speed",
        interval = 3000,
        chance = 40,
        target = true,
        speed = -900,
        duration = 20000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 20,
    armor = 20,
    {
        name = "healing",
        interval = 5000,
        chance = 60,
        minDamage = 50,
        maxDamage = 80,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 15},
    {type = COMBAT_ENERGYDAMAGE, percent = 15},
    {type = COMBAT_ICEDAMAGE, percent = 15},
    {type = COMBAT_HOLYDAMAGE, percent = 15},
    {type = COMBAT_DEATHDAMAGE, percent = 55},
    {type = COMBAT_FIREDAMAGE, percent = 15},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "You will suffer the same fate as I do!", yell = false},
    {text = "The pain! The pain!", yell = false},
    {text = "Stay away! I am contagious!", yell = false},
    {text = "The plague will get you!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 43000,
        maxCount = 25,
    },
}

mtype:register(monster)
