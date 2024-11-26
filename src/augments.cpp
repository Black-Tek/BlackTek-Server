// Credits: BlackTek Server Creator Codinablack@github.com.
// This project is based of otland's The Forgottenserver.
// Any and all code taken from otland's The Forgottenserver is licensed under GPL 2.0
// Any code Authored by: Codinablack or BlackTek contributers, that is not already licensed, is hereby licesned MIT. 
// The GPL 2.0 License that can be found in the LICENSE file.
// All code found in this file is licensed under MIT and can be found in the LICENSE file.

#include <toml++/toml.hpp>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "augments.h"

static std::unordered_map<std::string, std::shared_ptr<Augment>> global_augments {};

std::shared_ptr<Augment> Augments::MakeAugment(std::string_view augmentName)
{
    auto it = global_augments.find(augmentName.data());

    if (it != global_augments.end()) {
        auto augmentClone = Augment::MakeAugment(it->second);
        return augmentClone;
    }
    std::cout << "Failed to find augment named : " << augmentName;
    return nullptr;
}

void Augments::loadAll() {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".toml") {

            try {

                auto file = toml::parse_file(entry.path().string());

                for (const auto& [index, entry] : file) {

                    toml::table augment_info = *entry.as_table();
                    auto modifier_data = augment_info["modifiers"];
                    std::string name = augment_info["name"].value_or("unknown");
                    std::string description = augment_info["description"].value_or("unknown");

                    if (name == "unknown") {
                        std::cout << "Error: All augments require a name \n";
                        break;
                    }
                    std::shared_ptr<Augment> augment = Augment::MakeAugment(name);
                    augment->setDescription(description);
                    if (auto mod_list = modifier_data.as_array()) {
                        mod_list->for_each([augment, name](auto&& prop) {
                            if (prop.is_table()) {
                                auto& table = *prop.as_table();
                                std::string_view modType =          table["mod"].value_or("none");
                                uint16_t amount =                   table["value"].value_or(0);
                                std::string_view factor =           table["factor"].value_or("none");
                                uint8_t chance =                    table["chance"].value_or(100);
                                std::string_view damageType =       table["damage"].value_or("none");
                                std::string_view originType =       table["origin"].value_or("none");
                                std::string_view creatureType =     table["target"].value_or("none");
                                std::string_view race =             table["race"].value_or("none");
                                std::string_view creatureName =     table["monster"].value_or("none");

                                // To-do: Change all static methods used below to accept const values and use const variables above.
                                // also change the 'Get' methods into 'parse' methods for clarity
                                if (ParseStance(modType) == ATTACK_MOD) {

                                    std::shared_ptr<DamageModifier> damage_modifier = DamageModifier::makeModifier(
                                        ParseStance(modType),
                                        ParseAttackModifier(modType),
                                        amount,
                                        ParseFactor(factor),
                                        chance,
                                        ParseDamage(damageType),
                                        ParseOrigin(originType),
                                        ParseCreatureType(creatureType),
                                        ParseRaceType(race));

                                    // To-do : create a new variable for storing monster names to not conflict with other aux variables
                                    if (modType == "conversion") {
                                        auto convertedType = ParseDamage(table["toDamage"].value_or("none"));
                                        damage_modifier->setTransformDamageType(convertedType);
                                    }

                                    if (creatureName != "none") {
                                        damage_modifier->setCreatureName(creatureName);
                                    }

                                    augment->addModifier(damage_modifier);

                                } else if (ParseStance(modType) == DEFENSE_MOD) {

                                    std::shared_ptr<DamageModifier> damage_modifier = DamageModifier::makeModifier(
                                        ParseStance(modType),
                                        ParseDefenseModifier(modType),
                                        amount,
                                        ParseFactor(factor),
                                        chance,
                                        ParseDamage(damageType),
                                        ParseOrigin(originType),
                                        ParseCreatureType(creatureType),
                                        ParseRaceType(race));

                                    if (modType == "reform") {
                                        auto reformType = ParseDamage(table["toDamage"].value_or("none"));
                                        damage_modifier->setTransformDamageType(reformType);
                                    }

                                    if (creatureName != "none") {
                                        damage_modifier->setCreatureName(creatureName);
                                    }

                                    augment->addModifier(damage_modifier);

                                } else {

                                    std::cout << "Modifier has unknown stance " << table["stance"] << "\n";
                                }
                            }
                        });
                    }
                    AddAugment(augment);
                }
            } catch (const toml::parse_error& err) {
                std::cerr << "Error parsing file " << entry.path() << ": " << err << "\n";
            }
        }
    }
}

void Augments::clearAll()
{
    global_augments.clear();
}

void Augments::reload()
{
    clearAll();
    loadAll();
    // if (config::deleteOldAugments) { CleanPlayerAugments(); }
}

const ModifierStance Augments::ParseStance(std::string_view modName) noexcept
{
    if (ParseAttackModifier(modName) != ATTACK_MODIFIER_NONE) {
        return ATTACK_MOD;
    } else if (ParseDefenseModifier(modName) != DEFENSE_MODIFIER_NONE) {
        return DEFENSE_MOD;
    }
    std::cout << "[::Augment Error::] no such mod by type name : " << std::string{ modName } << " /n";
    return NO_MOD;
}

const ModFactor Augments::ParseFactor(std::string_view factor) noexcept
{
    std::string f_type = std::string{ factor };
    if (f_type == "flat") {
        return FLAT_MODIFIER;
    }
    return PERCENT_MODIFIER;
}

const CombatType_t Augments::ParseDamage(std::string_view damageName) noexcept
{   // Note : If you add values to the list you must increase the size manually
    // current size is : 21
    const std::array<std::pair<std::string_view, CombatType_t>, 21> static_map{ {
        {"none",            COMBAT_NONE},
        {"all",             COMBAT_NONE},
        {"physical",        COMBAT_PHYSICALDAMAGE},
        {"melee",           COMBAT_PHYSICALDAMAGE},
        {"energy",          COMBAT_ENERGYDAMAGE},
        {"electric",        COMBAT_ENERGYDAMAGE},
        {"earth",           COMBAT_EARTHDAMAGE},
        {"poison",          COMBAT_EARTHDAMAGE},
        {"fire",            COMBAT_FIREDAMAGE},
        {"lifedrain",       COMBAT_LIFEDRAIN},
        {"lifesteal",       COMBAT_LIFEDRAIN},
        {"lifeleech",       COMBAT_LIFEDRAIN},
        {"manadrain",       COMBAT_MANADRAIN},
        {"manasteal",       COMBAT_MANADRAIN},
        {"manaleech",       COMBAT_MANADRAIN},
        {"drown",           COMBAT_DROWNDAMAGE},
        {"water",           COMBAT_DROWNDAMAGE},
        {"ice",             COMBAT_ICEDAMAGE},
        {"holy",            COMBAT_HOLYDAMAGE},
        {"death",           COMBAT_DEATHDAMAGE},
        {"curse",           COMBAT_DEATHDAMAGE},
    } };

    for (const auto& [key, value] : static_map) {
        if (key == damageName) {
            return value;
        }
    }

    return COMBAT_NONE;
}

const CombatOrigin Augments::ParseOrigin(std::string_view originName) noexcept
{   // Note : If you add values to the list you must increase the size manually
    // current size is : 14
    const std::array<std::pair<std::string_view, CombatOrigin>, 14> static_map{ {
        {"none",            ORIGIN_NONE},
        {"all",             ORIGIN_NONE},
        {"condition",       ORIGIN_CONDITION},
        {"spell",           ORIGIN_SPELL},
        {"melee",           ORIGIN_MELEE},
        {"ranged",          ORIGIN_RANGED},
        {"absorb",          ORIGIN_ABSORB},
        {"restore",         ORIGIN_RESTORE},
        {"reflect",         ORIGIN_REFLECT},
        {"deflect",         ORIGIN_DEFLECT},
        {"ricochet",        ORIGIN_RICOCHET},
        {"piercing",        ORIGIN_PIERCING},
        {"augment",         ORIGIN_AUGMENT},
        {"imbuement",       ORIGIN_IMBUEMENT},
    } };

    for (const auto& [key, value] : static_map) {
        if (key == originName) {
            return value;
        }
    }

    return ORIGIN_NONE;
}

const ModifierAttackType Augments::ParseAttackModifier(std::string_view modName) noexcept {
    // Note : If you add values to the list you must increase the size manually
    // current size is : 8
    const std::array<std::pair<std::string_view, ModifierAttackType>, 8> static_map{ {
        {"none",            ATTACK_MODIFIER_NONE},
        {"lifesteal",       ATTACK_MODIFIER_LIFESTEAL},
        {"manasteal",       ATTACK_MODIFIER_MANASTEAL},
        {"staminasteal",    ATTACK_MODIFIER_STAMINASTEAL},
        {"soulsteal",       ATTACK_MODIFIER_SOULSTEAL},
        {"critical",        ATTACK_MODIFIER_CRITICAL},
        {"piercing",        ATTACK_MODIFIER_PIERCING},
        {"conversion",      ATTACK_MODIFIER_CONVERSION},
    } };

    for (const auto& [key, value] : static_map) {
        if (key == modName) {
            return value;
        }
    }

    return ATTACK_MODIFIER_NONE;
}

const ModifierDefenseType Augments::ParseDefenseModifier(std::string_view modName) noexcept
{   // Note : If you add values to the list you must increase the size manually
    // current size is : 10
    const std::array<std::pair<std::string_view, ModifierDefenseType>, 10> static_map{ {
        {"none",            DEFENSE_MODIFIER_NONE},
        {"absorb",          DEFENSE_MODIFIER_ABSORB},
        {"restore",         DEFENSE_MODIFIER_RESTORE},
        {"replenish",       DEFENSE_MODIFIER_REPLENISH},
        {"revive",          DEFENSE_MODIFIER_REVIVE},
        {"reflect",         DEFENSE_MODIFIER_REFLECT},
        {"deflect",         DEFENSE_MODIFIER_DEFLECT},
        {"ricochet",        DEFENSE_MODIFIER_RICOCHET},
        {"resist",          DEFENSE_MODIFIER_RESIST},
        {"reform",          DEFENSE_MODIFIER_REFORM},
    } };

    for (const auto& [key, value] : static_map) {
        if (key == modName) {
            return value;
        }
    }

    return DEFENSE_MODIFIER_NONE;
}


const RaceType_t Augments::ParseRaceType(std::string_view raceType) noexcept {
    // Note : If you add values to the list you must increase the size manually
    // current size is : 6
    const std::array<std::pair<std::string_view, RaceType_t>, 6> static_map{ {
        {"none",            RACE_NONE},
        {"venom",           RACE_VENOM},
        {"blood",           RACE_BLOOD},
        {"undead",          RACE_UNDEAD},
        {"fire",            RACE_FIRE},
        {"energy",          RACE_ENERGY},
    } };

    for (const auto& [key, value] : static_map) {
        if (key == raceType) {
            return value;
        }
    }

    return RACE_NONE;
}

const CreatureType_t Augments::ParseCreatureType(std::string_view creatureType) noexcept {
    // Note : If you add values to the list you must increase the size manually
    // current size is : 14
    const std::array<std::pair<std::string_view, CreatureType_t>, 14> static_map{ {
        {"player",                      CREATURETYPE_PLAYER},
        {"monster",                     CREATURETYPE_MONSTER},
        {"npc",                         CREATURETYPE_NPC},
        {"allsummon",                   CREATURETYPE_SUMMON_ALL},
        {"summons",                     CREATURETYPE_SUMMON_ALL},
        {"ownedsummon",                 CREATURETYPE_SUMMON_OWN},
        {"mysummon",                    CREATURETYPE_SUMMON_OWN},
        {"hostilesummon",               CREATURETYPE_SUMMON_HOSTILE},
        {"enemysummon",                 CREATURETYPE_SUMMON_HOSTILE},
        {"guildsummon",                 CREATURETYPE_SUMMON_GUILD},
        {"partysummon",                 CREATURETYPE_SUMMON_PARTY},
        {"boss",                        CREATURETYPE_BOSS},
        {"none",                        CREATURETYPE_ATTACKABLE},
        {"all",                         CREATURETYPE_ATTACKABLE},
    } };

    for (const auto& [key, value] : static_map) {
        if (key == creatureType) {
            return value;
        }
    }

    return CREATURETYPE_ATTACKABLE;
}

void Augments::AddAugment(std::shared_ptr<Augment> augment) {
    auto [it, inserted] = global_augments.try_emplace(augment->getName().data(), augment);
    if (!inserted) {
        std::cout << "[Warning][Augments] " << augment->getName() << " already exists! \n";
    }
}

void Augments::RemoveAugment(std::shared_ptr<Augment> augment) {
    auto it = global_augments.find(augment->getName().data());
    if (it != global_augments.end()) {
        global_augments.erase(it);
    }
}

void Augments::RemoveAugment(std::string_view augName) {
    auto it = global_augments.find(std::string(augName));
    if (it != global_augments.end()) {
        global_augments.erase(it);
    }
}

void Augments::RemoveAugment(std::string augName) {
    auto it = global_augments.find(augName);
    if (it != global_augments.end()) {
        global_augments.erase(it);
    }
}

std::shared_ptr<Augment> Augments::GetAugment(std::string_view augName)
{
    auto it = global_augments.find(augName.data());
    if (it != global_augments.end()) {
        auto augment = Augment::MakeAugment(it->second);
        return augment;
    }
    return nullptr;
}
