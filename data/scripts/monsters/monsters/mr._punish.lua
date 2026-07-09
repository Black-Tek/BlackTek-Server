local mtype = Game.createMonsterType("Mr. Punish")
local monster = {}

monster.name = "Mr. Punish"
monster.description = "Mr. Punish"
monster.experience = 9000
monster.race = "undead"
monster.maxHealth = 22000
monster.health = 22000
monster.speed = 470
monster.manaCost = 0
monster.corpse = 6331
monster.outfit = { lookType = 234 }
monster.runHealth = 2000
monster.changeTarget = {
    interval = 2000,
    chance = 5,
}
monster.targetDistance = 1
monster.staticAttackChance = 50
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
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
        maxDamage = -1280,
    },
}
monster.defenses = {
    defense = 72,
    armor = 64,
}
monster.immunities = {
    {type = "energy", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "Mr. Punish's handcuffs",
        chance = 100000,
    },
}

mtype:register(monster)
