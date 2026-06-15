local mtype = Game.createMonsterType("Mimic")
local monster = {}

monster.name = "Mimic"
monster.description = "a mimic"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 30
monster.health = 30
monster.speed = 170
monster.manaCost = 0
monster.corpse = 1740
monster.outfit = { lookType = 92 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 5
monster.staticAttackChance = 0
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
}
monster.defenses = {
    defense = 3,
    armor = 2,
}

mtype:register(monster)
