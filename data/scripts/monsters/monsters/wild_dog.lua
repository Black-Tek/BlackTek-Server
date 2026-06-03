local mtype = Game.createMonsterType("Wild Dog")
local monster = {}

monster.name = "Wild Dog"
monster.description = "a wild dog"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 1000000
monster.health = 1000000
monster.speed = 200
monster.manaCost = 220
monster.corpse = 5971
monster.outfit = {
    lookType = 32,
    lookHead = 20,
    lookBody = 30,
    lookLegs = 40,
    lookFeet = 50,
}
monster.runHealth = 8
monster.changeTarget = {
    interval = 2000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = false,
    hostile = false,
    illusionable = true,
    convinceable = true,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = 0,
    },
}
monster.defenses = {
    defense = 2,
    armor = 1,
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Wuff wuff", yell = false},
}

mtype:register(monster)
