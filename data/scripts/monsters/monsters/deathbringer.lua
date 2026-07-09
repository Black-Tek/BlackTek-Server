local mtype = Game.createMonsterType("Deathbringer")
local monster = {}

monster.name = "Deathbringer"
monster.description = "Deathbringer"
monster.experience = 5100
monster.race = "undead"
monster.maxHealth = 8440
monster.health = 8440
monster.speed = 300
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 231 }
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -465,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 10,
        range = 7,
        minDamage = -80,
        maxDamage = -120,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "fire",
        interval = 3000,
        chance = 17,
        minDamage = -300,
        maxDamage = -450,
        length = 8,
        spread = 3,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "earth",
        interval = 2000,
        chance = 12,
        minDamage = -300,
        maxDamage = -450,
        length = 8,
        spread = 3,
        effect = CONST_ME_POISONAREA,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 10,
        minDamage = -80,
        maxDamage = -100,
        radius = 6,
        target = false,
        effect = CONST_ME_POFF,
    },
    {
        name = "lifedrain",
        interval = 3000,
        chance = 25,
        range = 7,
        minDamage = -80,
        maxDamage = -150,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = -15},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "YOU FOOLS WILL PAY!", yell = true},
    {text = "YOU ALL WILL DIE!!", yell = true},
    {text = "DEATH, DESTRUCTION!", yell = true},
    {text = "I will eat your soul!", yell = false},
}

mtype:register(monster)
