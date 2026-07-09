local mtype = Game.createMonsterType("The Handmaiden")
local monster = {}

monster.name = "The Handmaiden"
monster.description = "The Handmaiden"
monster.experience = 7500
monster.race = "blood"
monster.maxHealth = 19500
monster.health = 19500
monster.speed = 450
monster.manaCost = 0
monster.corpse = 6312
monster.outfit = { lookType = 230 }
monster.runHealth = 3100
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 1
monster.staticAttackChance = 70
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
        maxDamage = -800,
    },
    {
        name = "manadrain",
        interval = 2000,
        chance = 25,
        range = 7,
        minDamage = -150,
        maxDamage = -800,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "drunk",
        interval = 1000,
        chance = 12,
        range = 1,
        target = true,
    },
}
monster.defenses = {
    defense = 35,
    armor = 25,
    {
        name = "speed",
        interval = 3000,
        chance = 12,
        speed = 380,
        duration = 8000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "invisible",
        interval = 4000,
        chance = 50,
        duration = 6000,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "healing",
        interval = 2000,
        chance = 50,
        minDamage = 100,
        maxDamage = 250,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "speed",
        interval = 1000,
        chance = 35,
        speed = 370,
        duration = 30000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "the Handmaiden's protector",
        chance = 35000,
    },
}

mtype:register(monster)
