local mtype = Game.createMonsterType("Haunter")
local monster = {}

monster.name = "Haunter"
monster.description = "Haunter"
monster.experience = 4000
monster.race = "blood"
monster.maxHealth = 8500
monster.health = 8500
monster.speed = 270
monster.manaCost = 0
monster.corpse = 9915
monster.outfit = { lookType = 320 }
monster.changeTarget = {
    interval = 2000,
    chance = 9,
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -150,
    },
    {
        name = "energy",
        interval = 2000,
        chance = 16,
        minDamage = 0,
        maxDamage = -130,
        radius = 3,
        target = false,
        effect = CONST_ME_ENERGYHIT,
    },
    {
        name = "poisoncondition",
        interval = 2000,
        chance = 16,
        range = 7,
        target = true,
        shootEffect = CONST_ANI_POISON,
        effect = CONST_ME_POISONAREA,
    },
}
monster.defenses = {
    defense = 20,
    armor = 25,
    {
        name = "healing",
        interval = 2000,
        chance = 16,
        minDamage = 100,
        maxDamage = 155,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 16,
        speed = 360,
        duration = 80000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 9,
    {text = "Surrender and I'll end it quick!", yell = false},
}

mtype:register(monster)
