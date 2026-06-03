local mtype = Game.createMonsterType("Spirit of Fire")
local monster = {}

monster.name = "Spirit of Fire"
monster.description = "a spirit of fire"
monster.experience = 950
monster.race = "blood"
monster.maxHealth = 2210
monster.health = 2210
monster.speed = 220
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 242 }
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
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -300,
    },
    {
        name = "firefield",
        interval = 1000,
        chance = 15,
        range = 7,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_FIRE,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 20,
        range = 7,
        minDamage = -650,
        maxDamage = -315,
        shootEffect = CONST_ANI_FIRE,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 6,
    {text = "Feel the heat.", yell = false},
}

mtype:register(monster)
