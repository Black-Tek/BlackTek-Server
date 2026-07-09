local mtype = Game.createMonsterType("Ghost Wolf")
local monster = {}

monster.name = "Ghost Wolf"
monster.description = "a ghost wolf"
monster.experience = 65
monster.race = "undead"
monster.maxHealth = 160
monster.health = 160
monster.speed = 135
monster.manaCost = 0
monster.corpse = 24737
monster.outfit = { lookType = 730 }
monster.runHealth = 20
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = true,
    canWalkOnFire = true,
    canWalkOnPoison = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -80,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -30,
        maxDamage = -60,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
}
monster.defenses = {
    defense = 8,
    armor = 12,
    {
        name = "invisible",
        interval = 2000,
        chance = 20,
        duration = 5000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -5},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_EARTHDAMAGE, percent = 10},
    {type = COMBAT_DEATHDAMAGE, percent = 20},
    {type = COMBAT_PHYSICALDAMAGE, percent = 45},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 69320,
        maxCount = 20,
    },
}

mtype:register(monster)
