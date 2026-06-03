local mtype = Game.createMonsterType("Rift Lord")
local monster = {}

monster.name = "Rift Lord"
monster.description = "a rift lord"
monster.experience = 0
monster.race = "fire"
monster.maxHealth = 5
monster.health = 5
monster.speed = 220
monster.manaCost = 0
monster.corpse = 6068
monster.outfit = {
    lookType = 12,
    lookHead = 9,
    lookBody = 0,
    lookLegs = 9,
    lookFeet = 85,
    lookAddons = 0,
}
monster.changeTarget = {
    interval = 2000,
    chance = 25,
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
        skill = 10,
    },
}
monster.defenses = {
    defense = 6,
    armor = 5,
    {
        name = "healing",
        interval = 2000,
        chance = 19,
        effect = CONST_ME_MAGIC_BLUE,
    },
}

mtype:register(monster)
