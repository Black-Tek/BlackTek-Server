local mtype = Game.createMonsterType("Pythius The Rotten")
local monster = {}

monster.name = "Pythius The Rotten"
monster.description = "Pythius the rotten"
monster.experience = 7000
monster.race = "undead"
monster.maxHealth = 9500
monster.health = 9500
monster.speed = 350
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 231 }
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
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnFire = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 1950,
        minDamage = 0,
        maxDamage = -210,
    },
    {
        name = "melee",
        interval = 2000,
        chance = 16,
        range = 7,
        minDamage = 0,
        maxDamage = -795,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "physical",
        interval = 2800,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -400,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 17,
        range = 7,
        minDamage = -55,
        maxDamage = -155,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "earth",
        interval = 2500,
        chance = 14,
        minDamage = -333,
        maxDamage = -413,
        length = 8,
        spread = 3,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "death",
        interval = 2000,
        chance = 18,
        minDamage = -165,
        maxDamage = -200,
        length = 7,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "manadrain",
        interval = 2500,
        chance = 22,
        range = 8,
        minDamage = -85,
        maxDamage = -110,
        shootEffect = CONST_ANI_ICE,
    },
    {
        name = "drowncondition",
        interval = 1000,
        chance = 45,
        length = 8,
        spread = 3,
        effect = CONST_ME_POFF,
    },
}
monster.defenses = {
    defense = 45,
    armor = 40,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 3
monster.summons = {
    {name = "Undead Gladiator", interval = 1000, chance = 10, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "YOU'LL NEVER GET MY TREASURE!", yell = true},
    {text = "MINIONS, MEET YOUR NEW BROTHER!", yell = true},
    {text = "YOU WILL REGRET THAT YOU ARE BORN!", yell = true},
    {text = "YOU MADE A HUGE WASTE!", yell = true},
    {text = "I SENSE LIFE", yell = true},
}

mtype:register(monster)
