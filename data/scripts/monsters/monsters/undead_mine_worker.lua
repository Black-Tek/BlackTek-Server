local mtype = Game.createMonsterType("Undead Mine Worker")
local monster = {}

monster.name = "Undead Mine Worker"
monster.description = "an undead mine worker"
monster.experience = 45
monster.race = "undead"
monster.maxHealth = 65
monster.health = 65
monster.speed = 150
monster.manaCost = 0
monster.corpse = 5972
monster.outfit = { lookType = 33 }
monster.changeTarget = {
    interval = 4000,
    chance = 0,
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
    canPushItems = false,
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
        maxDamage = -30,
    },
    {
        name = "lifedrain",
        interval = 2000,
        chance = 15,
        range = 1,
        minDamage = -7,
        maxDamage = -13,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 73000,
        maxCount = 10,
    },
    {
        id = 2230,
        chance = 42000,
    },
    {
        id = 2376,
        chance = 3850,
    },
    {
        id = "mace",
        chance = 26900,
    },
    {
        id = "white mushroom",
        chance = 15400,
        maxCount = 3,
    },
    {
        id = "brown mushroom",
        chance = 3850,
    },
}

mtype:register(monster)
