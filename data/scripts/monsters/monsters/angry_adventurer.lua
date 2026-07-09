local mtype = Game.createMonsterType("Angry Adventurer")
local monster = {}

monster.name = "Angry Adventurer"
monster.description = "an angry adventurer"
monster.experience = 50
monster.race = "blood"
monster.maxHealth = 65
monster.health = 65
monster.speed = 106
monster.manaCost = 0
monster.corpse = 20315
monster.outfit = {
    lookType = 129,
    lookHead = 93,
    lookBody = 15,
    lookLegs = 72,
    lookFeet = 80,
    lookAddons = 0,
}
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
    canPushCreatures = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -25,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}

mtype:register(monster)
