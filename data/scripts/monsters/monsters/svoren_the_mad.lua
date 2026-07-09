local mtype = Game.createMonsterType("Svoren the Mad")
local monster = {}

monster.name = "Svoren the Mad"
monster.description = "Svoren the Mad"
monster.experience = 3000
monster.race = "blood"
monster.maxHealth = 6310
monster.health = 6310
monster.speed = 180
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = {
    lookType = 254,
    lookHead = 61,
    lookBody = 61,
    lookLegs = 61,
    lookFeet = 4,
}
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -525,
    },
    {
        name = "speed",
        interval = 3500,
        chance = 35,
        range = 1,
        radius = 1,
        target = true,
        speed = -250,
        duration = 40,
        effect = CONST_ME_MAGIC_RED,
    },
}
monster.defenses = {
    defense = 27,
    armor = 25,
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
    {type = "energy", combat = true, condition = true},
    {type = "earth", combat = true, condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "NO mommy NO. Leave me alone!", yell = false},
    {text = "Not that tower again!", yell = false},
    {text = "The cat has grown some horns!!", yell = false},
    {text = "What was I doing here again?", yell = false},
    {text = "Are we there soon mommy?", yell = false},
}

mtype:register(monster)
