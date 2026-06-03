local mtype = Game.createMonsterType("Diseased Bill")
local monster = {}

monster.name = "Diseased Bill"
monster.description = "a diseased Bill"
monster.experience = 300
monster.race = "venom"
monster.maxHealth = 1000
monster.health = 1000
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
        maxDamage = -240,
        -- NOTE: melee condition (poison=40); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 40000,
        },
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -200,
        maxDamage = -250,
        target = true,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "physical",
        interval = 1000,
        chance = 40,
        minDamage = -100,
        maxDamage = -175,
        radius = 2,
        target = false,
        shootEffect = CONST_ANI_SMALLEARTH,
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
    {type = COMBAT_PHYSICALDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = 25},
    {type = COMBAT_FIREDAMAGE, percent = 25},
    {type = COMBAT_ENERGYDAMAGE, percent = 25},
    {type = COMBAT_ICEDAMAGE, percent = 25},
    {type = COMBAT_DEATHDAMAGE, percent = 75},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "People like you are the plague and I'll be the cure!", yell = false},
    {text = "You all will pay for not helping me!", yell = false},
    {text = "Cough! Cough!", yell = false},
    {text = "Desolate! Everything is so desolate!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 40000,
        maxCount = 25,
    },
}

mtype:register(monster)
