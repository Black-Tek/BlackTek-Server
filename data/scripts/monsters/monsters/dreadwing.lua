local mtype = Game.createMonsterType("Dreadwing")
local monster = {}

monster.name = "Dreadwing"
monster.description = "Dreadwing"
monster.experience = 3750
monster.race = "blood"
monster.maxHealth = 8500
monster.health = 8500
monster.speed = 245
monster.manaCost = 0
monster.corpse = 9829
monster.outfit = { lookType = 307 }
monster.changeTarget = {
    interval = 2000,
    chance = 10,
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -250,
    },
    {
        name = "physical",
        interval = 3000,
        chance = 50,
        minDamage = 0,
        maxDamage = -100,
        radius = 7,
        target = true,
        effect = CONST_ME_SOUND_WHITE,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
}
monster.immunities = {
    {type = "death", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}

mtype:register(monster)
