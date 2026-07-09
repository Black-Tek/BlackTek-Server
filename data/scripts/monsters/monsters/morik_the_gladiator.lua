local mtype = Game.createMonsterType("Morik The Gladiator")
local monster = {}

monster.name = "Morik The Gladiator"
monster.description = "Morik The Gladiator"
monster.experience = 160
monster.race = "blood"
monster.maxHealth = 1235
monster.health = 1235
monster.speed = 200
monster.manaCost = 0
monster.corpse = 20450
monster.outfit = {
    lookType = 131,
    lookHead = 57,
    lookBody = 57,
    lookLegs = 57,
    lookFeet = 57,
    lookAddons = 1,
}
monster.runHealth = 50
monster.changeTarget = {
    interval = 2000,
    chance = 0,
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
        maxDamage = -200,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        radius = 3,
        target = false,
        effect = CONST_ME_HITAREA,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        duration = 4000,
        shootEffect = CONST_ANI_WHIRLWINDCLUB,
        effect = CONST_ME_STUN,
    },
}
monster.defenses = {
    defense = 22,
    armor = 20,
    {
        name = "speed",
        interval = 1000,
        chance = 9,
        speed = 215,
        duration = 5000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_EARTHDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "Gladiator", interval = 2000, chance = 10, max = 0},
}
monster.voices = {
    interval = 1000,
    chance = 2,
    {text = "To be the one you'll have to beat the one!", yell = false},
    {text = "Where did I put my ultimate health potion again?", yell = false},
    {text = "I am the best!", yell = false},
    {text = "I'll take your ears as a trophy!", yell = false},
}
monster.loot = {
    {
        id = "Morik's helmet",
        chance = 100000,
    },
}

mtype:register(monster)
