local mtype = Game.createMonsterType("Menace")
local monster = {}

monster.name = "Menace"
monster.description = "Menace"
monster.experience = 3250
monster.race = "blood"
monster.maxHealth = 6000
monster.health = 6000
monster.speed = 220
monster.manaCost = 0
monster.corpse = 4251
monster.outfit = { lookType = 113 }
monster.changeTarget = {
    interval = 2000,
    chance = 9,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -215,
    },
}
monster.defenses = {
    defense = 19,
    armor = 16,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = 10},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}

mtype:register(monster)
