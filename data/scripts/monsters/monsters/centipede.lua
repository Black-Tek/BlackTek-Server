local mtype = Game.createMonsterType("Centipede")
local monster = {}

monster.name = "Centipede"
monster.experience = 34
monster.race = "venom"
monster.maxHealth = 70
monster.health = 70
monster.speed = 166
monster.manaCost = 335
monster.corpse = 6050
monster.outfit = { lookType = 124 }
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = true,
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
        maxDamage = -45,
        -- NOTE: melee condition (poison=20); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 20000,
        },
    },
}
monster.defenses = {
    defense = 8,
    armor = 8,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_FIREDAMAGE, percent = -15},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 80000,
        maxCount = 15,
    },
    {
        id = "centipede leg",
        chance = 10300,
    },
}

mtype:register(monster)
