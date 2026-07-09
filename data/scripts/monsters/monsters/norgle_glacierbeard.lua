local mtype = Game.createMonsterType("Norgle Glacierbeard")
local monster = {}

monster.name = "Norgle Glacierbeard"
monster.description = "Norgle Glacierbeard"
monster.experience = 2100
monster.race = "blood"
monster.maxHealth = 4280
monster.health = 4280
monster.speed = 195
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 257 }
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
        maxDamage = -225,
    },
}
monster.defenses = {
    defense = 27,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = -15},
    {type = COMBAT_ENERGYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "ice", combat = true, condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "I'll extinguish you warmbloods.", yell = false},
    {text = "REVENGE!", yell = false},
    {text = "Far too hot.", yell = false},
    {text = "DISGUSTING WARMBLOODS!", yell = false},
    {text = "Revenge is sweetest when served cold.", yell = false},
}

mtype:register(monster)
