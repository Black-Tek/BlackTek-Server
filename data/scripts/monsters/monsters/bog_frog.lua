local mtype = Game.createMonsterType("Bog Frog")
local monster = {}

monster.name = "Bog Frog"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 25
monster.health = 25
monster.speed = 200
monster.manaCost = 305
monster.corpse = 6079
monster.outfit = { lookType = 412 }
monster.runHealth = 25
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
}

mtype:register(monster)
