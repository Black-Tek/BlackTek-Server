local mtype = Game.createMonsterType("Iron Servant")
local monster = {}

monster.name = "Iron Servant"
monster.description = "an iron servant"
monster.experience = 210
monster.race = "energy"
monster.maxHealth = 350
monster.health = 350
monster.speed = 190
monster.manaCost = 0
monster.corpse = 13486
monster.outfit = { lookType = 395 }
monster.runHealth = 50
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
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -60,
        shootEffect = CONST_ANI_SMALLSTONE,
        effect = CONST_ME_EXPLOSIONAREA,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        range = 7,
        target = true,
        duration = 2000,
        shootEffect = CONST_ANI_EXPLOSION,
        effect = CONST_ME_STUN,
    },
}
monster.defenses = {
    defense = 17,
    armor = 17,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 25},
    {type = COMBAT_EARTHDAMAGE, percent = -10},
    {type = COMBAT_FIREDAMAGE, percent = 25},
    {type = COMBAT_HOLYDAMAGE, percent = 20},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "paralyze", condition = true},
}
monster.voices = {
    interval = 1000,
    chance = 5,
    {text = "Error. LOAD 'PROGRAM',8,1", yell = false},
    {text = "Remain. Obedient.", yell = false},
}
monster.loot = {
    {
        id = 2148,
        chance = 82109,
        maxCount = 55,
    },
    {
        id = 9690,
        chance = 4825,
    },
    {
        id = 7618,
        chance = 1964,
    },
    {
        id = 2381,
        chance = 1022,
    },
    {
        id = 9808,
        chance = 504,
    },
    {
        id = 13758,
        chance = 310,
    },
}

mtype:register(monster)
