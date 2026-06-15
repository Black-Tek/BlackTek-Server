local mtype = Game.createMonsterType("Energized Raging Mage")
local monster = {}

monster.name = "Energized Raging Mage"
monster.description = "an energized raging mage"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 3500
monster.health = 3500
monster.speed = 230
monster.manaCost = 0
monster.outfit = { lookType = 423 }
monster.runHealth = 300
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 70
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
        maxDamage = -78,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 40,
        range = 7,
        minDamage = -150,
        maxDamage = -230,
        radius = 5,
        target = true,
        shootEffect = CONST_ANI_ENERGYBALL,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -100,
        maxDamage = -200,
    },
    {
        name = "energyfield",
        interval = 2000,
        chance = 20,
        range = 7,
        radius = 2,
        target = true,
        shootEffect = CONST_ANI_ENERGY,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 25,
        minDamage = -200,
        maxDamage = -300,
        length = 8,
        effect = CONST_ME_BIGCLOUDS,
    },
}
monster.defenses = {
    defense = 35,
    armor = 35,
    {
        name = "healing",
        interval = 2000,
        chance = 20,
        minDamage = 155,
        maxDamage = 200,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = -50},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "outfit", condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 1
monster.summons = {
    {name = "golden servant", interval = 2000, chance = 50, max = 0},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Behold the all permeating powers I draw from this gate!!", yell = false},
    {text = "ENERGY!!", yell = false},
    {text = "I FEEL, I FEEEEL... OMNIPOTENCE!!", yell = false},
    {text = "MWAAAHAHAAA!! NO ONE!! NO ONE CAN DEFEAT MEEE!!!", yell = false},
}

mtype:register(monster)
