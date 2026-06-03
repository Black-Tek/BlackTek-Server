local mtype = Game.createMonsterType("Essence of Darkness")
local monster = {}

monster.name = "Essence of Darkness"
monster.description = "an essence of darkness"
monster.experience = 30
monster.race = "undead"
monster.maxHealth = 1000
monster.health = 1000
monster.speed = 230
monster.manaCost = 0
monster.corpse = 9960
monster.outfit = { lookType = 315 }
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 85
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
}
monster.defenses = {
    defense = 10,
    armor = 10,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = 98},
    {type = COMBAT_HOLYDAMAGE, percent = 80},
}
monster.immunities = {
    {type = "physical", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "lifedrain", combat = true},
}

mtype:register(monster)
