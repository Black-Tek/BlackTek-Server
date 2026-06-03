local mtype = Game.createMonsterType("Mornenion")
local monster = {}

monster.name = "Mornenion"
monster.description = "Mornenion"
monster.experience = 115
monster.race = "blood"
monster.maxHealth = 190
monster.health = 190
monster.speed = 110
monster.manaCost = 0
monster.corpse = 6012
monster.outfit = { lookType = 64 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
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
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -40,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 5,
        minDamage = 0,
        maxDamage = -40,
        shootEffect = CONST_ANI_ARROW,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Let us leave safely and maybe we'll give you a few coins.", yell = false},
}
monster.loot = {
    {
        id = "bag with stolen gold",
        chance = 100000,
    },
    {
        id = "platinum coin",
        chance = 100000,
        maxCount = 88,
    },
}

mtype:register(monster)
