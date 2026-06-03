local mtype = Game.createMonsterType("Heoni")
local monster = {}

monster.name = "Heoni"
monster.description = "Heoni"
monster.experience = 515
monster.race = "blood"
monster.maxHealth = 900
monster.health = 900
monster.speed = 300
monster.manaCost = 0
monster.corpse = 6302
monster.outfit = { lookType = 239 }
monster.runHealth = 300
monster.changeTarget = {
    interval = 2000,
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -120,
        -- NOTE: melee condition (poison=480); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 480000,
        },
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 15,
        minDamage = -20,
        maxDamage = -240,
        length = 8,
        spread = 3,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 13,
        length = 8,
        spread = 3,
        duration = 25000,
        effect = CONST_ME_SOUND_WHITE,
    },
}
monster.defenses = {
    defense = 18,
    armor = 25,
    {
        name = "healing",
        interval = 2000,
        chance = 11,
        minDamage = 76,
        maxDamage = 84,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        speed = 290,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 10,
    {text = "Shriiiek", yell = false},
}

mtype:register(monster)
