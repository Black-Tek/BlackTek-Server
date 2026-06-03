local mtype = Game.createMonsterType("Dracola")
local monster = {}

monster.name = "Dracola"
monster.description = "Dracola"
monster.experience = 11000
monster.race = "undead"
monster.maxHealth = 16200
monster.health = 16200
monster.speed = 410
monster.manaCost = 0
monster.corpse = 6307
monster.outfit = { lookType = 231 }
monster.changeTarget = {
    interval = 2000,
    chance = 5,
}
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = true,
    canPushCreatures = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -404,
    },
    {
        name = "melee",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -300,
        maxDamage = -395,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "physical",
        interval = 3000,
        chance = 10,
        range = 7,
        minDamage = 0,
        maxDamage = -800,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -120,
        maxDamage = -390,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "earth",
        interval = 1000,
        chance = 23,
        range = 7,
        minDamage = -50,
        maxDamage = -180,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "fire",
        interval = 4000,
        chance = 20,
        minDamage = -500,
        maxDamage = -600,
        length = 8,
        spread = 3,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 20,
        minDamage = -200,
        maxDamage = -685,
        length = 8,
        spread = 3,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "lifedrain",
        interval = 3000,
        chance = 20,
        minDamage = -300,
        maxDamage = -600,
        length = 8,
        spread = 3,
        effect = CONST_ME_MAGIC_GREEN,
    },
    {
        name = "drowncondition",
        interval = 1000,
        chance = 20,
        length = 8,
        spread = 3,
        effect = CONST_ME_POFF,
    },
}
monster.defenses = {
    defense = 39,
    armor = 40,
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "DEATH CAN'T STOP MY HUNGER!", yell = true},
}
monster.loot = {
    {
        id = "platinum coin",
        chance = 20000,
        maxCount = 8,
    },
    {
        id = "demonic essence",
        chance = 1000,
        maxCount = 4,
    },
    {
        id = "Dracola's eye",
        chance = 100000,
    },
}

mtype:register(monster)
