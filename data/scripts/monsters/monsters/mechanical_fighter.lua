local mtype = Game.createMonsterType("Mechanical Fighter")
local monster = {}

monster.name = "Mechanical Fighter"
monster.description = "a mechanical fighter"
monster.experience = 255
monster.race = "undead"
monster.maxHealth = 420
monster.health = 420
monster.speed = 200
monster.manaCost = 0
monster.corpse = 2253
monster.outfit = { lookType = 102 }
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
    pushable = true,
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
        maxDamage = -500,
    },
}
monster.defenses = {
    defense = 199,
    armor = 199,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 50},
}
monster.immunities = {
    {type = "holy", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}

mtype:register(monster)
