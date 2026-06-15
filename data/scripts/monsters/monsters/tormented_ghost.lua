local mtype = Game.createMonsterType("Tormented Ghost")
local monster = {}

monster.name = "Tormented Ghost"
monster.description = "a tormented ghost"
monster.experience = 5
monster.race = "undead"
monster.maxHealth = 210
monster.health = 210
monster.speed = 160
monster.manaCost = 0
monster.corpse = 9741
monster.outfit = { lookType = 48 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
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
        minDamage = 0,
        maxDamage = -100,
    },
    {
        name = "lifedrain",
        interval = 3000,
        chance = 15,
        range = 1,
        minDamage = -40,
        maxDamage = -70,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "physical", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Haaahhh!", yell = false},
    {text = "Grrglll", yell = false},
}

mtype:register(monster)
