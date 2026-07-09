local mtype = Game.createMonsterType("Yalahari")
local monster = {}

monster.name = "Yalahari"
monster.description = "a Yalahari"
monster.experience = 5
monster.race = "blood"
monster.maxHealth = 150
monster.health = 150
monster.speed = 200
monster.manaCost = 0
monster.corpse = 20550
monster.outfit = { lookType = 309 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = false,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
}
monster.defenses = {
    defense = 0,
    armor = 0,
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "physical", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 11,
    {text = "Welcome to Yalahar, outsider.", yell = false},
    {text = "Hail Yalahar.", yell = false},
    {text = "You can learn a lot from us.", yell = false},
    {text = "Our wisdom and knowledge are unequalled in this world.", yell = false},
    {text = "That knowledge would overburden your fragile mind.", yell = false},
    {text = "I wouldn't expect you to understand.", yell = false},
    {text = "One day Yalahar will return to its former glory.", yell = false},
}

mtype:register(monster)
