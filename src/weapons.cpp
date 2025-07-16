// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "combat.h"
#include "configmanager.h"
#include "game.h"
#include "pugicast.h"
#include "weapons.h"
#include "luavariant.h"
#include "events.h"

extern Game g_game;
extern Vocations g_vocations;
extern ConfigManager g_config;
extern Weapons* g_weapons;
extern Events* g_events;

namespace StandardHitChance
{

	constexpr uint32_t MAX_SKILL = 110;
	constexpr uint32_t DISTANCES = 8;
	constexpr uint32_t MAX_MODELS = 3;

	enum HitChanceModel : uint8_t
	{
		MODEL_75 = 0,
		MODEL_90 = 1,
		MODEL_100 = 2,
	};

	static constexpr std::array<std::pair<int32_t, int32_t>, 9> _DirectionOffsets{ {
		{-1, -1}, {0, -1}, {1, -1},
		{-1,  0}, {0,  0}, {1,  0},
		{-1,  1}, {0,  1}, {1,  1}
	} };

	constexpr std::array<std::array<uint32_t, MAX_SKILL + 1>, DISTANCES> _Model75()
	{
		std::array<std::array<uint32_t, MAX_SKILL + 1>, DISTANCES> model{};

		for (uint32_t d = 0; d < DISTANCES; ++d)
		{
			for (uint32_t skill = 0; skill <= MAX_SKILL; ++skill)
			{
				uint32_t chance = 0;
				switch (d) 
				{
					case 1:
					case 5:
						chance = std::min<uint32_t>(skill, 74) + 1;
						break;
					case 2:
						chance = static_cast<uint32_t>(std::min<uint32_t>(skill, 28) * 2.4f + 8);
						break;
					case 3:
						chance = static_cast<uint32_t>(std::min<uint32_t>(skill, 45) * 1.55f + 6);
						break;
					case 4:
						chance = static_cast<uint32_t>(std::min<uint32_t>(skill, 58) * 1.25f + 3);
						break;
					case 6:
						chance = static_cast<uint32_t>(std::min<uint32_t>(skill, 90) * 0.80f + 3);
						break;
					case 7:
						chance = static_cast<uint32_t>(std::min<uint32_t>(skill, 104) * 0.70f + 2);
						break;
					[[unlikely]]
					default:
						break;
				}
				model[d][skill] = chance;
			}
		}
		return model;
	}

	constexpr std::array<std::array<uint32_t, MAX_SKILL + 1>, DISTANCES> _Model90()
	{
		std::array<std::array<uint32_t, MAX_SKILL + 1>, DISTANCES> model{};

		for (uint32_t d = 0; d < DISTANCES; ++d)
		{
			for (uint32_t skill = 0; skill <= MAX_SKILL; ++skill)
			{
				uint32_t chance = 0;
				switch (d) 
				{
					case 1:
					case 5:
						chance = static_cast<uint32_t>(std::min<uint32_t>(skill, 74) * 1.20f + 1);
						break;
					case 2:
						chance = static_cast<uint32_t>(std::min<uint32_t>(skill, 28) * 3.20f);
						break;
					case 3:
						chance = static_cast<uint32_t>(std::min<uint32_t>(skill, 45) * 2);
						break;
					case 4:
						chance = static_cast<uint32_t>(std::min<uint32_t>(skill, 58) * 1.55f);
						break;
					case 6:
					case 7:
						chance = std::min<uint32_t>(skill, 90);
						break;
					[[unlikely]]
					default:
						break;
					}
				model[d][skill] = chance;
			}
		}
		return model;
	}

	constexpr std::array<std::array<uint32_t, MAX_SKILL + 1>, DISTANCES> _Model100()
	{
		std::array<std::array<uint32_t, MAX_SKILL + 1>, DISTANCES> model{};

		for (uint32_t d = 0; d < DISTANCES; ++d)
		{
			for (uint32_t skill = 0; skill <= MAX_SKILL; ++skill)
			{
				uint32_t chance = 0;
				switch (d) 
				{
					case 1:
					case 5:
						chance = static_cast<uint32_t>(std::min<uint32_t>(skill, 73) * 1.35f + 1);
						break;
					case 2:
						chance = static_cast<uint32_t>(std::min<uint32_t>(skill, 30) * 3.20f + 4);
						break;
					case 3:
						chance = static_cast<uint32_t>(std::min<uint32_t>(skill, 48) * 2.05f + 2);
						break;
					case 4:
						chance = static_cast<uint32_t>(std::min<uint32_t>(skill, 65) * 1.50f + 2);
						break;
					case 6:
						chance = static_cast<uint32_t>(std::min<uint32_t>(skill, 87) * 1.20f) - 4;
						break;
					case 7:
						chance = static_cast<uint32_t>(std::min<uint32_t>(skill, 90) * 1.10f + 1);
						break;
					[[unlikely]]
					default:
						break;
				}
				model[d][skill] = chance;
			}
		}
		return model;
	}

	constexpr auto _75_Model = _Model75();
	constexpr auto _90_Model = _Model90();
	constexpr auto _100_Model = _Model100();

	// this is the only method in the namespace meant for outside use
	// everything else is intended for internal use only
	inline uint32_t getChance(HitChanceModel model, uint32_t distance, uint32_t skill)
	{
		distance = std::clamp(distance, 0u, DISTANCES - 1);
		skill = std::min(skill, MAX_SKILL);

		switch (model) 
		{
			case MODEL_75: return _75_Model[distance][skill];
			case MODEL_90: return _90_Model[distance][skill];
			case MODEL_100: return _100_Model[distance][skill];
			[[unlikely]] default: return 0;
		}
	}
}

Weapons::Weapons()
{
	scriptInterface.initState();
}

Weapons::~Weapons()
{
	clear(false);
}

const Weapon* Weapons::getWeapon(const ItemConstPtr& item) const
{
	if (!item) {
		return nullptr;
	}

	const auto it = weapons.find(item->getID());
	if (it == weapons.end()) {
		return nullptr;
	}
	return it->second;
}

void Weapons::clear(bool fromLua)
{
	for (auto it = weapons.begin(); it != weapons.end(); ) {
		if (fromLua == it->second->fromLua) {
			it = weapons.erase(it);
		} else {
			++it;
		}
	}

	reInitState(fromLua);
}

LuaScriptInterface& Weapons::getScriptInterface()
{
	return scriptInterface;
}

void Weapons::loadDefaults()
{
	for (size_t i = 100, size = Item::items.size(); i < size; ++i) {
		const ItemType& it = Item::items.getItemType(i);
		if (it.id == 0 || weapons.contains(i)) {
			continue;
		}

		switch (it.weaponType) {
			case WEAPON_AXE:
			case WEAPON_SWORD:
			case WEAPON_CLUB: {
				WeaponMelee* weapon = new WeaponMelee(&scriptInterface);
				weapon->configureWeapon(it);
				weapons[i] = weapon;
				break;
			}

			case WEAPON_AMMO:
			case WEAPON_DISTANCE: {
				if (it.weaponType == WEAPON_DISTANCE && it.ammoType != AMMO_NONE) {
					continue;
				}

				WeaponDistance* weapon = new WeaponDistance(&scriptInterface);
				weapon->configureWeapon(it);
				weapons[i] = weapon;
				break;
			}

			default:
				break;
		}
	}
}

Event_ptr Weapons::getEvent(const std::string& nodeName)
{
	if (caseInsensitiveEqual(nodeName, "melee")) {
		return Event_ptr(new WeaponMelee(&scriptInterface));
	} else if (caseInsensitiveEqual(nodeName, "distance")) {
		return Event_ptr(new WeaponDistance(&scriptInterface));
	} else if (caseInsensitiveEqual(nodeName, "wand")) {
		return Event_ptr(new WeaponWand(&scriptInterface));
	}
	return nullptr;
}

bool Weapons::registerEvent(Event_ptr event, const pugi::xml_node&)
{
	Weapon* weapon = static_cast<Weapon*>(event.release()); //event is guaranteed to be a Weapon

	auto result = weapons.emplace(weapon->getID(), weapon);
	if (!result.second) {
		std::cout << "[Warning - Weapons::registerEvent] Duplicate registered item with id: " << weapon->getID() << std::endl;
	}
	return result.second;
}

bool Weapons::registerLuaEvent(Weapon* weapon)
{
	weapons[weapon->getID()] = weapon;
	return true;
}

//monsters
int32_t Weapons::getMaxMeleeDamage(int32_t attackSkill, int32_t attackValue)
{
	return static_cast<int32_t>(std::ceil((attackSkill * (attackValue * 0.05)) + (attackValue * 0.5)));
}

//players
int32_t Weapons::getMaxWeaponDamage(uint32_t level, int32_t attackSkill, int32_t attackValue, float attackFactor)
{
	return static_cast<int32_t>(std::round((level / 5) + (((((attackSkill / 4.) + 1) * (attackValue / 3.)) * 1.03) / attackFactor)));
}

bool Weapon::configureEvent(const pugi::xml_node& node)
{
	pugi::xml_attribute attr;
	if (!(attr = node.attribute("id"))) {
		std::cout << "[Error - Weapon::configureEvent] Weapon without id." << std::endl;
		return false;
	}
	id = pugi::cast<uint16_t>(attr.value());

	if ((attr = node.attribute("level"))) {
		level = pugi::cast<uint32_t>(attr.value());
	}

	if ((attr = node.attribute("maglv")) || (attr = node.attribute("maglevel"))) {
		magLevel = pugi::cast<uint32_t>(attr.value());
	}

	if ((attr = node.attribute("mana"))) {
		mana = pugi::cast<uint32_t>(attr.value());
	}

	if ((attr = node.attribute("manapercent"))) {
		manaPercent = pugi::cast<uint32_t>(attr.value());
	}

	if ((attr = node.attribute("soul"))) {
		soul = pugi::cast<uint32_t>(attr.value());
	}

	if ((attr = node.attribute("prem"))) {
		premium = attr.as_bool();
	}

	if ((attr = node.attribute("breakchance"))) {
		breakChance = std::min<uint8_t>(100, pugi::cast<uint16_t>(attr.value()));
	}

	if ((attr = node.attribute("action"))) {
		action = getWeaponAction(asLowerCaseString(attr.as_string()));
		if (action == WEAPONACTION_NONE) {
			std::cout << "[Warning - Weapon::configureEvent] Unknown action " << attr.as_string() << std::endl;
		}
	}

	if ((attr = node.attribute("enabled"))) {
		enabled = attr.as_bool();
	}

	if ((attr = node.attribute("unproperly"))) {
		wieldUnproperly = attr.as_bool();
	}

	std::list<std::string> vocStringList;
	for (auto vocationNode : node.children()) {
		if (!(attr = vocationNode.attribute("name"))) {
			continue;
		}

		int32_t vocationId = g_vocations.getVocationId(attr.as_string());
		if (vocationId != -1) {
			vocWeaponMap[vocationId] = true;
			int32_t promotedVocation = g_vocations.getPromotedVocation(vocationId);
			if (promotedVocation != VOCATION_NONE) {
				vocWeaponMap[promotedVocation] = true;
			}

			if (vocationNode.attribute("showInDescription").as_bool(true)) {
				vocStringList.push_back(asLowerCaseString(attr.as_string()));
			}
		}
	}

	std::string vocationString;
	for (const std::string& str : vocStringList) {
		if (!vocationString.empty()) {
			if (str != vocStringList.back()) {
				vocationString.push_back(',');
				vocationString.push_back(' ');
			} else {
				vocationString += " and ";
			}
		}

		vocationString += str;
		vocationString.push_back('s');
	}

	uint32_t wieldInfo = 0;
	if (getReqLevel() > 0) {
		wieldInfo |= WIELDINFO_LEVEL;
	}

	if (getReqMagLv() > 0) {
		wieldInfo |= WIELDINFO_MAGLV;
	}

	if (!vocationString.empty()) {
		wieldInfo |= WIELDINFO_VOCREQ;
	}

	if (isPremium()) {
		wieldInfo |= WIELDINFO_PREMIUM;
	}

	if (wieldInfo != 0) {
		ItemType& it = Item::items.getItemType(id);
		it.wieldInfo = wieldInfo;
		it.vocationString = vocationString;
		it.minReqLevel = getReqLevel();
		it.minReqMagicLevel = getReqMagLv();
	}

	configureWeapon(Item::items[id]);
	return true;
}

void Weapon::configureWeapon(const ItemType& it)
{
	id = it.id;
}

int32_t Weapon::playerWeaponCheck(const PlayerConstPtr& player, const CreatureConstPtr& target, uint8_t shootRange) const
{
	const Position& playerPos = player->getPosition();
	const Position& targetPos = target->getPosition();
	if (playerPos.z != targetPos.z) {
		return 0;
	}

	if (std::max<uint32_t>(Position::getDistanceX(playerPos, targetPos), Position::getDistanceY(playerPos, targetPos)) > shootRange) {
		return 0;
	}

	if (!player->hasFlag(PlayerFlag_IgnoreWeaponCheck)) {
		if (!enabled) {
			return 0;
		}

		if (player->getMana() < getManaCost(player)) {
			return 0;
		}

		if (player->getHealth() < getHealthCost(player)) {
			return 0;
		}

		if (player->getSoul() < soul) {
			return 0;
		}

		if (isPremium() && !player->isPremium()) {
			return 0;
		}

		if (!vocWeaponMap.empty()) {
			if (!vocWeaponMap.contains(player->getVocationId())) {
				return 0;
			}
		}

		int32_t damageModifier = 100;
		if (player->getLevel() < getReqLevel()) {
			damageModifier = (isWieldedUnproperly() ? damageModifier / 2 : 0);
		}

		if (player->getMagicLevel() < getReqMagLv()) {
			damageModifier = (isWieldedUnproperly() ? damageModifier / 2 : 0);
		}
		return damageModifier;
	}

	return 100;
}

bool Weapon::ammoCheck(const PlayerConstPtr& player) const
{
	if (!player->hasFlag(PlayerFlag_IgnoreWeaponCheck)) {
		if (!enabled || player->getMana() < getManaCost(player) || player->getHealth() < getHealthCost(player) ||
			(isPremium() && !player->isPremium()) || player->getLevel() < getReqLevel() ||
			player->getMagicLevel() < getReqMagLv() || player->getSoul() < soul) {
			return false;
		}

		if (!vocWeaponMap.empty()) {
			if (!vocWeaponMap.contains(player->getVocationId())) {
				return false;
			}
		}
	}

	return true;
}

bool Weapon::useWeapon(const PlayerPtr player, const ItemPtr item, const CreaturePtr target) const
{
	const auto damageModifier = playerWeaponCheck(player, target, item->getShootRange());
	if (damageModifier == 0) {
		return false;
	}

	internalUseWeapon(player, item, target, damageModifier);
	return true;
}

bool Weapon::useFist(const PlayerPtr& player, const CreaturePtr& target)
{
	if (!Position::areInRange<1, 1>(player->getPosition(), target->getPosition())) {
		return false;
	}

	float attackFactor = player->getAttackFactor();
	int32_t attackSkill = player->getSkillLevel(SKILL_FIST);
	int32_t attackValue = 7;

	int32_t maxDamage = Weapons::getMaxWeaponDamage(player->getLevel(), attackSkill, attackValue, attackFactor);

	CombatParams params;
	params.combatType = COMBAT_PHYSICALDAMAGE;
	params.blockedByArmor = true;
	params.blockedByShield = true;

	CombatDamage damage;
	damage.origin = ORIGIN_MELEE;
	damage.primary.type = params.combatType;
	damage.primary.value = -normal_random(0, maxDamage);

	Combat::doTargetCombat(player, target, damage, params);
	if (!player->hasFlag(PlayerFlag_NotGainSkill) && player->getAddAttackSkill()) {
		player->addSkillAdvance(SKILL_FIST, 1);
	}

	return true;
}

void Weapon::internalUseWeapon(const PlayerPtr& player, const ItemPtr& item, const CreaturePtr& target, int32_t damageModifier) const
{
	if (scripted) {
		LuaVariant var;
		var.setNumber(target->getID());
		executeUseWeapon(player, var);
	} else {
		CombatDamage damage;
		WeaponType_t weaponType = item->getWeaponType();
		if (weaponType == WEAPON_AMMO || weaponType == WEAPON_DISTANCE) {
			damage.origin = ORIGIN_RANGED;
		} else {
			damage.origin = ORIGIN_MELEE;
		}
		damage.primary.type = params.combatType;
		damage.primary.value = (getWeaponDamage(player, target, item) * damageModifier) / 100;
		damage.secondary.type = getElementType();
		damage.secondary.value = getElementDamage(player, target, item);
		Combat::doTargetCombat(player, target, damage, params);
	}

	onUsedWeapon(player, item, target->getTile());
}

void Weapon::internalUseWeapon(const PlayerPtr& player, const ItemPtr& item, const TilePtr& tile) const
{
	if (scripted) {
		LuaVariant var;
		var.setTargetPosition(tile->getPosition());
		executeUseWeapon(player, var);
	} else {
		Combat::postCombatEffects(player, tile->getPosition(), params);
		g_game.addMagicEffect(tile->getPosition(), CONST_ME_POFF);
	}

	onUsedWeapon(player, item, tile);
}

void Weapon::onUsedWeapon(const PlayerPtr& player, const ItemPtr& item, const TilePtr& destTile) const
{
	if (!player->hasFlag(PlayerFlag_NotGainSkill)) {
		skills_t skillType;
		uint32_t skillPoint;
		if (getSkillType(player, item, skillType, skillPoint)) {
			player->addSkillAdvance(skillType, skillPoint);
		}
	}

	uint32_t manaCost = getManaCost(player);
	if (manaCost != 0) {
		player->addManaSpent(manaCost);
		player->changeMana(-static_cast<int32_t>(manaCost));
	}

	uint32_t healthCost = getHealthCost(player);
	if (healthCost != 0) {
		player->changeHealth(-static_cast<int32_t>(healthCost));
	}

	if (!player->hasFlag(PlayerFlag_HasInfiniteSoul) && soul > 0) {
		player->changeSoul(-static_cast<int32_t>(soul));
	}

	if (breakChance != 0 && uniform_random(1, 100) <= breakChance) {
		Weapon::decrementItemCount(item);
		return;
	}

	CylinderPtr f_holder = item->getParent();
	CylinderPtr t_holder = destTile->getTile();
	switch (action) {
		case WEAPONACTION_REMOVECOUNT:
			if (g_config.getBoolean(ConfigManager::REMOVE_WEAPON_AMMO)) {
				Weapon::decrementItemCount(item);
			}
			break;

		case WEAPONACTION_REMOVECHARGE: {
			uint16_t charges = item->getCharges();
			if (charges != 0 && g_config.getBoolean(ConfigManager::REMOVE_WEAPON_CHARGES)) {
				g_game.transformItem(item, item->getID(), charges - 1);
			}
			break;
		}

		case WEAPONACTION_MOVE:
			g_game.internalMoveItem(f_holder, t_holder, INDEX_WHEREEVER, item, 1, std::nullopt, FLAG_NOLIMIT);
			break;

		default:
			break;
	}
}

uint32_t Weapon::getManaCost(const PlayerConstPtr& player) const
{
	if (mana != 0) {
		return mana;
	}

	if (manaPercent == 0) {
		return 0;
	}

	return (player->getMaxMana() * manaPercent) / 100;
}

int32_t Weapon::getHealthCost(const PlayerConstPtr& player) const
{
	if (health != 0) {
		return health;
	}

	if (healthPercent == 0) {
		return 0;
	}

	return (player->getMaxHealth() * healthPercent) / 100;
}

bool Weapon::executeUseWeapon(const PlayerPtr& player, const LuaVariant& var) const
{
	//onUseWeapon(player, var)
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - Weapon::executeUseWeapon] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);
	LuaScriptInterface::pushSharedPtr(L, player);
	LuaScriptInterface::setMetatable(L, -1, "Player");
	scriptInterface->pushVariant(L, var);

	return scriptInterface->callFunction(2);
}

void Weapon::decrementItemCount(const ItemPtr& item)
{
	uint16_t count = item->getItemCount();
	if (count > 1) {
		g_game.transformItem(item, item->getID(), count - 1);
	} else {
		g_game.internalRemoveItem(item);
	}
}

WeaponMelee::WeaponMelee(LuaScriptInterface* interface) :
	Weapon(interface)
{
	params.blockedByArmor = true;
	params.blockedByShield = true;
	params.combatType = COMBAT_PHYSICALDAMAGE;
}

void WeaponMelee::configureWeapon(const ItemType& it)
{
	if (it.abilities) {
		elementType = it.abilities->elementType;
		elementDamage = it.abilities->elementDamage;
		params.aggressive = true;
		params.useCharges = true;
	} else {
		elementType = COMBAT_NONE;
		elementDamage = 0;
	}
	Weapon::configureWeapon(it);
}

bool WeaponMelee::useWeapon(const PlayerPtr player, const ItemPtr item, const CreaturePtr target) const
{
	int32_t damageModifier = playerWeaponCheck(player, target, item->getShootRange());
	if (damageModifier == 0) {
		return false;
	}
	internalUseWeapon(player, item, target, damageModifier);
	return true;
}

bool WeaponMelee::getSkillType(const PlayerConstPtr& player, const ItemConstPtr& item,
                               skills_t& skill, uint32_t& skillpoint) const
{
	if (player->getAddAttackSkill() && player->getLastAttackBlockType() != BLOCK_IMMUNITY) {
		skillpoint = 1;
	} else {
		skillpoint = 0;
	}

	switch (const auto weaponType = item->getWeaponType()) {
		case WEAPON_SWORD: {
			skill = SKILL_SWORD;
			return true;
		}

		case WEAPON_CLUB: {
			skill = SKILL_CLUB;
			return true;
		}

		case WEAPON_AXE: {
			skill = SKILL_AXE;
			return true;
		}

		default:
			break;
	}
	return false;
}

int32_t WeaponMelee::getElementDamage(const PlayerConstPtr& player, const CreatureConstPtr&, const ItemConstPtr& item) const
{
	if (elementType == COMBAT_NONE) {
		return 0;
	}

	int32_t attackSkill = player->getWeaponSkill(item);
	int32_t attackValue = elementDamage;
	float attackFactor = player->getAttackFactor();

	int32_t maxValue = Weapons::getMaxWeaponDamage(player->getLevel(), attackSkill, attackValue, attackFactor);
	return -normal_random(0, static_cast<int32_t>(maxValue * player->getVocation()->meleeDamageMultiplier));
}

int32_t WeaponMelee::getWeaponDamage(const PlayerConstPtr& player, const CreatureConstPtr&, const ItemConstPtr& item, bool maxDamage /*= false*/) const {

	const int32_t playerLevel = player->getLevel();
	const float playerAttackFactor = player->getAttackFactor();
	const auto& playerVocation = player->getVocation();
	int32_t attackSkill = player->getWeaponSkill(item);
	int32_t attackValue = std::max<int32_t>(0, item->getAttack());
	const float meleeDamageMultiplier = playerVocation->meleeDamageMultiplier;
	int32_t maxValue = static_cast<int32_t>(Weapons::getMaxWeaponDamage(playerLevel, attackSkill, attackValue, playerAttackFactor) * meleeDamageMultiplier);

	if (maxDamage) 
	{
		return -maxValue;
	}
	return -normal_random(0, maxValue);
}

WeaponDistance::WeaponDistance(LuaScriptInterface* interface) :
	Weapon(interface)
{
	params.blockedByArmor = true;
	params.combatType = COMBAT_PHYSICALDAMAGE;
}

void WeaponDistance::configureWeapon(const ItemType& it)
{
	params.distanceEffect = it.shootType;

	if (it.abilities) {
		elementType = it.abilities->elementType;
		elementDamage = it.abilities->elementDamage;
		params.aggressive = true;
		params.useCharges = true;
	} else {
		elementType = COMBAT_NONE;
		elementDamage = 0;
	}

	Weapon::configureWeapon(it);
}

bool WeaponDistance::useWeapon(const PlayerPtr player, const ItemPtr item, const CreaturePtr target) const
{
	if (not player or !item or not target) 
	{
		return false;
	}

	const ItemType& it = Item::items[id];
	const Position& playerPos = player->getPosition();
	const Position& targetPos = target->getPosition();

	int32_t damageModifier = 0;
	if (it.weaponType == WEAPON_AMMO) 
	{
		const ItemPtr& mainWeaponItem = player->getWeapon(true);
		if (mainWeaponItem) 
		{
			if (const auto mainWeapon = g_weapons->getWeapon(mainWeaponItem)) 
			{
				damageModifier = mainWeapon->playerWeaponCheck(player, target, mainWeaponItem->getShootRange());
			}
			else 
			{
				damageModifier = playerWeaponCheck(player, target, mainWeaponItem->getShootRange());
			}
		}
	}
	else 
	{
		damageModifier = playerWeaponCheck(player, target, item->getShootRange());
	}

	if (damageModifier == 0) 
	{
		return false;
	}

	uint32_t chance;
	if (it.hitChance == 0) 
	{
		const uint32_t skill = player->getSkillLevel(SKILL_DISTANCE);
		const uint32_t distance = std::min(7u, std::max<uint32_t>(
			Position::getDistanceX(playerPos, targetPos),
			Position::getDistanceY(playerPos, targetPos)));

		const uint32_t maxHitChance = (it.maxHitChance != -1) ? it.maxHitChance	: (it.ammoType != AMMO_NONE ? 90 : 75);

		switch (maxHitChance) 
		{
			case 75:
				chance = StandardHitChance::getChance(StandardHitChance::MODEL_75, distance, skill);
				break;
			case 90:
				chance = StandardHitChance::getChance(StandardHitChance::MODEL_90, distance, skill);
				break;
			case 100:
				chance = StandardHitChance::getChance(StandardHitChance::MODEL_100, distance, skill);
				break;
			default:
				chance = maxHitChance;
				break;
		}
	}
	else 
	{
		chance = it.hitChance;
	}

	if (it.weaponType == WEAPON_AMMO) 
	{
		if (const auto& bow = player->getWeapon(true)) 
		{
			// add even if zero is faster than trying to lookup the hit chance on an item twice
			chance += bow->getHitChance();
		}
	}

	if (chance >= uniform_random(1, 100)) 
	{
		Weapon::internalUseWeapon(player, item, target, damageModifier);
		return true;
	}

	auto destTile = target->getTile();
	if (not Position::areInRange<1, 1, 0>(playerPos, targetPos)) 
	{
		std::array<std::pair<int32_t, int32_t>, 9> shuffledOffsets = StandardHitChance::_DirectionOffsets;
		std::ranges::shuffle(shuffledOffsets, getRandomGenerator());

		const Position& base = target->getPosition();
		for (const auto& [dx, dy] : shuffledOffsets) 
		{
			if (auto tile = g_game.map.getTile(base.x + dx, base.y + dy, base.z);
				tile and tile->getGround() and not tile->hasFlag(TILESTATE_IMMOVABLEBLOCKSOLID)) 
			{
				destTile = tile;
				break;
			}
		}
	}

	Weapon::internalUseWeapon(player, item, destTile);
	return true;
}


int32_t WeaponDistance::getElementDamage(const PlayerConstPtr& player, const CreatureConstPtr& target, const ItemConstPtr& item) const
{
	if (elementType == COMBAT_NONE) {
		return 0;
	}

	int32_t attackValue = elementDamage;
	if (item->getWeaponType() == WEAPON_AMMO) {
		if (const auto weapon = player->getWeapon(true)) {
			attackValue += weapon->getAttack();
		}
	}

	int32_t attackSkill = player->getSkillLevel(SKILL_DISTANCE);
	float attackFactor = player->getAttackFactor();

	int32_t minValue = 0;
	int32_t maxValue = Weapons::getMaxWeaponDamage(player->getLevel(), attackSkill, attackValue, attackFactor);
	if (target) {
		if (target->getPlayer()) {
			minValue = static_cast<int32_t>(std::ceil(player->getLevel() * 0.1));
		} else {
			minValue = static_cast<int32_t>(std::ceil(player->getLevel() * 0.2));
		}
	}

	return -normal_random(minValue, static_cast<int32_t>(maxValue * player->getVocation()->distDamageMultiplier));
}

int32_t WeaponDistance::getWeaponDamage(const PlayerConstPtr& player, const CreatureConstPtr& target, const ItemConstPtr& item, bool maxDamage /*= false*/) const 
{

	const int32_t playerLevel = player->getLevel();
	const float playerAttackFactor = player->getAttackFactor();
	const auto& playerVocation = player->getVocation();
	int32_t attackValue = item->getAttack();

	if (item->getWeaponType() == WEAPON_AMMO) 
	{
		if (const auto weapon = player->getWeapon(true)) 
		{
			attackValue += weapon->getAttack();
		}
	}

	int32_t attackSkill = player->getSkillLevel(SKILL_DISTANCE);
	const float distDamageMultiplier = playerVocation->distDamageMultiplier;
	int32_t maxValue = static_cast<int32_t>(Weapons::getMaxWeaponDamage(playerLevel, attackSkill, attackValue, playerAttackFactor) * distDamageMultiplier);

	if (maxDamage) 
	{
		return -maxValue;
	}

	int32_t minValue = target ? static_cast<int32_t>(std::ceil(playerLevel * (target->getPlayer() ? 0.1 : 0.2))) : 0;
	return -normal_random(minValue, maxValue);
}

bool WeaponDistance::getSkillType(const PlayerConstPtr& player, const ItemConstPtr&, skills_t& skill, uint32_t& skillpoint) const
{
	skill = SKILL_DISTANCE;

	if (player->getAddAttackSkill()) {
		switch (player->getLastAttackBlockType()) {
			case BLOCK_NONE: {
				skillpoint = 2;
				break;
			}

			case BLOCK_DEFENSE:
			case BLOCK_ARMOR: {
				skillpoint = 1;
				break;
			}

			default:
				skillpoint = 0;
				break;
		}
	} else {
		skillpoint = 0;
	}
	return true;
}

bool WeaponWand::configureEvent(const pugi::xml_node& node)
{
	if (!Weapon::configureEvent(node)) {
		return false;
	}

	pugi::xml_attribute attr;
	if ((attr = node.attribute("min"))) {
		minChange = pugi::cast<int32_t>(attr.value());
	}

	if ((attr = node.attribute("max"))) {
		maxChange = pugi::cast<int32_t>(attr.value());
	}

	attr = node.attribute("type");
	if (!attr) {
		return true;
	}

	std::string tmpStrValue = asLowerCaseString(attr.as_string());
	if (tmpStrValue == "earth") {
		params.combatType = COMBAT_EARTHDAMAGE;
	} else if (tmpStrValue == "ice") {
		params.combatType = COMBAT_ICEDAMAGE;
	} else if (tmpStrValue == "energy") {
		params.combatType = COMBAT_ENERGYDAMAGE;
	} else if (tmpStrValue == "fire") {
		params.combatType = COMBAT_FIREDAMAGE;
	} else if (tmpStrValue == "death") {
		params.combatType = COMBAT_DEATHDAMAGE;
	} else if (tmpStrValue == "holy") {
		params.combatType = COMBAT_HOLYDAMAGE;
	} else {
		std::cout << "[Warning - WeaponWand::configureEvent] Type \"" << attr.as_string() << "\" does not exist." << std::endl;
	}
	return true;
}

void WeaponWand::configureWeapon(const ItemType& it)
{
	params.distanceEffect = it.shootType;

	Weapon::configureWeapon(it);
}

int32_t WeaponWand::getWeaponDamage(const PlayerConstPtr&, const CreatureConstPtr&, const ItemConstPtr&, bool maxDamage /*= false*/) const
{
	if (maxDamage) {
		return -maxChange;
	}
	return -normal_random(minChange, maxChange);
}
