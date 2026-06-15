local mtype = Game.createMonsterType("Doomhowl")
local monster = {}

monster.name = "Doomhowl"
monster.description = "Doomhowl"
monster.experience = 3750
monster.race = "blood"
monster.maxHealth = 8500
monster.health = 8500
monster.speed = 320
monster.manaCost = 0
monster.corpse = 6080
monster.outfit = { lookType = 308 }
monster.runHealth = 100
monster.changeTarget = {
    interval = 4000,
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -400,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 50,
        minDamage = 0,
        maxDamage = -645,
        radius = 3,
        target = false,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "physical",
        interval = 4000,
        chance = 20,
        effect = CONST_ME_SOUND_RED,
    },
}
monster.defenses = {
    defense = 55,
    armor = 50,
    {
        name = "speed",
        interval = 2000,
        chance = 10,
        speed = 390,
        duration = 6000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 15},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}

mtype:register(monster)
