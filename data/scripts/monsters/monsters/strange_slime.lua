local mtype = Game.createMonsterType("Strange Slime")
local monster = {}

monster.name = "Strange Slime"
monster.description = "a strange slime"
monster.experience = 0
monster.race = "venom"
monster.maxHealth = 15
monster.health = 15
monster.speed = 120
monster.manaCost = 0
monster.corpse = 1496
monster.outfit = { lookType = 19 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = false,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.defenses = {
    defense = 10,
    armor = 3,
}

mtype:register(monster)
