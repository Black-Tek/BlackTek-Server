local mtype = Game.createMonsterType("Evil Sheep")
local monster = {}

monster.name = "Evil Sheep"
monster.description = "an evil sheep"
monster.experience = 240
monster.race = "blood"
monster.maxHealth = 350
monster.health = 350
monster.speed = 250
monster.manaCost = 0
monster.corpse = 5991
monster.outfit = { lookType = 14 }
monster.runHealth = 20
monster.changeTarget = {
    interval = 2000,
    chance = 20,
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
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -90,
    },
    {
        name = "physical",
        interval = 4000,
        chance = 30,
        range = 7,
        minDamage = 0,
        maxDamage = -50,
        target = true,
        shootEffect = CONST_ANI_SNOWBALL,
    },
}
monster.defenses = {
    defense = 35,
    armor = 30,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -10},
    {type = COMBAT_ICEDAMAGE, percent = 20},
    {type = COMBAT_PHYSICALDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "lifedrain", combat = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Grrrr", yell = false},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 50,
    },
}

mtype:register(monster)
