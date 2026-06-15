local mtype = Game.createMonsterType("War Wolf")
local monster = {}

monster.name = "War Wolf"
monster.description = "a war wolf"
monster.experience = 55
monster.race = "blood"
monster.maxHealth = 140
monster.health = 140
monster.speed = 264
monster.manaCost = 420
monster.corpse = 6009
monster.outfit = { lookType = 3 }
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
    convinceable = true,
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
        maxDamage = -50,
    },
}
monster.defenses = {
    defense = 8,
    armor = 8,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_HOLYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_DEATHDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "outfit", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Grrrrrrr", yell = false},
    {text = "Yoooohhuuuu!", yell = true},
}
monster.loot = {
    {
        id = 2671,
        chance = 39170,
    },
    {
        id = 11235,
        chance = 4951,
    },
    {
        id = 5897,
        chance = 993,
    },
    {
        id = 7394,
        chance = 48,
    },
}

mtype:register(monster)
