local mtype = Game.createMonsterType("Nomad")
local monster = {}

monster.name = "Nomad"
monster.description = "a nomad"
monster.experience = 60
monster.race = "blood"
monster.maxHealth = 160
monster.health = 160
monster.speed = 190
monster.manaCost = 420
monster.corpse = 20603
monster.outfit = {
    lookType = 146,
    lookHead = 85,
    lookBody = 48,
    lookLegs = 49,
    lookFeet = 3,
    lookAddons = 3,
}
monster.runHealth = 15
monster.changeTarget = {
    interval = 2000,
    chance = 2,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        attack = 40,
        skill = 30,
    },
}
monster.defenses = {
    defense = 6,
    armor = 5,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        effect = CONST_ME_SOUND_WHITE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_PHYSICALDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "We are the true sons of the desert!", yell = false},
    {text = "I will leave your remains to the vultures!", yell = false},
    {text = "We are swift as the wind of the desert!", yell = false},
    {text = "Your riches will be mine!", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 56210,
        maxCount = 40,
    },
    {
        id = 8838,
        chance = 4870,
        maxCount = 3,
    },
    {
        id = 12412,
        chance = 2140,
    },
    {
        id = 2386,
        chance = 2730,
    },
    {
        id = 2398,
        chance = 2120,
    },
    {
        id = 12448,
        chance = 6420,
    },
    {
        id = 2465,
        chance = 2350,
    },
    {
        id = 2509,
        chance = 920,
    },
    {
        id = 2459,
        chance = 650,
    },
    {
        id = 8267,
        chance = 210,
    },
}

mtype:register(monster)
