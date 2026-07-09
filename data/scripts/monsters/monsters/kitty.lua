local mtype = Game.createMonsterType("Kitty")
local monster = {}

monster.name = "Kitty"
monster.description = "Kitty"
monster.experience = 0
monster.race = "blood"
monster.maxHealth = 75
monster.health = 75
monster.speed = 210
monster.manaCost = 0
monster.corpse = 6051
monster.outfit = { lookType = 125 }
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 70
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -120,
    },
}
monster.defenses = {
    defense = 10,
    armor = 10,
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        speed = 310,
        duration = 3000,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.immunities = {
    {type = "physical", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "poison", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "holy", combat = true, condition = true},
    {type = "death", combat = true, condition = true},
    {type = "lifedrain", combat = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "drown", combat = true, condition = true},
}

mtype:register(monster)
