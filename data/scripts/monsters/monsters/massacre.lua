local mtype = Game.createMonsterType("Massacre")
local monster = {}

monster.name = "Massacre"
monster.description = "Massacre"
monster.experience = 20000
monster.race = "blood"
monster.maxHealth = 32000
monster.health = 32000
monster.speed = 390
monster.manaCost = 0
monster.corpse = 6336
monster.outfit = { lookType = 244 }
monster.changeTarget = {
    interval = 60000,
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
    canPushCreatures = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -1200,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 24,
        range = 7,
        minDamage = -280,
        maxDamage = -500,
        shootEffect = CONST_ANI_LARGEROCK,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "speed",
        interval = 2000,
        chance = 12,
        speed = 380,
        duration = 4000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "piece of Massacre's shell",
        chance = 100000,
    },
}

mtype:register(monster)
