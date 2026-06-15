local mtype = Game.createMonsterType("Spectral Scum")
local monster = {}

monster.name = "Spectral Scum"
monster.description = "Spectral Scum"
monster.experience = 0
monster.race = "undead"
monster.maxHealth = 1
monster.health = 1
monster.speed = 230
monster.manaCost = 0
monster.corpse = 6070
monster.outfit = { lookType = 195 }
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
    pushable = true,
    canPushItems = true,
    canPushCreatures = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = 0,
    },
    {
        name = "drunk",
        interval = 5000,
        chance = 10,
        range = 2,
        effect = CONST_ME_POFF,
    },
}
monster.defenses = {
    defense = 19,
    armor = 18,
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 10,
    {text = "BBooze! More booze!, Harrr Harrr!", yell = false},
    {text = "Burp, Fart.", yell = false},
    {text = "Nothing better than plundering and getting palstered!", yell = false},
    {text = "Yarrr!, A pirate's unlife for me!", yell = false},
}

mtype:register(monster)
