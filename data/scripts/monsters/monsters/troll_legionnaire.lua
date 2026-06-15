local mtype = Game.createMonsterType("Troll Legionnaire")
local monster = {}

monster.name = "Troll Legionnaire"
monster.description = "a troll legionnaire"
monster.experience = 140
monster.race = "blood"
monster.maxHealth = 210
monster.health = 210
monster.speed = 190
monster.manaCost = 0
monster.corpse = 5998
monster.outfit = { lookType = 53 }
monster.runHealth = 30
monster.changeTarget = {
    interval = 2000,
    chance = 5,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -35,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 18,
        range = 6,
        minDamage = 0,
        maxDamage = -130,
        target = true,
        shootEffect = CONST_ANI_THROWINGSTAR,
    },
}
monster.defenses = {
    defense = 9,
    armor = 7,
    {
        name = "invisible",
        interval = 2000,
        chance = 18,
        duration = 20000,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 28,
        minDamage = 17,
        maxDamage = 25,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 9,
    {text = "Attack!", yell = false},
    {text = "Graaaaar!", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 46000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 46000,
        maxCount = 55,
    },
    {
        id = "stealth ring",
        chance = 560,
    },
    {
        id = "throwing star",
        chance = 28000,
        maxCount = 10,
    },
    {
        id = "frosty ear of a troll",
        chance = 5120,
    },
}

mtype:register(monster)
