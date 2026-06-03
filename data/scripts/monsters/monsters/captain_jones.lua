local mtype = Game.createMonsterType("Captain Jones")
local monster = {}

monster.name = "Captain Jones"
monster.description = "Captain Jones"
monster.experience = 620
monster.race = "undead"
monster.maxHealth = 555
monster.health = 555
monster.speed = 200
monster.manaCost = 0
monster.corpse = 5566
monster.outfit = { lookType = 196 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
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
        maxDamage = -100,
    },
    {
        name = "death",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -50,
        maxDamage = -90,
        target = true,
        shootEffect = CONST_ANI_DEATH,
        effect = CONST_ME_SMALLCLOUDS,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 5,
        range = 7,
        target = true,
        duration = 4000,
        shootEffect = CONST_ANI_EXPLOSION,
    },
}
monster.defenses = {
    {
        name = "healing",
        interval = 2000,
        chance = 5,
        minDamage = 40,
        maxDamage = 70,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "physical", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "lifedrain", combat = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 50,
    },
    {
        id = "stealth ring",
        chance = 20000,
    },
    {
        id = 2488,
        chance = 5000,
    },
    {
        id = "focus cape",
        chance = 5000,
    },
    {
        id = "red robe",
        chance = 5000,
    },
    {
        id = "spike sword",
        chance = 5000,
    },
}

mtype:register(monster)
