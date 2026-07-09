local mtype = Game.createMonsterType("Nightfiend")
local monster = {}

monster.name = "Nightfiend"
monster.description = "a nightfiend"
monster.experience = 2100
monster.race = "blood"
monster.maxHealth = 2700
monster.health = 2700
monster.speed = 210
monster.manaCost = 0
monster.corpse = 2669
monster.outfit = { lookType = 568 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 4
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
        interval = 1000,
        chance = 10,
        range = 7,
        minDamage = -5,
        maxDamage = -40,
        shootEffect = CONST_ANI_THROWINGKNIFE,
    },
}
monster.defenses = {
    defense = 11,
    armor = 11,
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = 5},
    {type = COMBAT_ENERGYDAMAGE, percent = 5},
    {type = COMBAT_ICEDAMAGE, percent = 5},
    {type = COMBAT_FIREDAMAGE, percent = -5},
    {type = COMBAT_HOLYDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}

mtype:register(monster)
