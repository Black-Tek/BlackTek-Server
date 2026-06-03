local mtype = Game.createMonsterType("Damaged Crystal Golem")
local monster = {}

monster.name = "Damaged Crystal Golem"
monster.description = "a damaged crystal golem"
monster.experience = 0
monster.race = "energy"
monster.maxHealth = 500
monster.health = 500
monster.speed = 180
monster.manaCost = 0
monster.corpse = 18466
monster.outfit = { lookType = 508 }
monster.runHealth = 500
monster.changeTarget = {
    interval = 4000,
    chance = 10,
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
}

mtype:register(monster)
