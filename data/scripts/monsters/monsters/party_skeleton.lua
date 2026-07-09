local mtype = Game.createMonsterType("Party Skeleton")
local monster = {}

monster.name = "Party Skeleton"
monster.description = "a party skeleton"
monster.experience = 0
monster.race = "undead"
monster.maxHealth = 40
monster.health = 40
monster.speed = 154
monster.manaCost = 0
monster.corpse = 5972
monster.outfit = { lookType = 33 }
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
    illusionable = false,
    convinceable = false,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -3,
    },
}
monster.defenses = {
    defense = 2,
    armor = 2,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -25},
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
}

mtype:register(monster)
