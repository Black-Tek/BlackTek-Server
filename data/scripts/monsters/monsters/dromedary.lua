local mtype = Game.createMonsterType("Dromedary")
local monster = {}

monster.name = "Dromedary"
monster.description = "a dromedary"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 45
monster.health = 45
monster.speed = 170
monster.manaCost = 0
monster.corpse = 13528
monster.outfit = { lookType = 404 }
monster.runHealth = 45
monster.changeTarget = {
    interval = 4000,
    chance = 20,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = false,
    illusionable = true,
    convinceable = false,
    pushable = true,
    canPushItems = false,
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
        maxDamage = -8,
    },
    {
        name = "drunk",
        interval = 4000,
        chance = 5,
        range = 1,
        target = true,
        duration = 6000,
        shootEffect = CONST_ANI_EXPLOSION,
        effect = CONST_ME_STUN,
    },
}
monster.defenses = {
    defense = 5,
    armor = 5,
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Snort", yell = false},
    {text = "Grunt!", yell = false},
}
monster.loot = {
    {
        id = "meat",
        chance = 36000,
        maxCount = 2,
    },
    {
        id = "sugar oat",
        chance = 1000,
    },
}

mtype:register(monster)
