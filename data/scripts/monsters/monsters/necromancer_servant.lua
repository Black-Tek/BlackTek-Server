local mtype = Game.createMonsterType("Necromancer Servant")
local monster = {}

monster.name = "Necromancer Servant"
monster.description = "a necromancer servant"
monster.experience = 590
monster.race = "blood"
monster.maxHealth = 600
monster.health = 600
monster.speed = 210
monster.manaCost = 390
monster.corpse = 20455
monster.outfit = { lookType = 9 }
monster.runHealth = 10
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -45,
    },
    {
        name = "physical",
        interval = 1000,
        chance = 10,
        range = 7,
        minDamage = -5,
        maxDamage = -40,
        shootEffect = CONST_ANI_THROWINGKNIFE,
    },
}
monster.defenses = {
    defense = 11,
    armor = 11,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = -8},
    {type = COMBAT_DEATHDAMAGE, percent = -7},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Blib!", yell = false},
    {text = "Blub!", yell = false},
}

mtype:register(monster)
