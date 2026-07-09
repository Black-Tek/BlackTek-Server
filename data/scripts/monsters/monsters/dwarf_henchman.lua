local mtype = Game.createMonsterType("Dwarf Henchman")
local monster = {}

monster.name = "Dwarf Henchman"
monster.description = "a dwarf henchman"
monster.experience = 15
monster.race = "blood"
monster.maxHealth = 350
monster.health = 350
monster.speed = 210
monster.manaCost = 0
monster.corpse = 6007
monster.outfit = {
    lookType = 160,
    lookHead = 115,
    lookBody = 77,
    lookLegs = 93,
    lookFeet = 114,
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
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -50,
        -- NOTE: melee condition (poison=20); verify damage/duration
        condition = {
            type = CONDITION_POISON,
            interval = 20000,
        },
    },
    {
        name = "drowncondition",
        interval = 2000,
        chance = 20,
        target = true,
    },
}
monster.defenses = {
    defense = 16,
    armor = 16,
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "This place is for our eyes only!", yell = false},
    {text = "We will live and let you die!", yell = false},
    {text = "I will die another day!", yell = false},
    {text = "We have license to kill!", yell = false},
}

mtype:register(monster)
