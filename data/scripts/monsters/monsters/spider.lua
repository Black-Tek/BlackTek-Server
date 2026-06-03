local mtype = Game.createMonsterType("Spider")
local monster = {}

monster.name = "Spider"
monster.description = "a spider"
monster.experience = 12
monster.race = "venom"
monster.maxHealth = 20
monster.health = 20
monster.speed = 152
monster.manaCost = 210
monster.corpse = 5961
monster.outfit = { lookType = 30 }
monster.runHealth = 6
monster.changeTarget = {
    interval = 4000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = true,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = true,
    pushable = true,
    canPushItems = false,
    canPushCreatures = false,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
    canWalkOnPoison = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -9,
    },
}
monster.defenses = {
    defense = 2,
    armor = 2,
}
monster.elements = {
    {type = COMBAT_FIREDAMAGE, percent = -20},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 65000,
        maxCount = 5,
    },
    {
        id = "spider fangs",
        chance = 950,
    },
}

mtype:register(monster)
