local mtype = Game.createMonsterType("Undead Cavebear")
local monster = {}

monster.name = "Undead Cavebear"
monster.description = "Undead Cavebear"
monster.experience = 600
monster.race = "blood"
monster.maxHealth = 450
monster.health = 450
monster.speed = 250
monster.manaCost = 0
monster.corpse = 13323
monster.outfit = { lookType = 384 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -400,
    },
}
monster.defenses = {
    defense = 27,
    armor = 23,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -15},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 9750,
        maxCount = 80,
    },
    {
        id = "maxilla maximus",
        chance = 900,
    },
    {
        id = "maxilla",
        chance = 2350,
    },
    {
        id = "cavebear skull",
        chance = 3150,
    },
}

mtype:register(monster)
