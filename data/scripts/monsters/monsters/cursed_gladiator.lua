local mtype = Game.createMonsterType("Cursed Gladiator")
local monster = {}

monster.name = "Cursed Gladiator"
monster.description = "a cursed gladiator"
monster.experience = 215
monster.race = "undead"
monster.maxHealth = 435
monster.health = 435
monster.speed = 170
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 100 }
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -150,
    },
    {
        name = "lifedrain",
        interval = 4000,
        chance = 50,
        range = 5,
        minDamage = 0,
        maxDamage = 50,
        radius = 1,
        target = true,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 26,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 20},
    {type = COMBAT_FIREDAMAGE, percent = -5},
}
monster.immunities = {
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "Death where are you?", yell = false},
    {text = "Slay me, end my curse.", yell = false},
}

mtype:register(monster)
