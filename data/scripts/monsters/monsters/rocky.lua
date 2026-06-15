local mtype = Game.createMonsterType("Rocky")
local monster = {}

monster.name = "Rocky"
monster.description = "Rocky"
monster.experience = 190
monster.race = "undead"
monster.maxHealth = 390
monster.health = 390
monster.speed = 170
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 95 }
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
    canWalkOnFire = false,
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
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 50,
        minDamage = 15,
        maxDamage = 35,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 10},
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Another little gladiator!", yell = false},
    {text = "Come into my embrace.", yell = false},
}

mtype:register(monster)
