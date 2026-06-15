local mtype = Game.createMonsterType("Death Blob")
local monster = {}

monster.name = "Death Blob"
monster.description = "a death blob"
monster.experience = 300
monster.race = "undead"
monster.maxHealth = 320
monster.health = 320
monster.speed = 180
monster.manaCost = 0
monster.corpse = 9960
monster.outfit = { lookType = 315 }
monster.changeTarget = {
    interval = 5000,
    chance = 0,
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
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -100,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 25,
        range = 3,
        minDamage = -35,
        maxDamage = -60,
        radius = 4,
        target = false,
        effect = CONST_ME_POFF,
    },
    {
        name = "death blob curse",
        interval = 2000,
        chance = 5,
        range = 5,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 2000,
        chance = 5,
        minDamage = 20,
        maxDamage = 30,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 30},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
}
monster.maxSummons = 3
monster.summons = {
    {name = "Death Blob", interval = 2000, chance = 10, max = 0},
}
monster.loot = {
    {
        id = "glob of tar",
        chance = 14285,
    },
}

mtype:register(monster)
