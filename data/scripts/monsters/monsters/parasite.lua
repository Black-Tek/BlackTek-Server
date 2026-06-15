local mtype = Game.createMonsterType("Parasite")
local monster = {}

monster.name = "Parasite"
monster.description = "a parasite"
monster.experience = 0
monster.race = "venom"
monster.maxHealth = 550
monster.health = 550
monster.speed = 175
monster.manaCost = 0
monster.corpse = 6023
monster.outfit = { lookType = 82 }
monster.changeTarget = {
    interval = 5000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -100,
        -- NOTE: melee condition (poison=40); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 40000,
        },
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -20},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "fire", combat = true, condition = true},
}

mtype:register(monster)
