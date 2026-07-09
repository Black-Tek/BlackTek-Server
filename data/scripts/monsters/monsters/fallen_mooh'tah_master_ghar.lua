local mtype = Game.createMonsterType("Fallen Mooh'tah Master Ghar")
local monster = {}

monster.name = "Fallen Mooh'tah Master Ghar"
monster.description = "Fallen Mooh'Tah Master Ghar"
monster.experience = 4400
monster.race = "blood"
monster.maxHealth = 7990
monster.health = 7990
monster.speed = 190
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 29 }
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -525,
        -- NOTE: melee condition (poison=18); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 18000,
        },
    },
    {
        name = "fire",
        interval = 6000,
        chance = 30,
        minDamage = -80,
        maxDamage = -300,
        length = 8,
        spread = 3,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "fire",
        interval = 3000,
        chance = 45,
        minDamage = -80,
        maxDamage = -400,
        radius = 5,
        target = true,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "death",
        interval = 4000,
        chance = 30,
        minDamage = -80,
        maxDamage = -270,
        shootEffect = CONST_ANI_DEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "poisoncondition",
        interval = 4500,
        chance = 40,
        range = 10,
        minDamage = -10,
        maxDamage = -200,
        shootEffect = CONST_ANI_POISON,
    },
    {
        name = "earth",
        interval = 5000,
        chance = 30,
        minDamage = -60,
        maxDamage = -300,
        length = 8,
        spread = 3,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 33,
    armor = 30,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 60},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "I will finish you!", yell = false},
    {text = "You are no match for a master of the Mooh'Tha!", yell = false},
    {text = "I might be fallen but you will fall before me!", yell = false},
}

mtype:register(monster)
