local mtype = Game.createMonsterType("Grodrik")
local monster = {}

monster.name = "Grodrik"
monster.description = "Grodrik"
monster.experience = 60
monster.race = "blood"
monster.maxHealth = 120
monster.health = 120
monster.speed = 170
monster.manaCost = 0
monster.corpse = 6007
monster.outfit = {
    lookType = 160,
    lookHead = 77,
    lookBody = 120,
    lookLegs = 79,
    lookFeet = 115,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
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
        maxDamage = -35,
    },
}
monster.defenses = {
    defense = 7,
    armor = 7,
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "By the great mother of fire! What is this?", yell = false},
    {text = "Can I at least have a beer??", yell = false},
    {text = "I'll hail no one, not even Durin, until I am brought back to where I was just moments ago!", yell = false},
    {text = "If I ever find out who is behind all that... GRR!", yell = false},
    {text = "What the... where is this place?", yell = false},
}

mtype:register(monster)
