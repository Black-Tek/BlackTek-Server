local mtype = Game.createMonsterType("Scorpion")
local monster = {}

monster.name = "Scorpion"
monster.description = "a scorpion"
monster.experience = 45
monster.race = "venom"
monster.maxHealth = 45
monster.health = 45
monster.speed = 150
monster.manaCost = 310
monster.corpse = 5988
monster.outfit = { lookType = 43 }
monster.runHealth = 5
monster.changeTarget = {
    interval = 4000,
    chance = 10,
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
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -50,
        -- NOTE: melee condition (poison=340); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 340000,
        },
    },
}
monster.defenses = {
    defense = 14,
    armor = 14,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
}
monster.loot = {
    {
        id = "scorpion tail",
        chance = 5000,
    },
}

mtype:register(monster)
