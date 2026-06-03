local mtype = Game.createMonsterType("Flamingo")
local monster = {}

monster.name = "Flamingo"
monster.description = "a flamingo"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 25
monster.health = 25
monster.speed = 210
monster.manaCost = 250
monster.corpse = 6054
monster.outfit = { lookType = 212 }
monster.runHealth = 25
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = false,
    illusionable = true,
    convinceable = true,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.defenses = {
    defense = 1,
    armor = 1,
}
monster.loot = {
    {
        id = "downy feather",
        chance = 1000,
    },
}

mtype:register(monster)
