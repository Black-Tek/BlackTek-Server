local mtype = Game.createMonsterType("Slug")
local monster = {}

monster.name = "Slug"
monster.description = "a slug"
monster.experience = 70
monster.race = "venom"
monster.maxHealth = 255
monster.health = 255
monster.speed = 150
monster.manaCost = 0
monster.corpse = 13515
monster.outfit = { lookType = 407 }
monster.changeTarget = {
    interval = 5000,
    chance = 0,
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
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -44,
    },
    {
        name = "earth",
        interval = 1000,
        chance = 15,
        range = 7,
        minDamage = -5,
        maxDamage = -40,
        target = true,
        shootEffect = CONST_ANI_POISON,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = 7},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 62000,
        maxCount = 40,
    },
    {
        id = "worm",
        chance = 5000,
        maxCount = 3,
    },
}

mtype:register(monster)
