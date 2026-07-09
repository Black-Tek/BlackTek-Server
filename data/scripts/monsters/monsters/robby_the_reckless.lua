local mtype = Game.createMonsterType("Robby the Reckless")
local monster = {}

monster.name = "Robby the Reckless"
monster.description = "Robby the Reckless"
monster.experience = 110
monster.race = "blood"
monster.maxHealth = 155
monster.health = 155
monster.speed = 220
monster.manaCost = 0
monster.corpse = 20498
monster.outfit = {
    lookType = 129,
    lookHead = 76,
    lookBody = 115,
    lookLegs = 99,
    lookFeet = 114,
    lookAddons = 1,
}
monster.changeTarget = {
    interval = 4000,
    chance = 5,
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
        maxDamage = -80,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
}
monster.loot = {
    {
        id = "bag with stolen gold",
        chance = 100000,
    },
    {
        id = "platinum coin",
        chance = 100000,
        maxCount = 93,
    },
}

mtype:register(monster)
