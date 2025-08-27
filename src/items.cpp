// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "items.h"
#include "spells.h"
#include "movement.h"
#include "weapons.h"

#include <toml++/toml.hpp>
#include <filesystem>
#include "tools.h"
#include <fmt/color.h>

extern MoveEvents* g_moveEvents;
extern Weapons* g_weapons;

gtl::flat_hash_map<uint32_t, SkillRegistry> item_skills;
gtl::flat_hash_map<uint32_t, ItemBuff> item_buffs, item_debuffs;

const gtl::flat_hash_map<std::string, ItemParseAttributes_t> ItemParseAttributesMap = {
	{"type", ITEM_PARSE_TYPE},
	{"description", ITEM_PARSE_DESCRIPTION},
	{"runespellname", ITEM_PARSE_RUNESPELLNAME},
	{"weight", ITEM_PARSE_WEIGHT},
	{"showcount", ITEM_PARSE_SHOWCOUNT},
	{"armor", ITEM_PARSE_ARMOR},
	{"defense", ITEM_PARSE_DEFENSE},
	{"extradef", ITEM_PARSE_EXTRADEF},
	{"attack", ITEM_PARSE_ATTACK},
	{"attackspeed", ITEM_PARSE_ATTACK_SPEED},
	{"classification", ITEM_PARSE_CLASSIFICATION},
	{"tier", ITEM_PARSE_TIER},
	{"rotateto", ITEM_PARSE_ROTATETO},
	{"moveable", ITEM_PARSE_MOVEABLE},
	{"movable", ITEM_PARSE_MOVEABLE},
	{"blockprojectile", ITEM_PARSE_BLOCKPROJECTILE},
	{"allowpickupable", ITEM_PARSE_PICKUPABLE},
	{"pickupable", ITEM_PARSE_PICKUPABLE},
	{"forceserialize", ITEM_PARSE_FORCESERIALIZE},
	{"forcesave", ITEM_PARSE_FORCESERIALIZE},
	{"floorchange", ITEM_PARSE_FLOORCHANGE},
	{"corpsetype", ITEM_PARSE_CORPSETYPE},
	{"containersize", ITEM_PARSE_CONTAINERSIZE},
	{"fluidsource", ITEM_PARSE_FLUIDSOURCE},
	{"readable", ITEM_PARSE_READABLE},
	{"writeable", ITEM_PARSE_WRITEABLE},
	{"maxtextlen", ITEM_PARSE_MAXTEXTLEN},
	{"writeonceitemid", ITEM_PARSE_WRITEONCEITEMID},
	{"weapontype", ITEM_PARSE_WEAPONTYPE},
	{"slottype", ITEM_PARSE_SLOTTYPE},
	{"ammotype", ITEM_PARSE_AMMOTYPE},
	{"shoottype", ITEM_PARSE_SHOOTTYPE},
	{"effect", ITEM_PARSE_EFFECT},
	{"range", ITEM_PARSE_RANGE},
	{"stopduration", ITEM_PARSE_STOPDURATION},
	{"decayto", ITEM_PARSE_DECAYTO},
	{"transformequipto", ITEM_PARSE_TRANSFORMEQUIPTO},
	{"transformdeequipto", ITEM_PARSE_TRANSFORMDEEQUIPTO},
	{"duration", ITEM_PARSE_DURATION},
	{"showduration", ITEM_PARSE_SHOWDURATION},
	{"charges", ITEM_PARSE_CHARGES},
	{"showcharges", ITEM_PARSE_SHOWCHARGES},
	{"showattributes", ITEM_PARSE_SHOWATTRIBUTES},
	{"hitchance", ITEM_PARSE_HITCHANCE},
	{"maxhitchance", ITEM_PARSE_MAXHITCHANCE},
	{"invisible", ITEM_PARSE_INVISIBLE},
	{"speed", ITEM_PARSE_SPEED},
	{"healthgain", ITEM_PARSE_HEALTHGAIN},
	{"healthticks", ITEM_PARSE_HEALTHTICKS},
	{"managain", ITEM_PARSE_MANAGAIN},
	{"manaticks", ITEM_PARSE_MANATICKS},
	{"manashield", ITEM_PARSE_MANASHIELD},
	{"skillsword", ITEM_PARSE_SKILLSWORD},
	{"skillaxe", ITEM_PARSE_SKILLAXE},
	{"skillclub", ITEM_PARSE_SKILLCLUB},
	{"skilldist", ITEM_PARSE_SKILLDIST},
	{"skillfish", ITEM_PARSE_SKILLFISH},
	{"skillshield", ITEM_PARSE_SKILLSHIELD},
	{"skillfist", ITEM_PARSE_SKILLFIST},
	{"maxhitpoints", ITEM_PARSE_MAXHITPOINTS},
	{"maxhitpointspercent", ITEM_PARSE_MAXHITPOINTSPERCENT},
	{"maxmanapoints", ITEM_PARSE_MAXMANAPOINTS},
	{"maxmanapointspercent", ITEM_PARSE_MAXMANAPOINTSPERCENT},
	{"magicpoints", ITEM_PARSE_MAGICPOINTS},
	{"magiclevelpoints", ITEM_PARSE_MAGICPOINTS},
	{"magicpointspercent", ITEM_PARSE_MAGICPOINTSPERCENT},
	{"criticalhitchance", ITEM_PARSE_CRITICALHITCHANCE},
	{"criticalhitamount", ITEM_PARSE_CRITICALHITAMOUNT},
	{"lifeleechchance", ITEM_PARSE_LIFELEECHCHANCE},
	{"lifeleechamount", ITEM_PARSE_LIFELEECHAMOUNT},
	{"manaleechchance", ITEM_PARSE_MANALEECHCHANCE},
	{"manaleechamount", ITEM_PARSE_MANALEECHAMOUNT},
	{"suppressdrunk", ITEM_PARSE_SUPPRESSDRUNK},
	{"suppressenergy", ITEM_PARSE_SUPPRESSENERGY},
	{"suppressfire", ITEM_PARSE_SUPPRESSFIRE},
	{"suppresspoison", ITEM_PARSE_SUPPRESSPOISON},
	{"suppressdrown", ITEM_PARSE_SUPPRESSDROWN},
	{"suppressphysical", ITEM_PARSE_SUPPRESSPHYSICAL},
	{"suppressfreeze", ITEM_PARSE_SUPPRESSFREEZE},
	{"suppressdazzle", ITEM_PARSE_SUPPRESSDAZZLE},
	{"suppresscurse", ITEM_PARSE_SUPPRESSCURSE},
	{"field", ITEM_PARSE_FIELD},
	{"replaceable", ITEM_PARSE_REPLACEABLE},
	{"partnerdirection", ITEM_PARSE_PARTNERDIRECTION},
	{"leveldoor", ITEM_PARSE_LEVELDOOR},
	{"maletransformto", ITEM_PARSE_MALETRANSFORMTO},
	{"malesleeper", ITEM_PARSE_MALETRANSFORMTO},
	{"femaletransformto", ITEM_PARSE_FEMALETRANSFORMTO},
	{"femalesleeper", ITEM_PARSE_FEMALETRANSFORMTO},
	{"transformto", ITEM_PARSE_TRANSFORMTO},
	{"destroyto", ITEM_PARSE_DESTROYTO},
	{"elementice", ITEM_PARSE_ELEMENTICE},
	{"elementearth", ITEM_PARSE_ELEMENTEARTH},
	{"elementfire", ITEM_PARSE_ELEMENTFIRE},
	{"elementenergy", ITEM_PARSE_ELEMENTENERGY},
	{"elementdeath", ITEM_PARSE_ELEMENTDEATH},
	{"elementholy", ITEM_PARSE_ELEMENTHOLY},
	{"walkstack", ITEM_PARSE_WALKSTACK},
	{"blocking", ITEM_PARSE_BLOCKING},
	{"allowdistread", ITEM_PARSE_ALLOWDISTREAD},
	{"storeitem", ITEM_PARSE_STOREITEM},
	{"imbuementslots", 	ITEM_PARSE_IMBUEMENT_SLOT},
	{"worth", ITEM_PARSE_WORTH},
	{"augment", ITEM_PARSE_AUGMENT},
};

const gtl::flat_hash_map<std::string, ItemTypes_t> ItemTypesMap = {
	{"key", ITEM_TYPE_KEY},
	{"magicfield", ITEM_TYPE_MAGICFIELD},
	{"container", ITEM_TYPE_CONTAINER},
	{"depot", ITEM_TYPE_DEPOT},
	{"mailbox", ITEM_TYPE_MAILBOX},
	{"trashholder", ITEM_TYPE_TRASHHOLDER},
	{"teleport", ITEM_TYPE_TELEPORT},
	{"door", ITEM_TYPE_DOOR},
	{"bed", ITEM_TYPE_BED},
	{"rune", ITEM_TYPE_RUNE},
	{"rewardchest", ITEM_TYPE_REWARDCHEST},
};

const gtl::flat_hash_map<std::string, tileflags_t> TileStatesMap = {
	{"down", TILESTATE_FLOORCHANGE_DOWN},
	{"north", TILESTATE_FLOORCHANGE_NORTH},
	{"south", TILESTATE_FLOORCHANGE_SOUTH},
	{"southalt", TILESTATE_FLOORCHANGE_SOUTH_ALT},
	{"west", TILESTATE_FLOORCHANGE_WEST},
	{"east", TILESTATE_FLOORCHANGE_EAST},
	{"eastalt", TILESTATE_FLOORCHANGE_EAST_ALT},
};

const gtl::flat_hash_map<std::string, RaceType_t> RaceTypesMap = {
	{"venom", RACE_VENOM},
	{"blood", RACE_BLOOD},
	{"undead", RACE_UNDEAD},
	{"fire", RACE_FIRE},
	{"energy", RACE_ENERGY},
};

const gtl::flat_hash_map<std::string, WeaponType_t> WeaponTypesMap = {
	{"sword", WEAPON_SWORD},
	{"club", WEAPON_CLUB},
	{"axe", WEAPON_AXE},
	{"shield", WEAPON_SHIELD},
	{"distance", WEAPON_DISTANCE},
	{"wand", WEAPON_WAND},
	{"ammunition", WEAPON_AMMO},
	{"quiver", WEAPON_QUIVER},
};

const gtl::flat_hash_map<std::string, FluidTypes_t> FluidTypesMap = {
	{"water", FLUID_WATER},
	{"blood", FLUID_BLOOD},
	{"beer", FLUID_BEER},
	{"slime", FLUID_SLIME},
	{"lemonade", FLUID_LEMONADE},
	{"milk", FLUID_MILK},
	{"mana", FLUID_MANA},
	{"life", FLUID_LIFE},
	{"oil", FLUID_OIL},
	{"urine", FLUID_URINE},
	{"coconut", FLUID_COCONUTMILK},
	{"wine", FLUID_WINE},
	{"mud", FLUID_MUD},
	{"fruitjuice", FLUID_FRUITJUICE},
	{"lava", FLUID_LAVA},
	{"rum", FLUID_RUM},
	{"swamp", FLUID_SWAMP},
	{"tea", FLUID_TEA},
	{"mead", FLUID_MEAD},
};

bool Items::addItemSkill(uint32_t item_id, std::string_view skill_name, const std::shared_ptr<CustomSkill>& skill)
{
    auto& skillMap = item_skills[item_id];
    return skillMap.try_emplace(skill_name, skill).second;
}

std::optional<std::shared_ptr<CustomSkill>> Items::getItemSkill(std::string_view skill_name, uint32_t item_id)
{
    if (auto it = item_skills.find(item_id); it != item_skills.end()) 
    {
        const auto& skills = it->second;
        if (auto skillIt = skills.find(skill_name); skillIt != skills.end()) 
        {
            return skillIt->second;
        }
    }
    return std::nullopt;
}

static bool addItemBuff(uint32_t item_id, const ItemBuff& buff)
{
    return item_buffs.try_emplace(item_id, buff).second;
}

static std::optional<ItemBuff> getItemBuff(uint32_t item_id)
{
    if (auto it = item_buffs.find(item_id); it != item_buffs.end()) 
    {
        return it->second;
    }
    return std::nullopt;
}

static bool addItemDebuff(uint32_t item_id, const ItemBuff& debuff)
{
    return item_debuffs.try_emplace(item_id, debuff).second;
}

static std::optional<ItemBuff> getItemDebuff(uint32_t item_id)
{
    if (auto it = item_debuffs.find(item_id); it != item_debuffs.end())
    {
        return it->second;
    }
    return std::nullopt;
}


Items::Items()
{
	items.reserve(30000);
	nameToItems.reserve(30000);
}

void Items::clear()
{
	items.clear();
	nameToItems.clear();
	currencyItems.clear();
	inventory.clear();
}

bool Items::reload()
{
	clear();
	loadFromDat(g_config.getString(ConfigManager::ITEMS_DAT_PATH));

	if (!loadFromToml()) {
		return false;
	}

	g_moveEvents->reload();
	g_weapons->reload();
	g_weapons->loadDefaults();
	return true;
}

bool Items::loadFromDat(const std::string& file)
{
    std::ifstream fin(datFile, std::ios::binary);
    if (!fin.is_open()) {
        fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "> ERROR: {:s}\n", "Unable to load Tibia.dat, from specified path: " + file);
        return false;
    }

    // Skip signature
    fin.seekg(4, std::ios::cur);

    // Read item count
    uint16_t m_loadedItemsCount = 0;
    fin.read(reinterpret_cast<char *>(&m_loadedItemsCount), sizeof(m_loadedItemsCount));

    // Skip the outfit count, effect count, missiles count (distance effects)
    fin.seekg(6, std::ios::cur); // uint16_t * 3

    // Resize items vector
    items.resize(m_loadedItemsCount + 1);

    for (uint16_t id = 100; id < items.size(); ++id) {
        ItemType& iType = items[id];
        iType.clientId = id;
        iType.id = id;
        unserializeDatItem(iType, fin);
    }

	items.shrink_to_fit();
	return true;
}

bool Items::loadFromToml()
{
    namespace fs = std::filesystem;
    const std::string itemsDir = "data/items/";
    if (!fs::exists(itemsDir)) {
        std::cout << "[Error - Items::loadFromToml] Directory " << itemsDir << " not found" << std::endl;
        return false;
    }

    bool success = true;
    for (const auto& entry : fs::directory_iterator(itemsDir)) {
        if (entry.path().extension() != ".toml") {
            continue;
        }

        try {
            toml::table tbl = toml::parse_file(entry.path().string());
            auto itemsArray = tbl["items"].as_array(); // Changed from "item" to "items"
            if (!itemsArray) {
                std::cout << "[Warning - Items::loadFromToml] No 'items' array in " << entry.path() << std::endl;
                continue;
            }

            for (const auto& item : *itemsArray) {
                if (!item.is_table()) {
                    std::cout << "[Warning - Items::loadFromToml] Invalid item entry in " << entry.path() << std::endl;
                    continue;
                }

                const toml::table& itemTable = *item.as_table();
                if (auto idNode = itemTable["id"]) {
                    uint16_t id = static_cast<uint16_t>(idNode.as_integer()->get());
                    parseItemToml(itemTable, id);
                } else if (auto fromIdNode = itemTable["fromid"], toIdNode = itemTable["toid"]; fromIdNode && toIdNode) {
                    uint16_t fromId = static_cast<uint16_t>(fromIdNode.as_integer()->get());
                    uint16_t toId = static_cast<uint16_t>(toIdNode.as_integer()->get());
                    if (fromId > toId) {
                        std::cout << "[Warning - Items::loadFromToml] fromid (" << fromId << ") > toid (" << toId << ") in " << entry.path() << std::endl;
                        continue;
                    }
                    for (uint16_t id = fromId; id <= toId; ++id) {
                        parseItemToml(itemTable, id);
                    }
                } else {
                    std::cout << "[Warning - Items::loadFromToml] Missing id or fromid/toid in " << entry.path() << std::endl;
                }
            }
        } catch (const toml::parse_error& e) {
            std::cout << "[Error - Items::loadFromToml] Parse error in " << entry.path() << ": " << e.description() << std::endl;
            success = false;
        }
    }

    if (!success) {
        return false;
    }

    buildInventoryList();
    return true;
}

void Items::buildInventoryList()
{
    inventory.clear();
    inventory.reserve(items.size() / 2); // Rough estimate to reduce reallocations
    for (const auto& type : items) {
        if (type.id == 0) {
            continue; // Skip uninitialized items
        }
        if (type.weaponType != WEAPON_NONE || type.ammoType != AMMO_NONE ||
            type.attack != 0 || type.defense != 0 || type.extraDefense != 0 ||
            type.armor != 0 || (type.slotPosition & (SLOTP_NECKLACE | SLOTP_RING | SLOTP_AMMO | SLOTP_FEET | SLOTP_HEAD | SLOTP_ARMOR | SLOTP_LEGS))) {
            inventory.push_back(type.clientId);
        }
    }
    inventory.shrink_to_fit();
    std::sort(inventory.begin(), inventory.end());
}

void Items::parseItemToml(const toml::table& itemTable, uint16_t id)
{
    if (id >= items.size()) {
        items.resize(id + 1);
    }

    ItemType& it = items[id];
    if (it.id == 0) {
        it.id = id;
    }
    else if (!it.name.empty()) {
        std::cout << "[Warning - Items::parseItemToml] Duplicate item definition for id: " << id << std::endl;
        return;
    }

    Abilities& abilities = it.getAbilities();

    // Process all attributes in a single pass
    for (const auto& [key, value] : itemTable) {
        std::string keyStr = asLowerCaseString(std::string(key));

        // Handle top-level attributes explicitly
        if (keyStr == "id") {
            continue; // Skip id as it's already handled
        }
        else if (keyStr == "name") {
            if (value.is_string()) {
                it.name = value.as_string()->get();
                nameToItems.emplace(asLowerCaseString(it.name), id);
            }
            continue;
        }
        else if (keyStr == "article") {
            if (value.is_string()) {
                it.article = value.as_string()->get();
            }
            continue;
        }
        else if (keyStr == "plural") {
            if (value.is_string()) {
                it.pluralName = value.as_string()->get();
            }
            continue;
        }
        else if (keyStr == "skills") {
            if (value.is_array()) {
                for (const auto& skillEntry : *value.as_array()) {
                    if (skillEntry.is_table()) {
                        const toml::table& skillTable = *skillEntry.as_table();

                        std::string name = skillTable["name"] ? skillTable["name"].as_string()->get() : "";
                        int level = skillTable["level"] ? static_cast<int>(skillTable["level"].as_integer()->get()) : 1;
                        int max = skillTable["max"] ? static_cast<int>(skillTable["max"].as_integer()->get()) : 10;
                        std::string formula_type = skillTable["formula"] ? skillTable["formula"].as_string()->get() : "default";
                        int threshold = skillTable["threshold"] ? static_cast<int>(skillTable["threshold"].as_integer()->get()) : 10;
                        int difficulty = skillTable["difficulty"] ? static_cast<int>(skillTable["difficulty"].as_integer()->get()) : 50;
                        double multiplier = skillTable["multiplier"] ? skillTable["multiplier"].as_floating_point()->get() : 1.0;

                        FormulaType formula = Components::Skills::ParseFormula(formula_type);

                        if (not name.empty()) {
                            auto customSkill = Components::Skills::CustomSkill::make_skill(formula, threshold, difficulty, multiplier);
                            Items::addItemSkill(id, name, customSkill);
                            continue;
                        }
                    }
                }
            }
            continue;
        }
        else if (keyStr == "buffs") {
            if (value.is_array()) {
                for (const auto& traitEntry : *value.as_array()) {
                    if (traitEntry.is_table()) {
                        const toml::table& buffs = *traitEntry.as_table();
                        std::string name = buffs["name"] ? buffs["name"].as_string()->get() : "none";

                        if (name == "none")
                        {
                            // log here the skipped buff
                            continue;
                        }

                        int value = buffs["value"] ? static_cast<int>(buffs["value"].as_integer()->get()) : 0;

                        if (auto buff_type = buffs["type"].as_string()->get() == "debuff")
                        {
                            addItemDebuff(id, { name, value });
                            continue;
                        }
                        else
                        {
                            addItemBuff(id, {name, value});
                            continue;
                        }
                    }
                }
            }
            continue;
        }

        auto parseAttribute = ItemParseAttributesMap.find(keyStr);
        if (parseAttribute == ItemParseAttributesMap.end()) {
            std::cout << "[Warning - Items::parseItemToml] Unknown attribute '" << key << "' for item id: " << id << std::endl;
            continue;
        }

        switch (parseAttribute->second) {
        case ITEM_PARSE_TYPE:
            if (value.is_string()) {
                std::string typeStr = asLowerCaseString(value.as_string()->get());
                if (auto it2 = ItemTypesMap.find(typeStr); it2 != ItemTypesMap.end()) {
                    it.type = it2->second;
                    if (it.type == ITEM_TYPE_CONTAINER) {
                        it.group = ITEM_GROUP_CONTAINER;
                    }
                }
            }
            break;

        case ITEM_PARSE_DESCRIPTION:
            if (value.is_string()) it.description = value.as_string()->get();
            break;

        case ITEM_PARSE_RUNESPELLNAME:
            if (value.is_string()) it.runeSpellName = value.as_string()->get();
            break;

        case ITEM_PARSE_WEIGHT:
            if (value.is_integer()) it.weight = static_cast<uint32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_SHOWCOUNT:
            if (value.is_boolean()) it.showCount = value.as_boolean()->get();
            break;

        case ITEM_PARSE_ARMOR:
            if (value.is_integer()) it.armor = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_DEFENSE:
            if (value.is_integer()) it.defense = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_EXTRADEF:
            if (value.is_integer()) it.extraDefense = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_ATTACK:
            if (value.is_integer()) it.attack = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_ATTACK_SPEED:
            if (value.is_integer()) {
                it.attackSpeed = static_cast<uint32_t>(value.as_integer()->get());
                if (it.attackSpeed > 0 && it.attackSpeed < 100) {
                    std::cout << "[Warning - Items::parseItemToml] AttackSpeed < 100 for item " << id << ", setting to 100" << std::endl;
                    it.attackSpeed = 100;
                }
            }
            break;

        case ITEM_PARSE_CLASSIFICATION:
            if (value.is_string()) it.classification = value.as_string()->get();
            break;

        case ITEM_PARSE_TIER:
            if (value.is_string()) it.tier = value.as_string()->get();
            break;

        case ITEM_PARSE_IMBUEMENT_SLOT:
            if (value.is_integer()) it.imbuementslots = static_cast<int16_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_ROTATETO:
            if (value.is_integer()) it.rotateTo = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_MOVEABLE:
            if (value.is_boolean()) it.moveable = value.as_boolean()->get();
            break;

        case ITEM_PARSE_BLOCKPROJECTILE:
            if (value.is_boolean()) it.blockProjectile = value.as_boolean()->get();
            break;

        case ITEM_PARSE_PICKUPABLE:
            if (value.is_boolean()) it.allowPickupable = value.as_boolean()->get();
            break;

        case ITEM_PARSE_FORCESERIALIZE:
            if (value.is_boolean()) it.forceSerialize = value.as_boolean()->get();
            break;

        case ITEM_PARSE_FLOORCHANGE:
            if (value.is_string()) {
                std::string floorChangeStr = asLowerCaseString(value.as_string()->get());
                if (auto it2 = TileStatesMap.find(floorChangeStr); it2 != TileStatesMap.end()) {
                    it.floorChange |= it2->second;
                }
            }
            break;

        case ITEM_PARSE_CORPSETYPE:
            if (value.is_string()) {
                std::string corpseTypeStr = asLowerCaseString(value.as_string()->get());
                if (auto it2 = RaceTypesMap.find(corpseTypeStr); it2 != RaceTypesMap.end()) {
                    it.corpseType = it2->second;
                }
            }
            break;

        case ITEM_PARSE_CONTAINERSIZE:
            if (value.is_integer()) it.maxItems = static_cast<uint16_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_FLUIDSOURCE:
            if (value.is_string()) {
                std::string fluidSourceStr = asLowerCaseString(value.as_string()->get());
                if (auto it2 = FluidTypesMap.find(fluidSourceStr); it2 != FluidTypesMap.end()) {
                    it.fluidSource = it2->second;
                }
            }
            break;

        case ITEM_PARSE_READABLE:
            if (value.is_boolean()) it.canReadText = value.as_boolean()->get();
            break;

        case ITEM_PARSE_WRITEABLE:
            if (value.is_boolean()) {
                it.canWriteText = value.as_boolean()->get();
                it.canReadText |= it.canWriteText;
            }
            break;

        case ITEM_PARSE_MAXTEXTLEN:
            if (value.is_integer()) it.maxTextLen = static_cast<uint16_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_WRITEONCEITEMID:
            if (value.is_integer()) it.writeOnceItemId = static_cast<uint16_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_WEAPONTYPE:
            if (value.is_string()) {
                std::string weaponTypeStr = asLowerCaseString(value.as_string()->get());
                if (auto it2 = WeaponTypesMap.find(weaponTypeStr); it2 != WeaponTypesMap.end()) {
                    it.weaponType = it2->second;
                }
            }
            break;

        case ITEM_PARSE_SLOTTYPE:
            if (value.is_string()) {
                std::string slotTypeStr = asLowerCaseString(value.as_string()->get());
                if (slotTypeStr == "head") { it.slotPosition |= SLOTP_HEAD; it.equipSlot = SLOTP_HEAD; }
                else if (slotTypeStr == "body") { it.slotPosition |= SLOTP_ARMOR; it.equipSlot = SLOTP_ARMOR; }
                else if (slotTypeStr == "legs") { it.slotPosition |= SLOTP_LEGS; it.equipSlot = SLOTP_LEGS; }
                else if (slotTypeStr == "feet") { it.slotPosition |= SLOTP_FEET; it.equipSlot = SLOTP_FEET; }
                else if (slotTypeStr == "backpack") { it.slotPosition |= SLOTP_BACKPACK; it.equipSlot = SLOTP_BACKPACK; }
                else if (slotTypeStr == "two-handed") { it.slotPosition |= SLOTP_TWO_HAND; it.equipSlot = SLOTP_TWO_HAND; }
                else if (slotTypeStr == "right-hand") { it.slotPosition &= ~SLOTP_LEFT; it.equipSlot = SLOTP_LEFT; }
                else if (slotTypeStr == "left-hand") { it.slotPosition &= ~SLOTP_RIGHT; it.equipSlot = SLOTP_RIGHT; }
                else if (slotTypeStr == "necklace") { it.slotPosition |= SLOTP_NECKLACE; it.equipSlot = SLOTP_NECKLACE; }
                else if (slotTypeStr == "ring") { it.slotPosition |= SLOTP_RING; it.equipSlot = SLOTP_RING; }
                else if (slotTypeStr == "ammo") { it.slotPosition |= SLOTP_AMMO; it.equipSlot = SLOTP_AMMO; }
                else if (slotTypeStr == "hand") { it.slotPosition |= SLOTP_HAND; it.equipSlot = SLOTP_HAND; }
            }
            break;

        case ITEM_PARSE_AMMOTYPE:
            if (value.is_string()) it.ammoType = getAmmoType(asLowerCaseString(value.as_string()->get()));
            break;

        case ITEM_PARSE_SHOOTTYPE:
            if (value.is_string()) it.shootType = getShootType(asLowerCaseString(value.as_string()->get()));
            break;

        case ITEM_PARSE_EFFECT:
            if (value.is_string()) it.magicEffect = getMagicEffect(asLowerCaseString(value.as_string()->get()));
            break;

        case ITEM_PARSE_RANGE:
            if (value.is_integer()) it.shootRange = static_cast<uint16_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_STOPDURATION:
            if (value.is_boolean()) it.stopTime = value.as_boolean()->get();
            break;

        case ITEM_PARSE_DECAYTO:
            if (value.is_integer()) it.decayTo = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_TRANSFORMEQUIPTO:
            if (value.is_integer()) it.transformEquipTo = static_cast<uint16_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_TRANSFORMDEEQUIPTO:
            if (value.is_integer()) it.transformDeEquipTo = static_cast<uint16_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_DURATION:
            if (value.is_integer()) it.decayTime = static_cast<uint32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_SHOWDURATION:
            if (value.is_boolean()) it.showDuration = value.as_boolean()->get();
            break;

        case ITEM_PARSE_CHARGES:
            if (value.is_integer()) it.charges = static_cast<uint32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_SHOWCHARGES:
            if (value.is_boolean()) it.showCharges = value.as_boolean()->get();
            break;

        case ITEM_PARSE_SHOWATTRIBUTES:
            if (value.is_boolean()) it.showAttributes = value.as_boolean()->get();
            break;

        case ITEM_PARSE_HITCHANCE:
            if (value.is_integer()) {
                int16_t val = static_cast<int16_t>(value.as_integer()->get());
                it.hitChance = (val < -100) ? -100 : (val > 100) ? 100 : val;
            }
            break;

        case ITEM_PARSE_MAXHITCHANCE:
            if (value.is_integer()) it.maxHitChance = std::min<uint32_t>(100, static_cast<uint32_t>(value.as_integer()->get()));
            break;

        case ITEM_PARSE_INVISIBLE:
            if (value.is_boolean()) abilities.invisible = value.as_boolean()->get();
            break;

        case ITEM_PARSE_SPEED:
            if (value.is_integer()) abilities.speed = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_HEALTHGAIN:
            if (value.is_integer()) {
                abilities.regeneration = true;
                abilities.healthGain = static_cast<uint32_t>(value.as_integer()->get());
            }
            break;

        case ITEM_PARSE_HEALTHTICKS:
            if (value.is_integer()) {
                abilities.regeneration = true;
                abilities.healthTicks = static_cast<uint32_t>(value.as_integer()->get());
            }
            break;

        case ITEM_PARSE_MANAGAIN:
            if (value.is_integer()) {
                abilities.regeneration = true;
                abilities.manaGain = static_cast<uint32_t>(value.as_integer()->get());
            }
            break;

        case ITEM_PARSE_MANATICKS:
            if (value.is_integer()) {
                abilities.regeneration = true;
                abilities.manaTicks = static_cast<uint32_t>(value.as_integer()->get());
            }
            break;

        case ITEM_PARSE_MANASHIELD:
            if (value.is_boolean()) abilities.manaShield = value.as_boolean()->get();
            break;

        case ITEM_PARSE_SKILLSWORD:
            if (value.is_integer()) abilities.skills[SKILL_SWORD] = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_SKILLAXE:
            if (value.is_integer()) abilities.skills[SKILL_AXE] = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_SKILLCLUB:
            if (value.is_integer()) abilities.skills[SKILL_CLUB] = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_SKILLDIST:
            if (value.is_integer()) abilities.skills[SKILL_DISTANCE] = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_SKILLFISH:
            if (value.is_integer()) abilities.skills[SKILL_FISHING] = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_SKILLSHIELD:
            if (value.is_integer()) abilities.skills[SKILL_SHIELD] = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_SKILLFIST:
            if (value.is_integer()) abilities.skills[SKILL_FIST] = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_CRITICALHITAMOUNT:
            if (value.is_integer()) abilities.specialSkills[SPECIALSKILL_CRITICALHITAMOUNT] = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_CRITICALHITCHANCE:
            if (value.is_integer()) abilities.specialSkills[SPECIALSKILL_CRITICALHITCHANCE] = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_MANALEECHAMOUNT:
            if (value.is_integer()) abilities.specialSkills[SPECIALSKILL_MANALEECHAMOUNT] = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_MANALEECHCHANCE:
            if (value.is_integer()) abilities.specialSkills[SPECIALSKILL_MANALEECHCHANCE] = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_LIFELEECHAMOUNT:
            if (value.is_integer()) abilities.specialSkills[SPECIALSKILL_LIFELEECHAMOUNT] = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_LIFELEECHCHANCE:
            if (value.is_integer()) abilities.specialSkills[SPECIALSKILL_LIFELEECHCHANCE] = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_MAXHITPOINTS:
            if (value.is_integer()) abilities.stats[STAT_MAXHITPOINTS] = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_MAXHITPOINTSPERCENT:
            if (value.is_integer()) abilities.statsPercent[STAT_MAXHITPOINTS] = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_MAXMANAPOINTS:
            if (value.is_integer()) abilities.stats[STAT_MAXMANAPOINTS] = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_MAXMANAPOINTSPERCENT:
            if (value.is_integer()) abilities.statsPercent[STAT_MAXMANAPOINTS] = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_MAGICPOINTS:
            if (value.is_integer()) abilities.stats[STAT_MAGICPOINTS] = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_MAGICPOINTSPERCENT:
            if (value.is_integer()) abilities.statsPercent[STAT_MAGICPOINTS] = static_cast<int32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_SUPPRESSDRUNK:
            if (value.is_boolean() && value.as_boolean()->get()) abilities.conditionSuppressions |= CONDITION_DRUNK;
            break;

        case ITEM_PARSE_SUPPRESSENERGY:
            if (value.is_boolean() && value.as_boolean()->get()) abilities.conditionSuppressions |= CONDITION_ENERGY;
            break;

        case ITEM_PARSE_SUPPRESSFIRE:
            if (value.is_boolean() && value.as_boolean()->get()) abilities.conditionSuppressions |= CONDITION_FIRE;
            break;

        case ITEM_PARSE_SUPPRESSPOISON:
            if (value.is_boolean() && value.as_boolean()->get()) abilities.conditionSuppressions |= CONDITION_POISON;
            break;

        case ITEM_PARSE_SUPPRESSDROWN:
            if (value.is_boolean() && value.as_boolean()->get()) abilities.conditionSuppressions |= CONDITION_DROWN;
            break;

        case ITEM_PARSE_SUPPRESSPHYSICAL:
            if (value.is_boolean() && value.as_boolean()->get()) abilities.conditionSuppressions |= CONDITION_BLEEDING;
            break;

        case ITEM_PARSE_SUPPRESSFREEZE:
            if (value.is_boolean() && value.as_boolean()->get()) abilities.conditionSuppressions |= CONDITION_FREEZING;
            break;

        case ITEM_PARSE_SUPPRESSDAZZLE:
            if (value.is_boolean() && value.as_boolean()->get()) abilities.conditionSuppressions |= CONDITION_DAZZLED;
            break;

        case ITEM_PARSE_SUPPRESSCURSE:
            if (value.is_boolean() && value.as_boolean()->get()) abilities.conditionSuppressions |= CONDITION_CURSED;
            break;

        case ITEM_PARSE_FIELD:
            if (value.is_table()) {
                const toml::table& fieldTable = *value.as_table();
                it.group = ITEM_GROUP_MAGICFIELD;
                it.type = ITEM_TYPE_MAGICFIELD;

                std::string fieldType = fieldTable["type"] ? asLowerCaseString(fieldTable["type"].as_string()->get()) : "";
                CombatType_t combatType = COMBAT_NONE;
                std::unique_ptr<ConditionDamage> conditionDamage;

                if (fieldType == "fire") { combatType = COMBAT_FIREDAMAGE; conditionDamage = std::make_unique<ConditionDamage>(CONDITIONID_COMBAT, CONDITION_FIRE); }
                else if (fieldType == "energy") { combatType = COMBAT_ENERGYDAMAGE; conditionDamage = std::make_unique<ConditionDamage>(CONDITIONID_COMBAT, CONDITION_ENERGY); }
                else if (fieldType == "poison") { combatType = COMBAT_EARTHDAMAGE; conditionDamage = std::make_unique<ConditionDamage>(CONDITIONID_COMBAT, CONDITION_POISON); }
                else if (fieldType == "drown") { combatType = COMBAT_DROWNDAMAGE; conditionDamage = std::make_unique<ConditionDamage>(CONDITIONID_COMBAT, CONDITION_DROWN); }
                else if (fieldType == "physical") { combatType = COMBAT_PHYSICALDAMAGE; conditionDamage = std::make_unique<ConditionDamage>(CONDITIONID_COMBAT, CONDITION_BLEEDING); }

                if (combatType != COMBAT_NONE) {
                    it.combatType = combatType;
                    it.conditionDamage = std::move(conditionDamage);

                    uint32_t ticks = fieldTable["ticks"] ? static_cast<uint32_t>(fieldTable["ticks"].as_integer()->get()) : 0;
                    int32_t start = fieldTable["start"] ? std::max<int32_t>(0, static_cast<int32_t>(fieldTable["start"].as_integer()->get())) : 0;
                    int32_t count = fieldTable["count"] ? std::max<int32_t>(1, static_cast<int32_t>(fieldTable["count"].as_integer()->get())) : 1;
                    int32_t initDamage = fieldTable["initdamage"] ? static_cast<int32_t>(fieldTable["initdamage"].as_integer()->get()) : -1;
                    int32_t damage = fieldTable["damage"] ? -static_cast<int32_t>(fieldTable["damage"].as_integer()->get()) : 0;

                    if (damage != 0) {
                        if (start > 0) {
                            std::list<int32_t> damageList;
                            ConditionDamage::generateDamageList(damage, start, damageList);
                            for (int32_t damageValue : damageList) {
                                it.conditionDamage->addDamage(1, ticks, -damageValue);
                            }
                        }
                        else {
                            it.conditionDamage->addDamage(count, ticks, damage);
                        }
                    }

                    if (initDamage > 0 || initDamage < -1) {
                        it.conditionDamage->setInitDamage(-initDamage);
                    }
                    else if (initDamage == -1 && start != 0) {
                        it.conditionDamage->setInitDamage(start);
                    }

                    it.conditionDamage->setParam(CONDITION_PARAM_FIELD, 1);
                    if (it.conditionDamage->getTotalDamage() > 0) {
                        it.conditionDamage->setParam(CONDITION_PARAM_FORCEUPDATE, 1);
                    }
                }
            }
            break;

        case ITEM_PARSE_REPLACEABLE:
            if (value.is_boolean()) it.replaceable = value.as_boolean()->get();
            break;

        case ITEM_PARSE_PARTNERDIRECTION:
            if (value.is_string()) it.bedPartnerDir = getDirection(value.as_string()->get());
            break;

        case ITEM_PARSE_LEVELDOOR:
            if (value.is_integer()) it.levelDoor = static_cast<uint32_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_MALETRANSFORMTO:
        case ITEM_PARSE_FEMALETRANSFORMTO:
            if (value.is_integer()) {
                uint16_t transformId = static_cast<uint16_t>(value.as_integer()->get());
                PlayerSex_t sex = (parseAttribute->second == ITEM_PARSE_MALETRANSFORMTO) ? PLAYERSEX_MALE : PLAYERSEX_FEMALE;
                it.transformToOnUse[sex] = transformId;
                ItemType& other = getItemType(transformId);
                if (other.transformToFree == 0) {
                    other.transformToFree = it.id;
                }
                if (it.transformToOnUse[sex == PLAYERSEX_MALE ? PLAYERSEX_FEMALE : PLAYERSEX_MALE] == 0) {
                    it.transformToOnUse[sex == PLAYERSEX_MALE ? PLAYERSEX_FEMALE : PLAYERSEX_MALE] = transformId;
                }
            }
            break;

        case ITEM_PARSE_TRANSFORMTO:
            if (value.is_integer()) it.transformToFree = static_cast<uint16_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_DESTROYTO:
            if (value.is_integer()) it.destroyTo = static_cast<uint16_t>(value.as_integer()->get());
            break;

        case ITEM_PARSE_ELEMENTICE:
            if (value.is_integer()) { abilities.elementDamage = static_cast<uint16_t>(value.as_integer()->get()); abilities.elementType = COMBAT_ICEDAMAGE; }
            break;

        case ITEM_PARSE_ELEMENTEARTH:
            if (value.is_integer()) { abilities.elementDamage = static_cast<uint16_t>(value.as_integer()->get()); abilities.elementType = COMBAT_EARTHDAMAGE; }
            break;

        case ITEM_PARSE_ELEMENTFIRE:
            if (value.is_integer()) { abilities.elementDamage = static_cast<uint16_t>(value.as_integer()->get()); abilities.elementType = COMBAT_FIREDAMAGE; }
            break;

        case ITEM_PARSE_ELEMENTENERGY:
            if (value.is_integer()) { abilities.elementDamage = static_cast<uint16_t>(value.as_integer()->get()); abilities.elementType = COMBAT_ENERGYDAMAGE; }
            break;

        case ITEM_PARSE_ELEMENTDEATH:
            if (value.is_integer()) { abilities.elementDamage = static_cast<uint16_t>(value.as_integer()->get()); abilities.elementType = COMBAT_DEATHDAMAGE; }
            break;

        case ITEM_PARSE_ELEMENTHOLY:
            if (value.is_integer()) { abilities.elementDamage = static_cast<uint16_t>(value.as_integer()->get()); abilities.elementType = COMBAT_HOLYDAMAGE; }
            break;

        case ITEM_PARSE_WALKSTACK:
            if (value.is_boolean()) it.walkStack = value.as_boolean()->get();
            break;

        case ITEM_PARSE_BLOCKING:
            if (value.is_boolean()) it.blockSolid = value.as_boolean()->get();
            break;

        case ITEM_PARSE_ALLOWDISTREAD:
            if (value.is_string()) it.allowDistRead = booleanString(value.as_string()->get());
            else if (value.is_boolean()) it.allowDistRead = value.as_boolean()->get();
            break;

        case ITEM_PARSE_STOREITEM:
            if (value.is_string()) it.storeItem = booleanString(value.as_string()->get());
            else if (value.is_boolean()) it.storeItem = value.as_boolean()->get();
            break;

        case ITEM_PARSE_WORTH:
            if (value.is_integer()) {
                uint64_t worth = static_cast<uint64_t>(value.as_integer()->get());
                if (currencyItems.emplace(worth, id).second) {
                    it.worth = worth;
                }
                else {
                    std::cout << "[Warning - Items::parseItemToml] Duplicate currency worth " << worth << " for item " << id << std::endl;
                }
            }
            break;

        case ITEM_PARSE_AUGMENT:
            if (value.is_string()) {
                it.augments.emplace(value.as_string()->get());
            }
            else if (value.is_array()) {
                for (const auto& aug : *value.as_array()) {
                    if (aug.is_string()) it.augments.emplace(aug.as_string()->get());
                }
            }
            break;

        default:
            break;
        }
    }

    // Validate transform attributes for non-bed items
    if ((it.transformToFree || it.transformToOnUse[PLAYERSEX_FEMALE] || it.transformToOnUse[PLAYERSEX_MALE]) && it.type != ITEM_TYPE_BED) {
        std::cout << "[Warning - Items::parseItemToml] Item " << id << " has transform attributes but is not a bed" << std::endl;
    }
}

ItemType& Items::getItemType(size_t id)
{
	if (id < items.size()) {
		return items[id];
	}
	return items.front();
}

const ItemType& Items::getItemType(size_t id) const
{
	if (id < items.size()) {
		return items[id];
	}
	return items.front();
}

const ItemType& Items::getItemIdByClientId(uint16_t spriteId) const
{
	if (spriteId >= 100) {
		return getItemType(spriteId);
	}
	return items.front();
}

uint16_t Items::getItemIdByName(const std::string& name)
{
	auto result = nameToItems.find(asLowerCaseString(name));

	if (result == nameToItems.end())
		return 0;

	return result->second;
}

bool unserializeDatItem(ItemType& iType, std::ifstream& fin)
{
	// read the options until we find the termination flag
	ItemDatFlag flag;
	do
	{
		flag = (ItemDatFlag)fin.get();

		switch (flag)
		{
			case ItemDatFlag.Ground:
				iType.group = ITEM_GROUP_GROUND;
				uint16_t groundSpeed;
                fin.read(reinterpret_cast<char*>(&groundSpeed), sizeof(groundSpeed));
				iType.speed = groundSpeed;
				break;

			case ItemDatFlag.GroundBorder:
				iType.alwaysOnTopOrder = 1;
				break;

			case ItemDatFlag.OnBottom:
				iType.alwaysOnTopOrder = 2;
				break;

			case ItemDatFlag.OnTop:
				iiType.alwaysOnTopOrder = 3;
				break;

			case ItemDatFlag.Container:
				iType.type = ITEM_TYPE_CONTAINER;
				break;

			case ItemDatFlag.Stackable:
				iType.stackable = true;
				break;

			case ItemDatFlag.ForceUse:
				iType.forceUse = true;
				break;

			case ItemDatFlag.MultiUse:
				iType.useable = true;
				break;

			case ItemDatFlag.Writable:
				iType.canWriteText = true;
				iType.canReadText = true;
				uint16_t maxTextLen;
                fin.read(reinterpret_cast<char*>(&maxTextLen), sizeof(maxTextLen));
				iType.maxTextLen = maxTextLen;
				break;

			case ItemDatFlag.WritableOnce:
				iType.canReadText = true;
				// to-do - confirm if correct
				uint16_t maxTextLen;
                fin.read(reinterpret_cast<char*>(&maxTextLen), sizeof(maxTextLen));
				iType.maxTextLen = maxTextLen;
				break;

			case ItemDatFlag.FluidContainer:
				// to-do - confirm if correct
				iType.group = ITEM_GROUP_FLUID;
				break;

			case ItemDatFlag.Fluid:
				// to-do - confirm if correct
				iType.group = ITEM_GROUP_FLUID;
				break;

			case ItemDatFlag.IsUnpassable:
				// to-do - verify if correct - blockSolid is 90% unpassable
				iType.blockSolid   = true;
				break;

			case ItemDatFlag.IsUnmoveable:
				iType.moveable = false;
				break;

			case ItemDatFlag.BlockMissiles:
				iType.blockProjectile = true;
				break;

			case ItemDatFlag.BlockPathfinder:
				iType.blockPathFind = true;
				break;

			case ItemDatFlag.NoMoveAnimation:
				iType.isAnimation = true;
				break;

			case ItemDatFlag.Pickupable:
				iType.pickupable = true;
				break;

			case ItemDatFlag.Hangable:
				iType.isHangable = true;
				break;

			case ItemDatFlag.IsHorizontal:
				iType.isHorizontal = true;
				break;

			case ItemDatFlag.IsVertical:
				iType.isVertical = true;
				break;

			case ItemDatFlag.Rotatable:
				iType.rotatable = true;
				break;

			case ItemDatFlag.HasLight:
				uint16_t lightLevel;
				uint16_t lightColor;
                fin.read(reinterpret_cast<char*>(&lightLevel), sizeof(lightLevel));
                fin.read(reinterpret_cast<char*>(&lightColor), sizeof(lightColor));
				iType.lightLevel = lightLevel;
				iType.lightColor = lightColor;
				break;

			case ItemDatFlag.DontHide:
				break;

			case ItemDatFlag.Translucent:
				break;

			case ItemDatFlag.HasOffset:
				fin.seekg(2, std::ios::cur); // OffsetX
				fin.seekg(2, std::ios::cur); // OffsetY
				break;

			case ItemDatFlag.HasElevation:
				//item.HasElevation = true;
				fin.seekg(2, std::ios::cur); // Height
				break;

			case ItemDatFlag.Lying:
				break;

			case ItemDatFlag.AnimateAlways:
				break;

			case ItemDatFlag.Minimap:
				fin.seekg(2, std::ios::cur); // minimap color
				break;

			case ItemDatFlag.LensHelp:
				uint16_t lensHelp;
                fin.read(reinterpret_cast<char*>(&lensHelp), sizeof(lensHelp));
				if (lensHelp == 1112)
				{
					item.canReadText = true;
				}
				break;

			case ItemDatFlag.FullGround:
				//item.FullGround = true;
				break;

			case ItemDatFlag.IgnoreLook:
				// to-do confirm if should be false/true
				iType.lookThrough = false;
				break;

			case ItemDatFlag.Cloth:
				fin.seekg(2, std::ios::cur); // cloth
				break;

			case ItemDatFlag.Market:
				fin.seekg(2, std::ios::cur); // category
				fin.seekg(2, std::ios::cur); // trade as
				fin.seekg(2, std::ios::cur); // show as
				uint16_t nameLength;
                fin.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
				fin.seekg(nameLength, std::ios::cur); // name
				// Skip profession (2 bytes) and level (2 bytes)
        		fin.seekg(4, std::ios::cur);
				break;

			case ItemDatFlag.DefaultAction:
				fin.seekg(2, std::ios::cur);
				break;

			case ItemDatFlag.Wrappable:
			case ItemDatFlag.Unwrappable:
			case ItemDatFlag.TopEffect:
			case ItemDatFlag.Usable:
				break;

			case ItemDatFlag.LastFlag:
				break;

			default: {
				fmt::print(
					fmt::fg(fmt::color::crimson) | fmt::emphasis::bold,
					"UnserializeDatItem: Error while parsing, unknown flag 0x{:X} at id {}.\n",
					flag, iType.id
				);
				return false;
			}
		}

	} while (flag != ItemDatFlag.LastFlag);

    return true;
}