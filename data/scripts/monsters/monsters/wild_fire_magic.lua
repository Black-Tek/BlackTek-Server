local mtype = Game.createMonsterType("Wild Fire Magic")
local monster = {}

monster.name = "Wild Fire Magic"
monster.description = "a wild fire magic"
monster.experience = 0
monster.race = "undead"
monster.maxHealth = 1
monster.health = 1
monster.speed = 210
monster.manaCost = 0
monster.outfit = { lookTypeEx = 21445 }
monster.runHealth = 1
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = false,
    hostile = false,
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
}

mtype:register(monster)
