local mtype = Game.createMonsterType("Fire Elemental")
local monster = {}

monster.name = "Fire Elemental"
monster.description = "a fire elemental"
monster.experience = 220
monster.race = "fire"
monster.maxHealth = 280
monster.health = 280
monster.speed = 190
monster.manaCost = 690
monster.corpse = 8964
monster.outfit = { lookType = 49 }
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
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -100,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = -45,
        maxDamage = -160,
        radius = 2,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "firefield",
        interval = 2000,
        chance = 25,
        range = 7,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_FIRE,
    },
}
monster.defenses = {
    defense = 18,
    armor = 18,
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}

mtype:register(monster)
