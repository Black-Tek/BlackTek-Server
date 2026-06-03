local mtype = Game.createMonsterType("Coldheart")
local monster = {}

monster.name = "Coldheart"
monster.description = "Coldheart"
monster.experience = 3500
monster.race = "undead"
monster.maxHealth = 7000
monster.health = 7000
monster.speed = 195
monster.manaCost = 0
monster.corpse = 7282
monster.outfit = { lookType = 261 }
monster.changeTarget = {
    interval = 2000,
    chance = 9,
}
monster.targetDistance = 1
monster.staticAttackChance = 50
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -220,
    },
    {
        name = "ice",
        interval = 2000,
        chance = 25,
        minDamage = 0,
        maxDamage = -710,
        length = 8,
        effect = CONST_ME_ICEAREA,
    },
}
monster.defenses = {
    defense = 26,
    armor = 25,
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
}

mtype:register(monster)
