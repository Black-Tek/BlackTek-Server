local mtype = Game.createMonsterType("Rift Scythe")
local monster = {}

monster.name = "Rift Scythe"
monster.description = "Rift Scythe"
monster.experience = 2000
monster.race = "undead"
monster.maxHealth = 3600
monster.health = 3600
monster.speed = 370
monster.manaCost = 0
monster.outfit = { lookType = 300 }
monster.changeTarget = {
    interval = 5000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 85
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -979,
    },
    {
        name = "death",
        interval = 2000,
        chance = 60,
        range = 7,
        minDamage = 0,
        maxDamage = -200,
        target = true,
        shootEffect = CONST_ANI_SUDDENDEATH,
        effect = CONST_ME_MORTAREA,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 50,
        minDamage = 0,
        maxDamage = -600,
        length = 7,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 40,
        minDamage = 0,
        maxDamage = -395,
        radius = 4,
        target = false,
        effect = CONST_ME_DRAWBLOOD,
    },
    {
        name = "physical",
        interval = 3000,
        chance = 60,
        minDamage = 0,
        maxDamage = -300,
        length = 7,
        spread = 3,
        effect = CONST_ME_EXPLOSIONAREA,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 1000,
        chance = 25,
        minDamage = 100,
        maxDamage = 195,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_HOLYDAMAGE, percent = -20},
    {type = COMBAT_DEATHDAMAGE, percent = 60},
    {type = COMBAT_FIREDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}

mtype:register(monster)
