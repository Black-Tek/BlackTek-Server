local mtype = Game.createMonsterType("Golgordan")
local monster = {}

monster.name = "Golgordan"
monster.description = "Golgordan"
monster.experience = 10000
monster.race = "fire"
monster.maxHealth = 40000
monster.health = 40000
monster.speed = 350
monster.manaCost = 0
monster.corpse = 8721
monster.outfit = {
    lookType = 12,
    lookHead = 128,
    lookBody = 62,
    lookLegs = 95,
    lookFeet = 15,
}
monster.changeTarget = {
    interval = 7000,
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
    canPushCreatures = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -575,
    },
    {
        name = "death",
        interval = 3000,
        chance = 15,
        minDamage = -250,
        maxDamage = -850,
        length = 8,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "poisoncondition",
        interval = 1000,
        chance = 11,
        minDamage = -300,
        maxDamage = -350,
        length = 5,
        spread = 2,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "physical",
        interval = 1000,
        chance = 8,
        minDamage = -50,
        maxDamage = -955,
        radius = 6,
        target = false,
        effect = CONST_ME_GROUNDSHAKER,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        minDamage = -50,
        maxDamage = -200,
        radius = 7,
        target = true,
        effect = CONST_ME_FIREAREA,
    },
}
monster.defenses = {
    defense = 54,
    armor = 48,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = 15},
    {type = COMBAT_PHYSICALDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Latrivan your fool", yell = true},
}

mtype:register(monster)
