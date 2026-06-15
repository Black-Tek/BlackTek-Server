local mtype = Game.createMonsterType("a carved stone tile")
local monster = {}

monster.name = "a carved stone tile"
monster.description = "a carved stone tile"
monster.experience = 0
monster.race = "undead"
monster.maxHealth = 100
monster.health = 100
monster.speed = 0
monster.manaCost = 0
monster.outfit = { lookTypeEx = 3169 }
monster.changeTarget = {
    interval = 5000,
    chance = 16,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = false,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = false,
    canPushCreatures = false,
}
monster.attacks = {
    {
        name = "lifedrain",
        interval = 2000,
        chance = 20,
        minDamage = 0,
        maxDamage = 0,
        radius = 3,
        target = false,
        effect = CONST_ME_GROUNDSHAKER,
    },
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.maxSummons = 4
monster.summons = {
    {name = "Dreadbeast", interval = 2000, chance = 25, max = 0},
}

mtype:register(monster)
