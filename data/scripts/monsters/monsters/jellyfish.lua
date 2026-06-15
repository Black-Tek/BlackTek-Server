local mtype = Game.createMonsterType("Jellyfish")
local monster = {}

monster.name = "Jellyfish"
monster.description = "a jellyfish"
monster.experience = 0
monster.race = "undead"
monster.maxHealth = 55
monster.health = 55
monster.speed = 170
monster.manaCost = 0
monster.corpse = 15284
monster.outfit = { lookType = 452 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
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
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -10,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
    {
        name = "invisible",
        interval = 2000,
        chance = 15,
        duration = 3000,
    },
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
    {type = "drown", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Luuurrrp", yell = false},
}
monster.loot = {
    {
        id = "shrimp",
        chance = 8333,
    },
}

mtype:register(monster)
