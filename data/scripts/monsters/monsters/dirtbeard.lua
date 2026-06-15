local mtype = Game.createMonsterType("Dirtbeard")
local monster = {}

monster.name = "Dirtbeard"
monster.description = "Dirtbeard"
monster.experience = 375
monster.race = "blood"
monster.maxHealth = 630
monster.health = 630
monster.speed = 300
monster.manaCost = 775
monster.corpse = 10320
monster.outfit = { lookType = 98 }
monster.runHealth = 50
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
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 1200,
        minDamage = 0,
        maxDamage = -125,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 30,
        range = 7,
        minDamage = 0,
        maxDamage = -100,
        shootEffect = CONST_ANI_THROWINGSTAR,
    },
    {
        name = "melee",
        interval = 1200,
        chance = 30,
        range = 7,
        radius = 3,
        target = false,
        effect = CONST_ME_SOUND_RED,
    },
}
monster.defenses = {
    defense = 35,
    armor = 30,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = -5},
    {type = COMBAT_FIREDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.maxSummons = 2
monster.summons = {
    {name = "pirate marauder", interval = 4000, chance = 30, max = 2},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "You are no match for the scourge of the seas!", yell = false},
    {text = "You move like a seasick whale!", yell = false},
    {text = "Yarr, death to all landlubbers!", yell = false},
}

mtype:register(monster)
