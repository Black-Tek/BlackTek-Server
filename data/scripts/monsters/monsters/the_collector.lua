local mtype = Game.createMonsterType("The Collector")
local monster = {}

monster.name = "The Collector"
monster.description = "The Collector"
monster.experience = 100
monster.race = "undead"
monster.maxHealth = 340
monster.health = 340
monster.speed = 195
monster.manaCost = 0
monster.corpse = 7282
monster.outfit = { lookType = 261 }
monster.runHealth = 20
monster.changeTarget = {
    interval = 2000,
    chance = 5,
}
monster.targetDistance = 1
monster.staticAttackChance = 50
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -220,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 13,
        length = 8,
        speed = -800,
        duration = 20000,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "physical",
        interval = 1000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -85,
        shootEffect = CONST_ANI_LARGEROCK,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -10,
        maxDamage = -80,
        radius = 3,
        target = false,
        effect = CONST_ME_HITAREA,
    },
}
monster.defenses = {
    defense = 26,
    armor = 25,
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "Leave as long as you can.", yell = false},
}

mtype:register(monster)
