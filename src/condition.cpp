// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "condition.h"
#include "game.h"
#include "monster.h"
#include "configmanager.h"
#include "metrics.h"

extern Game g_game;
extern ConfigManager g_config;

namespace BlackTek
{
	// Global PMR pool for all Condition allocations.
	// unsynchronized_pool_resource is used because condition management happens
	// on the main game thread. All sizes of Condition subclasses are served by
	// the pool's internal size-segregated free lists.
	std::pmr::unsynchronized_pool_resource g_condition_pool{ std::pmr::new_delete_resource() };
}

void* Condition::operator new(std::size_t size)
{
	return BlackTek::g_condition_pool.allocate(size, alignof(std::max_align_t));
}

void Condition::operator delete(void* ptr, std::size_t size) noexcept
{
	BlackTek::g_condition_pool.deallocate(ptr, size, alignof(std::max_align_t));
}

void intrusive_ptr_add_ref(const Condition* p) noexcept
{
	++p->m_ref_count;
}

void intrusive_ptr_release(const Condition* p) noexcept
{
	if (--p->m_ref_count == 0)
		delete const_cast<Condition*>(p);
}

bool Condition::setParam(ConditionParam_t param, int32_t value)
{
	switch (param) {
		case CONDITION_PARAM_TICKS: {
			ticks = value;
			return true;
		}

		case CONDITION_PARAM_BUFF_SPELL: {
			isBuff = (value != 0);
			return true;
		}

		case CONDITION_PARAM_SUBID: {
			subId = value;
			return true;
		}

		case CONDITION_PARAM_AGGRESSIVE: {
			aggressive = (value != 0);
			return true;
		}

		default: {
			return false;
		}
	}
}

int32_t Condition::getParam(ConditionParam_t param)
{
	switch (param) {
		case CONDITION_PARAM_TICKS:
			return ticks;

		case CONDITION_PARAM_BUFF_SPELL:
			return isBuff ? 1: 0;

		case CONDITION_PARAM_SUBID:
			return subId;

		default:
			return std::numeric_limits<int32_t>().max();
	}
}

bool Condition::unserialize(PropStream& propStream)
{
	uint8_t attr_type;
	while (propStream.read<uint8_t>(attr_type) && attr_type != CONDITIONATTR_END) {
		if (!unserializeProp(static_cast<ConditionAttr_t>(attr_type), propStream)) {
			return false;
		}
	}
	return true;
}

bool Condition::unserializeProp(ConditionAttr_t attr, PropStream& propStream)
{
	switch (attr) {
		case CONDITIONATTR_TYPE: {
			int32_t value;
			if (!propStream.read<int32_t>(value)) {
				return false;
			}

			conditionType = static_cast<ConditionType_t>(value);
			return true;
		}

		case CONDITIONATTR_ID: {
			int32_t value;
			if (!propStream.read<int32_t>(value)) {
				return false;
			}

			id = static_cast<ConditionId_t>(value);
			return true;
		}

		case CONDITIONATTR_TICKS: {
			return propStream.read<int32_t>(ticks);
		}

		case CONDITIONATTR_ISBUFF: {
			uint8_t value;
			if (!propStream.read<uint8_t>(value)) {
				return false;
			}

			isBuff = (value != 0);
			return true;
		}

		case CONDITIONATTR_SUBID: {
			return propStream.read<uint32_t>(subId);
		}

		case CONDITIONATTR_ISAGGRESSIVE: {
			uint8_t value;
			if (!propStream.read<uint8_t>(value)) {
				return false;
			}

			aggressive = (value != 0);
			return true;
		}

		case CONDITIONATTR_END:
			return true;

		default:
			return false;
	}
}

void Condition::serialize(PropWriteStream& propWriteStream)
{
	propWriteStream.write<uint8_t>(CONDITIONATTR_TYPE);
	propWriteStream.write<uint32_t>(conditionType);

	propWriteStream.write<uint8_t>(CONDITIONATTR_ID);
	propWriteStream.write<uint32_t>(id);

	propWriteStream.write<uint8_t>(CONDITIONATTR_TICKS);
	propWriteStream.write<uint32_t>(ticks);

	propWriteStream.write<uint8_t>(CONDITIONATTR_ISBUFF);
	propWriteStream.write<uint8_t>(isBuff);

	propWriteStream.write<uint8_t>(CONDITIONATTR_SUBID);
	propWriteStream.write<uint32_t>(subId);

	propWriteStream.write<uint8_t>(CONDITIONATTR_ISAGGRESSIVE);
	propWriteStream.write<uint8_t>(aggressive);
}

void Condition::setTicks(int32_t newTicks)
{
	ticks = newTicks;
	endTime = ticks + OTSYS_TIME();
}

bool Condition::executeCondition(CreaturePtr, int32_t interval)
{
	if (ticks == -1) {
		return true;
	}

	//Not using set ticks here since it would reset endTime
	ticks = std::max<int32_t>(0, ticks - interval);
	return getEndTime() >= OTSYS_TIME();
}

ConditionHandle Condition::createCondition(ConditionId_t id, ConditionType_t type, int32_t ticks, int32_t param/* = 0*/, bool buff/* = false*/, uint32_t subId/* = 0*/, bool aggressive/* = false */)
{
	switch (type) {
		case CONDITION_POISON:
		case CONDITION_FIRE:
		case CONDITION_ENERGY:
		case CONDITION_DROWN:
		case CONDITION_FREEZING:
		case CONDITION_DAZZLED:
		case CONDITION_CURSED:
		case CONDITION_BLEEDING:
			return ConditionHandle(new ConditionDamage(id, type, buff, subId, aggressive));

		case CONDITION_HASTE:
		case CONDITION_PARALYZE:
			return ConditionHandle(new ConditionSpeed(id, type, ticks, buff, subId, param, aggressive));

		case CONDITION_INVISIBLE:
			return ConditionHandle(new ConditionInvisible(id, type, ticks, buff, subId, aggressive));

		case CONDITION_OUTFIT:
			return ConditionHandle(new ConditionOutfit(id, type, ticks, buff, subId, aggressive));

		case CONDITION_LIGHT:
			return ConditionHandle(new ConditionLight(id, type, ticks, buff, subId, param & 0xFF, (param & 0xFF00) >> 8, aggressive));

		case CONDITION_REGENERATION:
			return ConditionHandle(new ConditionRegeneration(id, type, ticks, buff, subId, aggressive));

		case CONDITION_SOUL:
			return ConditionHandle(new ConditionSoul(id, type, ticks, buff, subId, aggressive));

		case CONDITION_ATTRIBUTES:
			return ConditionHandle(new ConditionAttributes(id, type, ticks, buff, subId, aggressive));

		case CONDITION_SPELLCOOLDOWN:
			return ConditionHandle(new ConditionSpellCooldown(id, type, ticks, buff, subId, aggressive));

		case CONDITION_SPELLGROUPCOOLDOWN:
			return ConditionHandle(new ConditionSpellGroupCooldown(id, type, ticks, buff, subId, aggressive));

		case CONDITION_DRUNK:
			return ConditionHandle(new ConditionDrunk(id, type, ticks, buff, subId, param, aggressive));

		case CONDITION_INFIGHT:
		case CONDITION_EXHAUST_WEAPON:
		case CONDITION_EXHAUST_COMBAT:
		case CONDITION_EXHAUST_HEAL:
		case CONDITION_MUTED:
		case CONDITION_CHANNELMUTEDTICKS:
		case CONDITION_YELLTICKS:
		case CONDITION_PACIFIED:
		case CONDITION_MANASHIELD:
			return ConditionHandle(new ConditionGeneric(id, type, ticks, buff, subId, aggressive));

		default:
			return {};
	}
}

ConditionHandle Condition::createCondition(PropStream& propStream)
{
	uint8_t attr;
	if (!propStream.read<uint8_t>(attr) || attr != CONDITIONATTR_TYPE) {
		return {};
	}

	uint32_t type;
	if (!propStream.read<uint32_t>(type)) {
		return {};
	}

	if (!propStream.read<uint8_t>(attr) || attr != CONDITIONATTR_ID) {
		return {};
	}

	uint32_t id;
	if (!propStream.read<uint32_t>(id)) {
		return {};
	}

	if (!propStream.read<uint8_t>(attr) || attr != CONDITIONATTR_TICKS) {
		return {};
	}

	uint32_t ticks;
	if (!propStream.read<uint32_t>(ticks)) {
		return {};
	}

	if (!propStream.read<uint8_t>(attr) || attr != CONDITIONATTR_ISBUFF) {
		return {};
	}

	uint8_t buff;
	if (!propStream.read<uint8_t>(buff)) {
		return {};
	}

	if (!propStream.read<uint8_t>(attr) || attr != CONDITIONATTR_SUBID) {
		return {};
	}

	uint32_t subId;
	if (!propStream.read<uint32_t>(subId)) {
		return {};
	}

	if (!propStream.read<uint8_t>(attr) || attr != CONDITIONATTR_ISAGGRESSIVE) {
		return {};
	}

	uint8_t aggressive;
	if (!propStream.read<uint8_t>(aggressive)) {
		return {};
	}

	return createCondition(static_cast<ConditionId_t>(id), static_cast<ConditionType_t>(type), ticks, 0, buff != 0, subId, aggressive);
}

bool Condition::startCondition(CreaturePtr)
{
	if (ticks > 0) {
		endTime = ticks + OTSYS_TIME();
	}
	return true;
}

bool Condition::isPersistent() const
{
	if (ticks == -1) {
		return false;
	}

	if (!(id == CONDITIONID_DEFAULT || id == CONDITIONID_COMBAT || conditionType == CONDITION_MUTED)) {
		return false;
	}

	return true;
}

uint32_t Condition::getIcons() const
{
	return isBuff ? ICON_PARTY_BUFF : 0;
}

bool Condition::updateCondition(const Condition* addCondition)
{
	if (conditionType != addCondition->getType()) {
		return false;
	}

	if (ticks == -1 && addCondition->getTicks() > 0) {
		return false;
	}

	if (addCondition->getTicks() >= 0 && getEndTime() > (OTSYS_TIME() + addCondition->getTicks())) {
		return false;
	}

	return true;
}

bool ConditionGeneric::startCondition(const CreaturePtr creature)
{
	return Condition::startCondition(creature);
}

bool ConditionGeneric::executeCondition(const CreaturePtr creature, int32_t interval)
{
	return Condition::executeCondition(creature, interval);
}

void ConditionGeneric::endCondition(CreaturePtr)
{
	//
}

void ConditionGeneric::addCondition(CreaturePtr, const Condition* condition)
{
	if (updateCondition(condition)) {
		setTicks(condition->getTicks());
	}
}

uint32_t ConditionGeneric::getIcons() const
{
	uint32_t icons = Condition::getIcons();

	switch (conditionType) {
		case CONDITION_MANASHIELD:
			icons |= ICON_MANASHIELD;
			break;

		case CONDITION_INFIGHT:
			icons |= ICON_SWORDS;
			break;

		default:
			break;
	}

	return icons;
}

void ConditionAttributes::addCondition(const CreaturePtr creature, const Condition* condition)
{
	if (updateCondition(condition)) {
		setTicks(condition->getTicks());

		const ConditionAttributes& conditionAttrs = static_cast<const ConditionAttributes&>(*condition);
		//Remove the old condition
		endCondition(creature);

		//Apply the new one
		memcpy(skills, conditionAttrs.skills, sizeof(skills));
		memcpy(specialSkills, conditionAttrs.specialSkills, sizeof(specialSkills));
		memcpy(skillsPercent, conditionAttrs.skillsPercent, sizeof(skillsPercent));
		memcpy(stats, conditionAttrs.stats, sizeof(stats));
		memcpy(statsPercent, conditionAttrs.statsPercent, sizeof(statsPercent));
		disableDefense = conditionAttrs.disableDefense;

		if (const auto player = creature->getPlayer()) {
			updatePercentSkills(player);
			updateSkills(player);
			updatePercentStats(player);
			updateStats(player);
		}
	}
}

bool ConditionAttributes::unserializeProp(ConditionAttr_t attr, PropStream& propStream)
{
	if (attr == CONDITIONATTR_SKILLS) {
		return propStream.read<int32_t>(skills[currentSkill++]);
	} else if (attr == CONDITIONATTR_SPECIALSKILLS) {
		return propStream.read<int32_t>(specialSkills[currentSpecialSkill++]);
	} else if (attr == CONDITIONATTR_STATS) {
		return propStream.read<int32_t>(stats[currentStat++]);
	} else if (attr == CONDITIONATTR_DISABLEDEFENSE) {
		return propStream.read<bool>(disableDefense);
	}
	return Condition::unserializeProp(attr, propStream);
}

void ConditionAttributes::serialize(PropWriteStream& propWriteStream)
{
	Condition::serialize(propWriteStream);

	for (int32_t i = SKILL_FIRST; i <= SKILL_LAST; ++i) {
		propWriteStream.write<uint8_t>(CONDITIONATTR_SKILLS);
		propWriteStream.write<int32_t>(skills[i]);
	}

	for (int32_t i = STAT_FIRST; i <= STAT_LAST; ++i) {
		propWriteStream.write<uint8_t>(CONDITIONATTR_STATS);
		propWriteStream.write<int32_t>(stats[i]);
	}

	propWriteStream.write<uint8_t>(CONDITIONATTR_DISABLEDEFENSE);
	propWriteStream.write<bool>(disableDefense);

	for (int32_t i = SPECIALSKILL_FIRST; i <= SPECIALSKILL_LAST; ++i) {
		propWriteStream.write<uint8_t>(CONDITIONATTR_SPECIALSKILLS);
		propWriteStream.write<int32_t>(specialSkills[i]);
	}
}

bool ConditionAttributes::startCondition(CreaturePtr creature)
{
	if (!Condition::startCondition(creature)) {
		return false;
	}

	creature->setUseDefense(!disableDefense);

	if (const auto player = creature->getPlayer()) {
		updatePercentSkills(player);
		updateSkills(player);
		updatePercentStats(player);
		updateStats(player);
	}

	return true;
}

void ConditionAttributes::updatePercentStats(const PlayerPtr& player)
{
	for (int32_t i = STAT_FIRST; i <= STAT_LAST; ++i) {
		if (statsPercent[i] == 0) {
			continue;
		}

		switch (i) {
			case STAT_MAXHITPOINTS:
				stats[i] = static_cast<int32_t>(player->getMaxHealth() * ((statsPercent[i] - 100) / 100.f));
				break;

			case STAT_MAXMANAPOINTS:
				stats[i] = static_cast<int32_t>(player->getMaxMana() * ((statsPercent[i] - 100) / 100.f));
				break;

			case STAT_MAGICPOINTS:
				stats[i] = static_cast<int32_t>(player->getBaseMagicLevel() * ((statsPercent[i] - 100) / 100.f));
				break;
		}
	}
}

void ConditionAttributes::updateStats(const PlayerPtr& player) const
{
	bool needUpdateStats = false;

	for (int32_t i = STAT_FIRST; i <= STAT_LAST; ++i) {
		if (stats[i]) {
			needUpdateStats = true;
			player->setVarStats(static_cast<stats_t>(i), stats[i]);
		}
	}

	if (needUpdateStats) {
		player->sendStats();
	}
}

void ConditionAttributes::updatePercentSkills(const PlayerPtr& player)
{
	for (uint8_t i = SKILL_FIRST; i <= SKILL_LAST; ++i) {
		if (skillsPercent[i] == 0) {
			continue;
		}

		int32_t unmodifiedSkill = player->getBaseSkill(i);
		skills[i] = static_cast<int32_t>(unmodifiedSkill * ((skillsPercent[i] - 100) / 100.f));
	}
}

void ConditionAttributes::updateSkills(const PlayerPtr& player) const
{
	bool needUpdateSkills = false;

	for (int32_t i = SKILL_FIRST; i <= SKILL_LAST; ++i) {
		if (skills[i]) {
			needUpdateSkills = true;
			player->setVarSkill(static_cast<skills_t>(i), skills[i]);
		}
	}

	for (int32_t i = SPECIALSKILL_FIRST; i <= SPECIALSKILL_LAST; ++i) {
		if (specialSkills[i]) {
			needUpdateSkills = true;
			player->setVarSpecialSkill(static_cast<SpecialSkills_t>(i), specialSkills[i]);
		}
	}

	if (needUpdateSkills) {
		player->sendSkills();
	}
}

bool ConditionAttributes::executeCondition(const CreaturePtr creature, int32_t interval)
{
	return ConditionGeneric::executeCondition(creature, interval);
}

void ConditionAttributes::endCondition(const CreaturePtr creature)
{
	if (const auto player = creature->getPlayer()) {
		bool needUpdateSkills = false;

		for (int32_t i = SKILL_FIRST; i <= SKILL_LAST; ++i) {
			if (skills[i] || skillsPercent[i]) {
				needUpdateSkills = true;
				player->setVarSkill(static_cast<skills_t>(i), -skills[i]);
			}
		}

		for (int32_t i = SPECIALSKILL_FIRST; i <= SPECIALSKILL_LAST; ++i) {
			if (specialSkills[i]) {
				needUpdateSkills = true;
				player->setVarSpecialSkill(static_cast<SpecialSkills_t>(i), -specialSkills[i]);
			}
		}

		if (needUpdateSkills) {
			player->sendSkills();
		}

		bool needUpdateStats = false;

		for (int32_t i = STAT_FIRST; i <= STAT_LAST; ++i) {
			if (stats[i]) {
				needUpdateStats = true;
				player->setVarStats(static_cast<stats_t>(i), -stats[i]);
			}
		}

		if (needUpdateStats) {
			player->sendStats();
		}
	}

	if (disableDefense) {
		creature->setUseDefense(true);
	}
}

bool ConditionAttributes::setParam(ConditionParam_t param, int32_t value)
{
	bool ret = ConditionGeneric::setParam(param, value);

	switch (param) {
		case CONDITION_PARAM_SKILL_MELEE: {
			skills[SKILL_CLUB] = value;
			skills[SKILL_AXE] = value;
			skills[SKILL_SWORD] = value;
			return true;
		}

		case CONDITION_PARAM_SKILL_MELEEPERCENT: {
			skillsPercent[SKILL_CLUB] = value;
			skillsPercent[SKILL_AXE] = value;
			skillsPercent[SKILL_SWORD] = value;
			return true;
		}

		case CONDITION_PARAM_SKILL_FIST: {
			skills[SKILL_FIST] = value;
			return true;
		}

		case CONDITION_PARAM_SKILL_FISTPERCENT: {
			skillsPercent[SKILL_FIST] = value;
			return true;
		}

		case CONDITION_PARAM_SKILL_CLUB: {
			skills[SKILL_CLUB] = value;
			return true;
		}

		case CONDITION_PARAM_SKILL_CLUBPERCENT: {
			skillsPercent[SKILL_CLUB] = value;
			return true;
		}

		case CONDITION_PARAM_SKILL_SWORD: {
			skills[SKILL_SWORD] = value;
			return true;
		}

		case CONDITION_PARAM_SKILL_SWORDPERCENT: {
			skillsPercent[SKILL_SWORD] = value;
			return true;
		}

		case CONDITION_PARAM_SKILL_AXE: {
			skills[SKILL_AXE] = value;
			return true;
		}

		case CONDITION_PARAM_SKILL_AXEPERCENT: {
			skillsPercent[SKILL_AXE] = value;
			return true;
		}

		case CONDITION_PARAM_SKILL_DISTANCE: {
			skills[SKILL_DISTANCE] = value;
			return true;
		}

		case CONDITION_PARAM_SKILL_DISTANCEPERCENT: {
			skillsPercent[SKILL_DISTANCE] = value;
			return true;
		}

		case CONDITION_PARAM_SKILL_SHIELD: {
			skills[SKILL_SHIELD] = value;
			return true;
		}

		case CONDITION_PARAM_SKILL_SHIELDPERCENT: {
			skillsPercent[SKILL_SHIELD] = value;
			return true;
		}

		case CONDITION_PARAM_SKILL_FISHING: {
			skills[SKILL_FISHING] = value;
			return true;
		}

		case CONDITION_PARAM_SKILL_FISHINGPERCENT: {
			skillsPercent[SKILL_FISHING] = value;
			return true;
		}

		case CONDITION_PARAM_STAT_MAXHITPOINTS: {
			stats[STAT_MAXHITPOINTS] = value;
			return true;
		}

		case CONDITION_PARAM_STAT_MAXMANAPOINTS: {
			stats[STAT_MAXMANAPOINTS] = value;
			return true;
		}

		case CONDITION_PARAM_STAT_MAGICPOINTS: {
			stats[STAT_MAGICPOINTS] = value;
			return true;
		}

		case CONDITION_PARAM_STAT_MAXHITPOINTSPERCENT: {
			statsPercent[STAT_MAXHITPOINTS] = std::max<int32_t>(0, value);
			return true;
		}

		case CONDITION_PARAM_STAT_MAXMANAPOINTSPERCENT: {
			statsPercent[STAT_MAXMANAPOINTS] = std::max<int32_t>(0, value);
			return true;
		}

		case CONDITION_PARAM_STAT_MAGICPOINTSPERCENT: {
			statsPercent[STAT_MAGICPOINTS] = std::max<int32_t>(0, value);
			return true;
		}

		case CONDITION_PARAM_DISABLE_DEFENSE: {
			disableDefense = (value != 0);
			return true;
		}

		case CONDITION_PARAM_SPECIALSKILL_CRITICALHITCHANCE: {
			specialSkills[SPECIALSKILL_CRITICALHITCHANCE] = value;
			return true;
		}

		case CONDITION_PARAM_SPECIALSKILL_CRITICALHITAMOUNT: {
			specialSkills[SPECIALSKILL_CRITICALHITAMOUNT] = value;
			return true;
		}

		case CONDITION_PARAM_SPECIALSKILL_LIFELEECHCHANCE: {
			specialSkills[SPECIALSKILL_LIFELEECHCHANCE] = value;
			return true;
		}

		case CONDITION_PARAM_SPECIALSKILL_LIFELEECHAMOUNT: {
			specialSkills[SPECIALSKILL_LIFELEECHAMOUNT] = value;
			return true;
		}

		case CONDITION_PARAM_SPECIALSKILL_MANALEECHCHANCE: {
			specialSkills[SPECIALSKILL_MANALEECHCHANCE] = value;
			return true;
		}

		case CONDITION_PARAM_SPECIALSKILL_MANALEECHAMOUNT: {
			specialSkills[SPECIALSKILL_MANALEECHAMOUNT] = value;
			return true;
		}

		case CONDITION_PARAM_AGGRESSIVE: {
			aggressive = (value != 0);
			return true;
		}

		default:
			return ret;
	}
}

int32_t ConditionAttributes::getParam(const ConditionParam_t param)
{
	switch (param) {
		case CONDITION_PARAM_SKILL_FIST:
			return skills[SKILL_FIST];

		case CONDITION_PARAM_SKILL_FISTPERCENT:
			return skillsPercent[SKILL_FIST];

		case CONDITION_PARAM_SKILL_CLUB:
			return skills[SKILL_CLUB];

		case CONDITION_PARAM_SKILL_CLUBPERCENT:
			return skillsPercent[SKILL_CLUB];

		case CONDITION_PARAM_SKILL_SWORD:
			return skills[SKILL_SWORD];

		case CONDITION_PARAM_SKILL_SWORDPERCENT:
			return skillsPercent[SKILL_SWORD];

		case CONDITION_PARAM_SKILL_AXE:
			return skills[SKILL_AXE];

		case CONDITION_PARAM_SKILL_AXEPERCENT:
			return skillsPercent[SKILL_AXE];

		case CONDITION_PARAM_SKILL_DISTANCE:
			return skills[SKILL_DISTANCE];

		case CONDITION_PARAM_SKILL_DISTANCEPERCENT:
			return skillsPercent[SKILL_DISTANCE];

		case CONDITION_PARAM_SKILL_SHIELD:
			return skills[SKILL_SHIELD];

		case CONDITION_PARAM_SKILL_SHIELDPERCENT:
			return skillsPercent[SKILL_SHIELD];

		case CONDITION_PARAM_SKILL_FISHING:
			return skills[SKILL_FISHING];

		case CONDITION_PARAM_SKILL_FISHINGPERCENT: 
			return skillsPercent[SKILL_FISHING];

		case CONDITION_PARAM_STAT_MAXHITPOINTS:
			return stats[STAT_MAXHITPOINTS];

		case CONDITION_PARAM_STAT_MAXMANAPOINTS:
			return stats[STAT_MAXMANAPOINTS];

		case CONDITION_PARAM_STAT_MAGICPOINTS:
			return stats[STAT_MAGICPOINTS];

		case CONDITION_PARAM_STAT_MAXHITPOINTSPERCENT:
			return statsPercent[STAT_MAXHITPOINTS];

		case CONDITION_PARAM_STAT_MAXMANAPOINTSPERCENT:
			return statsPercent[STAT_MAXMANAPOINTS];

		case CONDITION_PARAM_STAT_MAGICPOINTSPERCENT:
			return statsPercent[STAT_MAGICPOINTS];

		case CONDITION_PARAM_DISABLE_DEFENSE:
			return disableDefense ? 1 : 0;

		case CONDITION_PARAM_SPECIALSKILL_CRITICALHITCHANCE:
			return specialSkills[SPECIALSKILL_CRITICALHITCHANCE];

		case CONDITION_PARAM_SPECIALSKILL_CRITICALHITAMOUNT:
			return specialSkills[SPECIALSKILL_CRITICALHITAMOUNT];

		case CONDITION_PARAM_SPECIALSKILL_LIFELEECHCHANCE:
			return specialSkills[SPECIALSKILL_LIFELEECHCHANCE];

		case CONDITION_PARAM_SPECIALSKILL_LIFELEECHAMOUNT:
			return specialSkills[SPECIALSKILL_LIFELEECHAMOUNT];

		case CONDITION_PARAM_SPECIALSKILL_MANALEECHCHANCE:
			return specialSkills[SPECIALSKILL_MANALEECHCHANCE];

		case CONDITION_PARAM_SPECIALSKILL_MANALEECHAMOUNT:
			return specialSkills[SPECIALSKILL_MANALEECHAMOUNT];

		default:
			return ConditionGeneric::getParam(param);
	}
}

void ConditionRegeneration::addCondition(CreaturePtr, const Condition* condition)
{
	if (updateCondition(condition)) {
		setTicks(condition->getTicks());

		const ConditionRegeneration& conditionRegen = static_cast<const ConditionRegeneration&>(*condition);

		healthTicks = conditionRegen.healthTicks;
		manaTicks = conditionRegen.manaTicks;

		healthGain = conditionRegen.healthGain;
		manaGain = conditionRegen.manaGain;
	}
}

bool ConditionRegeneration::unserializeProp(const ConditionAttr_t attr, PropStream& propStream)
{
	if (attr == CONDITIONATTR_HEALTHTICKS) {
		return propStream.read<uint32_t>(healthTicks);
	} else if (attr == CONDITIONATTR_HEALTHGAIN) {
		return propStream.read<uint32_t>(healthGain);
	} else if (attr == CONDITIONATTR_MANATICKS) {
		return propStream.read<uint32_t>(manaTicks);
	} else if (attr == CONDITIONATTR_MANAGAIN) {
		return propStream.read<uint32_t>(manaGain);
	}
	return Condition::unserializeProp(attr, propStream);
}

void ConditionRegeneration::serialize(PropWriteStream& propWriteStream)
{
	Condition::serialize(propWriteStream);

	propWriteStream.write<uint8_t>(CONDITIONATTR_HEALTHTICKS);
	propWriteStream.write<uint32_t>(healthTicks);

	propWriteStream.write<uint8_t>(CONDITIONATTR_HEALTHGAIN);
	propWriteStream.write<uint32_t>(healthGain);

	propWriteStream.write<uint8_t>(CONDITIONATTR_MANATICKS);
	propWriteStream.write<uint32_t>(manaTicks);

	propWriteStream.write<uint8_t>(CONDITIONATTR_MANAGAIN);
	propWriteStream.write<uint32_t>(manaGain);
}

bool ConditionRegeneration::executeCondition(const CreaturePtr creature, int32_t interval)
{
	internalHealthTicks += interval;
	internalManaTicks += interval;

	if (creature->getZone() == ZONE_PROTECTION) {
		return ConditionGeneric::executeCondition(creature, interval);
	}

	if (internalHealthTicks >= healthTicks && healthGain != 0) {
		internalHealthTicks = 0;
		auto hpRegen = BlackTek::g_combat_registry.Create(
		    static_cast<uint16_t>(BlackTek::Combat::DamageType::Healing),
		    static_cast<uint32_t>(healthGain));
		hpRegen->SetConfig(BlackTek::Combat::Config::HealthTarget);
		hpRegen->SetConfig(BlackTek::Combat::Config::TrueDamage);
		hpRegen->heal_target(creature, creature, true);
	}

	if (internalManaTicks >= manaTicks && manaGain != 0) {
		internalManaTicks = 0;

		if (auto player = creature->getPlayer()) {
			auto manaRegen = BlackTek::g_combat_registry.Create(
			    static_cast<uint16_t>(BlackTek::Combat::DamageType::Healing),
			    static_cast<uint32_t>(manaGain));
			manaRegen->SetConfig(BlackTek::Combat::Config::ManaTarget);
			manaRegen->SetConfig(BlackTek::Combat::Config::TrueDamage);
			manaRegen->heal_target(creature, player, true);
		}
	}

	return ConditionGeneric::executeCondition(creature, interval);
}

bool ConditionRegeneration::setParam(ConditionParam_t param, int32_t value)
{
	const bool ret = ConditionGeneric::setParam(param, value);

	switch (param) {
		case CONDITION_PARAM_HEALTHGAIN:
			healthGain = value;
			return true;

		case CONDITION_PARAM_HEALTHTICKS:
			healthTicks = value;
			return true;

		case CONDITION_PARAM_MANAGAIN:
			manaGain = value;
			return true;

		case CONDITION_PARAM_MANATICKS:
			manaTicks = value;
			return true;

		default:
			return ret;
	}
}

int32_t ConditionRegeneration::getParam(ConditionParam_t param)
{
	switch (param) {
		case CONDITION_PARAM_HEALTHGAIN:
			return healthGain;

		case CONDITION_PARAM_HEALTHTICKS:
			return healthTicks;

		case CONDITION_PARAM_MANAGAIN:
			return manaGain;

		case CONDITION_PARAM_MANATICKS:
			return manaTicks;

		default:
			return ConditionGeneric::getParam(param);
	}
}

void ConditionSoul::addCondition(CreaturePtr, const Condition* condition)
{
	if (updateCondition(condition)) {
		setTicks(condition->getTicks());

		const auto& conditionSoul = static_cast<const ConditionSoul&>(*condition);

		soulTicks = conditionSoul.soulTicks;
		soulGain = conditionSoul.soulGain;
	}
}

bool ConditionSoul::unserializeProp(ConditionAttr_t attr, PropStream& propStream)
{
	if (attr == CONDITIONATTR_SOULGAIN) {
		return propStream.read<uint32_t>(soulGain);
	} else if (attr == CONDITIONATTR_SOULTICKS) {
		return propStream.read<uint32_t>(soulTicks);
	}
	return Condition::unserializeProp(attr, propStream);
}

void ConditionSoul::serialize(PropWriteStream& propWriteStream)
{
	Condition::serialize(propWriteStream);

	propWriteStream.write<uint8_t>(CONDITIONATTR_SOULGAIN);
	propWriteStream.write<uint32_t>(soulGain);

	propWriteStream.write<uint8_t>(CONDITIONATTR_SOULTICKS);
	propWriteStream.write<uint32_t>(soulTicks);
}

bool ConditionSoul::executeCondition(const CreaturePtr creature, int32_t interval)
{
	internalSoulTicks += interval;

	if (const auto player = creature->getPlayer()) {
		if (player->getZone() != ZONE_PROTECTION) {
			if (internalSoulTicks >= soulTicks) {
				internalSoulTicks = 0;
				player->changeSoul(soulGain);
			}
		}
	}

	return ConditionGeneric::executeCondition(creature, interval);
}

bool ConditionSoul::setParam(const ConditionParam_t param, const int32_t value)
{
	const bool ret = ConditionGeneric::setParam(param, value);
	switch (param) {
		case CONDITION_PARAM_SOULGAIN:
			soulGain = value;
			return true;

		case CONDITION_PARAM_SOULTICKS:
			soulTicks = value;
			return true;

		default:
			return ret;
	}
}

int32_t ConditionSoul::getParam(const ConditionParam_t param)
{
	switch (param) {
		case CONDITION_PARAM_SOULGAIN:
			return soulGain;

		case CONDITION_PARAM_SOULTICKS:
			return soulTicks;

		default:
			return ConditionGeneric::getParam(param);
	}
}

bool ConditionDamage::setParam(const ConditionParam_t param, const int32_t value)
{
	bool ret = Condition::setParam(param, value);

	switch (param) {
		case CONDITION_PARAM_OWNER:
			owner = value;
			return true;

		case CONDITION_PARAM_FORCEUPDATE:
			forceUpdate = (value != 0);
			return true;

		case CONDITION_PARAM_DELAYED:
			delayed = (value != 0);
			return true;

		case CONDITION_PARAM_MAXVALUE:
			maxDamage = std::abs(value);
			break;

		case CONDITION_PARAM_MINVALUE:
			minDamage = std::abs(value);
			break;

		case CONDITION_PARAM_STARTVALUE:
			startDamage = std::abs(value);
			break;

		case CONDITION_PARAM_TICKINTERVAL:
			tickInterval = std::abs(value);
			break;

		case CONDITION_PARAM_PERIODICDAMAGE:
			periodDamage = value;
			break;

		case CONDITION_PARAM_FIELD:
			field = (value != 0);
			break;

		default:
			return false;
	}

	return ret;
}

int32_t ConditionDamage::getParam(const ConditionParam_t param)
{
	switch (param) {
		case CONDITION_PARAM_OWNER:
			return owner;

		case CONDITION_PARAM_FORCEUPDATE:
			return forceUpdate ? 1 : 0;

		case CONDITION_PARAM_DELAYED:
			return delayed ? 1 : 0;

		case CONDITION_PARAM_MAXVALUE:
			return maxDamage;

		case CONDITION_PARAM_MINVALUE:
			return minDamage;

		case CONDITION_PARAM_STARTVALUE:
			return startDamage;

		case CONDITION_PARAM_TICKINTERVAL:
			return tickInterval;

		case CONDITION_PARAM_PERIODICDAMAGE:
			return periodDamage;

		case CONDITION_PARAM_FIELD:
			return field ? 1 : 0;

		default:
			return Condition::getParam(param);
	}
}

bool ConditionDamage::unserializeProp(const ConditionAttr_t attr, PropStream& propStream)
{
	if (attr == CONDITIONATTR_DELAYED) {
		uint8_t value;
		if (!propStream.read<uint8_t>(value)) {
			return false;
		}

		delayed = (value != 0);
		return true;
	} else if (attr == CONDITIONATTR_PERIODDAMAGE) {
		return propStream.read<int32_t>(periodDamage);
	} else if (attr == CONDITIONATTR_OWNER) {
		return propStream.skip(4);
	} else if (attr == CONDITIONATTR_INTERVALDATA) {
		IntervalInfo damageInfo;
		if (!propStream.read<IntervalInfo>(damageInfo)) {
			return false;
		}

		damageList.push_back(damageInfo);
		if (ticks != -1) {
			setTicks(ticks + damageInfo.interval);
		}
		return true;
	}
	return Condition::unserializeProp(attr, propStream);
}

void ConditionDamage::serialize(PropWriteStream& propWriteStream)
{
	Condition::serialize(propWriteStream);

	propWriteStream.write<uint8_t>(CONDITIONATTR_DELAYED);
	propWriteStream.write<uint8_t>(delayed);

	propWriteStream.write<uint8_t>(CONDITIONATTR_PERIODDAMAGE);
	propWriteStream.write<int32_t>(periodDamage);

	for (size_t i = damage_front; i < damageList.size(); ++i) {
		propWriteStream.write<uint8_t>(CONDITIONATTR_INTERVALDATA);
		propWriteStream.write<IntervalInfo>(damageList[i]);
	}
}

bool ConditionDamage::updateCondition(const Condition* addCondition)
{
	const auto& conditionDamage = static_cast<const ConditionDamage&>(*addCondition);
	if (conditionDamage.doForceUpdate()) {
		return true;
	}

	if (ticks == -1 && conditionDamage.ticks > 0) {
		return false;
	}

	return conditionDamage.getTotalDamage() > getTotalDamage();
}

bool ConditionDamage::addDamage(const int32_t rounds, int32_t time, const int32_t value)
{
	time = std::max<int32_t>(time, EVENT_CREATURE_THINK_INTERVAL);
	if (rounds == -1) {
		//periodic damage
		periodDamage = value;
		setParam(CONDITION_PARAM_TICKINTERVAL, time);
		setParam(CONDITION_PARAM_TICKS, -1);
		return true;
	}

	if (periodDamage > 0) {
		return false;
	}

	//rounds, time, damage
	for (int32_t i = 0; i < rounds; ++i) {
		IntervalInfo damageInfo;
		damageInfo.interval = time;
		damageInfo.timeLeft = time;
		damageInfo.value = value;

		damageList.push_back(damageInfo);

		if (ticks != -1) {
			setTicks(ticks + damageInfo.interval);
		}
	}

	return true;
}

bool ConditionDamage::init()
{
	if (periodDamage != 0) {
		return true;
	}

	if (damage_front < damageList.size()) {
		return true;
	}

	setTicks(0);

	if (const auto amount = uniform_random(minDamage, maxDamage); amount != 0) {
		if (startDamage > maxDamage) {
			startDamage = maxDamage;
		} else if (startDamage == 0) {
			startDamage = std::max<int32_t>(1, std::ceil(amount / 20.0));
		}

		std::vector<int32_t> list;
		ConditionDamage::generateDamageList(amount, startDamage, list);
		for (int32_t value : list) {
			addDamage(1, tickInterval, -value);
		}
	}
	return damage_front < damageList.size();
}

bool ConditionDamage::startCondition(const CreaturePtr creature)
{
	if (!Condition::startCondition(creature)) {
		return false;
	}

	if (!delayed) {
		// delayed condition does no initial damage
		if (!doDamage(creature, initDamage)) {
			return false;
		}
	}

	if (!init()) {
		return false;
	}
	return true;
}

namespace BlackTek
{
	[[nodiscard]] uint8_t ConditionMetricsVocationOf(const CreaturePtr& creature) noexcept
	{
		if (not creature or creature->getCreatureSubType() != CreatureSubType::Player)
			return 0;
		const auto* vocation = creature->getPlayer()->getVocation();
		return vocation ? static_cast<uint8_t>(vocation->getId()) : 0;
	}

	// Bit values match Combat::Situation (combat.h) without requiring that header here.
	[[nodiscard]] uint8_t ConditionMetricsSituationOf(const CreaturePtr& a, const CreaturePtr& b) noexcept
	{
		const bool aPlayer = a and a->getCreatureSubType() == CreatureSubType::Player;
		const bool bPlayer = b and b->getCreatureSubType() == CreatureSubType::Player;

		if (aPlayer)
			return bPlayer ? 1u : 2u; // PlayerVsPlayer : PlayerVsMonster
		return bPlayer ? 4u : 8u;     // MonsterVsPlayer : MonsterVsMonster
	}

	// tick_damage is the formula-resolved amount; actual HP loss is not available here
	// since doDamage() is a stub that doesn't return it.
	void EmitConditionTick(const CreaturePtr& creature, ConditionType_t type, int32_t tickDamage) noexcept
	{
		if constexpr (not Metrics::ENABLED or not Metrics::CAPTURE_CONDITIONS)
			return;

		const auto magnitude = static_cast<uint32_t>(std::abs(tickDamage));
		const auto source = Metrics::AccumulateConditionTick(creature->getID(), static_cast<uint8_t>(type), magnitude);

		if (not source)
			return;

		const auto applier = source->applier_id != 0 ? g_game.getCreatureByID(source->applier_id) : nullptr;

		Metrics::ConditionRecord record{};
		record.timestamp_ms = static_cast<uint64_t>(OTSYS_TIME());
		record.instance_guid = source->instance_guid;
		record.applier_id = source->applier_id;
		record.target_id = creature->getID();
		record.source_combat_id = source->source_combat_id;
		record.record_type = std::to_underlying(Metrics::ConditionRecordType::Tick);
		record.condition_type = static_cast<uint8_t>(type);
		record.tick_number = source->tick_count;
		record.tick_damage = magnitude;
		record.running_total = source->running_total;
		record.was_fatal = creature->getHealth() <= 0;
		record.applier_vocation = ConditionMetricsVocationOf(applier);
		record.target_vocation  = ConditionMetricsVocationOf(creature);
		record.situation         = ConditionMetricsSituationOf(applier, creature);

		Metrics::RecordCondition(record);
	}
}

bool ConditionDamage::executeCondition(const CreaturePtr creature, int32_t interval)
{
	if (periodDamage != 0) {
		periodDamageTick += interval;

		if (periodDamageTick >= tickInterval) {
			periodDamageTick = 0;
			if (doDamage(creature, periodDamage))
				BlackTek::EmitConditionTick(creature, getType(), periodDamage);
		}
	} else if (damage_front < damageList.size()) {
		IntervalInfo& damageInfo = damageList[damage_front];

		bool bRemove = (ticks != -1);
		creature->onTickCondition(getType(), bRemove);
		damageInfo.timeLeft -= interval;

		if (damageInfo.timeLeft <= 0) {
			int32_t damage = damageInfo.value;

			if (bRemove) {
				++damage_front;
			} else {
				damageInfo.timeLeft = damageInfo.interval;
			}

			if (doDamage(creature, damage))
				BlackTek::EmitConditionTick(creature, getType(), damage);
		}

		if (!bRemove) {
			if (ticks > 0) {
				endTime += interval;
			}

			interval = 0;
		}
	}

	return Condition::executeCondition(creature, interval);
}

bool ConditionDamage::getNextDamage(int32_t& damage)
{
	if (periodDamage != 0) {
		damage = periodDamage;
		return true;
	} else if (damage_front < damageList.size()) {
		damage = damageList[damage_front].value;
		if (ticks != -1) {
			++damage_front;
		}
		return true;
	}
	return false;
}

bool ConditionDamage::doDamage(CreaturePtr creature, int32_t healthChange) const
{
	if (creature->isSuppress(getType()) or creature->isImmune(getType()))
		return false;
	return true;
}

void ConditionDamage::endCondition(CreaturePtr)
{
	//
}

void ConditionDamage::addCondition(const CreaturePtr creature, const Condition* condition)
{
	if (condition->getType() != conditionType) {
		return;
	}

	if (!updateCondition(condition)) {
		return;
	}

	const ConditionDamage& conditionDamage = static_cast<const ConditionDamage&>(*condition);

	setTicks(condition->getTicks());
	owner = conditionDamage.owner;
	maxDamage = conditionDamage.maxDamage;
	minDamage = conditionDamage.minDamage;
	startDamage = conditionDamage.startDamage;
	tickInterval = conditionDamage.tickInterval;
	periodDamage = conditionDamage.periodDamage;
	int32_t nextTimeLeft = tickInterval;

	if (damage_front < damageList.size()) {
		//save previous timeLeft
		nextTimeLeft = damageList[damage_front].timeLeft;
		damageList.clear();
		damage_front = 0;
	}

	damageList  = conditionDamage.damageList;
	damage_front = conditionDamage.damage_front;

	if (init()) {
		if (damage_front < damageList.size()) {
			//restore last timeLeft
			damageList[damage_front].timeLeft = nextTimeLeft;
		}

		if (!delayed) {
			int32_t damage;
			if (getNextDamage(damage)) {
				doDamage(creature, damage);
			}
		}
	}
}

int32_t ConditionDamage::getTotalDamage() const
{
	int32_t result;
	if (damage_front < damageList.size()) {
		result = 0;
		for (size_t i = damage_front; i < damageList.size(); ++i) {
			result += damageList[i].value;
		}
	} else {
		result = minDamage + (maxDamage - minDamage) / 2;
	}
	return std::abs(result);
}

uint32_t ConditionDamage::getIcons() const
{
	uint32_t icons = Condition::getIcons();
	switch (conditionType) {
		case CONDITION_FIRE:
			icons |= ICON_BURN;
			break;

		case CONDITION_ENERGY:
			icons |= ICON_ENERGY;
			break;

		case CONDITION_DROWN:
			icons |= ICON_DROWNING;
			break;

		case CONDITION_POISON:
			icons |= ICON_POISON;
			break;

		case CONDITION_FREEZING:
			icons |= ICON_FREEZING;
			break;

		case CONDITION_DAZZLED:
			icons |= ICON_DAZZLED;
			break;

		case CONDITION_CURSED:
			icons |= ICON_CURSED;
			break;

		case CONDITION_BLEEDING:
			icons |= ICON_BLEEDING;
			break;

		default:
			break;
	}
	return icons;
}

void ConditionDamage::generateDamageList(int32_t amount, int32_t start, std::vector<int32_t>& list)
{
	amount = std::abs(amount);
	int32_t sum = 0;
	double x1, x2;

	for (int32_t i = start; i > 0; --i) {
		int32_t n = start + 1 - i;
		int32_t med = (n * amount) / start;

		do {
			sum += i;
			list.push_back(i);

			x1 = std::fabs(1.0 - ((static_cast<float>(sum)) + i) / med);
			x2 = std::fabs(1.0 - (static_cast<float>(sum) / med));
		} while (x1 < x2);
	}
}

void ConditionSpeed::setFormulaVars(float mina, float minb, float maxa, float maxb)
{
	this->mina = mina;
	this->minb = minb;
	this->maxa = maxa;
	this->maxb = maxb;
}

bool ConditionSpeed::setParam(ConditionParam_t param, int32_t value)
{
	Condition::setParam(param, value);
	if (param != CONDITION_PARAM_SPEED) {
		return false;
	}

	speedDelta = value;

	if (value > 0) {
		conditionType = CONDITION_HASTE;
	} else {
		conditionType = CONDITION_PARALYZE;
	}
	return true;
}

int32_t ConditionSpeed::getParam(ConditionParam_t param)
{
	switch (param) {
		case CONDITION_PARAM_SPEED:
			return speedDelta;

		default:
			return Condition::getParam(param);
	}
}

bool ConditionSpeed::unserializeProp(ConditionAttr_t attr, PropStream& propStream)
{
	if (attr == CONDITIONATTR_SPEEDDELTA) {
		return propStream.read<int32_t>(speedDelta);
	} else if (attr == CONDITIONATTR_FORMULA_MINA) {
		return propStream.read<float>(mina);
	} else if (attr == CONDITIONATTR_FORMULA_MINB) {
		return propStream.read<float>(minb);
	} else if (attr == CONDITIONATTR_FORMULA_MAXA) {
		return propStream.read<float>(maxa);
	} else if (attr == CONDITIONATTR_FORMULA_MAXB) {
		return propStream.read<float>(maxb);
	}
	return Condition::unserializeProp(attr, propStream);
}

void ConditionSpeed::serialize(PropWriteStream& propWriteStream)
{
	Condition::serialize(propWriteStream);

	propWriteStream.write<uint8_t>(CONDITIONATTR_SPEEDDELTA);
	propWriteStream.write<int32_t>(speedDelta);

	propWriteStream.write<uint8_t>(CONDITIONATTR_FORMULA_MINA);
	propWriteStream.write<float>(mina);

	propWriteStream.write<uint8_t>(CONDITIONATTR_FORMULA_MINB);
	propWriteStream.write<float>(minb);

	propWriteStream.write<uint8_t>(CONDITIONATTR_FORMULA_MAXA);
	propWriteStream.write<float>(maxa);

	propWriteStream.write<uint8_t>(CONDITIONATTR_FORMULA_MAXB);
	propWriteStream.write<float>(maxb);
}

static std::pair<int32_t, int32_t> getFormulaValues(int32_t var, float mina, float minb, float maxa, float maxb)
{
	return {std::fma(var, mina, minb), std::fma(var, maxa, maxb)};
}

bool ConditionSpeed::startCondition(CreaturePtr creature)
{
	if (!Condition::startCondition(creature)) {
		return false;
	}

	if (speedDelta == 0) {
		auto minmax = getFormulaValues(creature->getBaseSpeed(), mina, minb, maxa, maxb);
		speedDelta = uniform_random(minmax.first, minmax.second);
	}

	g_game.changeSpeed(creature, speedDelta);
	return true;
}

bool ConditionSpeed::executeCondition(const CreaturePtr creature, const int32_t interval)
{
	return Condition::executeCondition(creature, interval);
}

void ConditionSpeed::endCondition(const CreaturePtr creature)
{
	g_game.changeSpeed(creature, -speedDelta);
}

void ConditionSpeed::addCondition(const CreaturePtr creature, const Condition* condition)
{
	if (conditionType != condition->getType()) {
		return;
	}

	if (ticks == -1 && condition->getTicks() > 0) {
		return;
	}

	setTicks(condition->getTicks());

	const ConditionSpeed& conditionSpeed = static_cast<const ConditionSpeed&>(*condition);
	int32_t oldSpeedDelta = speedDelta;
	speedDelta = conditionSpeed.speedDelta;
	mina = conditionSpeed.mina;
	maxa = conditionSpeed.maxa;
	minb = conditionSpeed.minb;
	maxb = conditionSpeed.maxb;

	if (speedDelta == 0) {
		auto minmax = getFormulaValues(creature->getBaseSpeed(), mina, minb, maxa, maxb);
		speedDelta = uniform_random(minmax.first, minmax.second);
	}

	int32_t newSpeedChange = (speedDelta - oldSpeedDelta);
	if (newSpeedChange != 0) {
		g_game.changeSpeed(creature, newSpeedChange);
	}
}

uint32_t ConditionSpeed::getIcons() const
{
	uint32_t icons = Condition::getIcons();
	switch (conditionType) {
		case CONDITION_HASTE:
			icons |= ICON_HASTE;
			break;

		case CONDITION_PARALYZE:
			icons |= ICON_PARALYZE;
			break;

		default:
			break;
	}
	return icons;
}

bool ConditionInvisible::startCondition(const CreaturePtr creature)
{
	if (!Condition::startCondition(creature)) {
		return false;
	}

	if (!creature->isInGhostMode()) {
		g_game.internalCreatureChangeVisible(creature, false);
	}
	return true;
}

void ConditionInvisible::endCondition(const CreaturePtr creature)
{
	if (!creature->isInGhostMode() && !creature->isInvisible()) {
		g_game.internalCreatureChangeVisible(creature, true);
	}
}

void ConditionOutfit::setOutfit(const Outfit_t& outfit)
{
	this->outfit = outfit;
}

bool ConditionOutfit::unserializeProp(const ConditionAttr_t attr, PropStream& propStream)
{
	if (attr == CONDITIONATTR_OUTFIT) {
		return propStream.read<Outfit_t>(outfit);
	}
	return Condition::unserializeProp(attr, propStream);
}

void ConditionOutfit::serialize(PropWriteStream& propWriteStream)
{
	Condition::serialize(propWriteStream);

	propWriteStream.write<uint8_t>(CONDITIONATTR_OUTFIT);
	propWriteStream.write<Outfit_t>(outfit);
}

bool ConditionOutfit::startCondition(const CreaturePtr creature)
{
	if (!Condition::startCondition(creature)) {
		return false;
	}

	g_game.internalCreatureChangeOutfit(creature, outfit);
	return true;
}

bool ConditionOutfit::executeCondition(const CreaturePtr creature, const int32_t interval)
{
	return Condition::executeCondition(creature, interval);
}

void ConditionOutfit::endCondition(const CreaturePtr creature)
{
	g_game.internalCreatureChangeOutfit(creature, creature->getDefaultOutfit());
}

void ConditionOutfit::addCondition(const CreaturePtr creature, const Condition* condition)
{
	if (updateCondition(condition)) {
		setTicks(condition->getTicks());

		const auto& conditionOutfit = static_cast<const ConditionOutfit&>(*condition);
		outfit = conditionOutfit.outfit;

		g_game.internalCreatureChangeOutfit(creature, outfit);
	}
}

bool ConditionLight::startCondition(const CreaturePtr creature)
{
	if (!Condition::startCondition(creature)) {
		return false;
	}

	internalLightTicks = 0;
	lightChangeInterval = ticks / lightInfo.level;
	creature->setCreatureLight(lightInfo);
	g_game.changeLight(creature);
	return true;
}

bool ConditionLight::executeCondition(const CreaturePtr creature, const int32_t interval)
{
	internalLightTicks += interval;

	if (internalLightTicks >= lightChangeInterval) {
		internalLightTicks = 0;
		LightInfo lightInfo = creature->getCreatureLight();

		if (lightInfo.level > 0) {
			--lightInfo.level;
			creature->setCreatureLight(lightInfo);
			g_game.changeLight(creature);
		}
	}

	return Condition::executeCondition(creature, interval);
}

void ConditionLight::endCondition(const CreaturePtr creature)
{
	creature->setNormalCreatureLight();
	g_game.changeLight(creature);
}

void ConditionLight::addCondition(const CreaturePtr creature, const Condition* condition)
{
	if (updateCondition(condition)) {
		setTicks(condition->getTicks());

		const auto& conditionLight = static_cast<const ConditionLight&>(*condition);
		lightInfo.level = conditionLight.lightInfo.level;
		lightInfo.color = conditionLight.lightInfo.color;
		lightChangeInterval = ticks / lightInfo.level;
		internalLightTicks = 0;
		creature->setCreatureLight(lightInfo);
		g_game.changeLight(creature);
	}
}

bool ConditionLight::setParam(const ConditionParam_t param, const int32_t value)
{
	if (const bool ret = Condition::setParam(param, value)) {
		return ret;
	}

	switch (param) {
		case CONDITION_PARAM_LIGHT_LEVEL:
			lightInfo.level = value;
			return true;

		case CONDITION_PARAM_LIGHT_COLOR:
			lightInfo.color = value;
			return true;

		default:
			return false;
	}
}

int32_t ConditionLight::getParam(ConditionParam_t param)
{
	switch (param) {
		case CONDITION_PARAM_LIGHT_LEVEL:
			return lightInfo.level;

		case CONDITION_PARAM_LIGHT_COLOR:
			return lightInfo.color;

		default:
			return Condition::getParam(param);
	}
}

bool ConditionLight::unserializeProp(const ConditionAttr_t attr, PropStream& propStream)
{
	if (attr == CONDITIONATTR_LIGHTCOLOR) {
		uint32_t value;
		if (!propStream.read<uint32_t>(value)) {
			return false;
		}

		lightInfo.color = value;
		return true;
	} else if (attr == CONDITIONATTR_LIGHTLEVEL) {
		uint32_t value;
		if (!propStream.read<uint32_t>(value)) {
			return false;
		}

		lightInfo.level = value;
		return true;
	} else if (attr == CONDITIONATTR_LIGHTTICKS) {
		return propStream.read<uint32_t>(internalLightTicks);
	} else if (attr == CONDITIONATTR_LIGHTINTERVAL) {
		return propStream.read<uint32_t>(lightChangeInterval);
	}
	return Condition::unserializeProp(attr, propStream);
}

void ConditionLight::serialize(PropWriteStream& propWriteStream)
{
	Condition::serialize(propWriteStream);

	// TODO: color and level could be serialized as 8-bit if we can retain backwards
	// compatibility, but perhaps we should keep it like this in case they increase
	// in the future...
	propWriteStream.write<uint8_t>(CONDITIONATTR_LIGHTCOLOR);
	propWriteStream.write<uint32_t>(lightInfo.color);

	propWriteStream.write<uint8_t>(CONDITIONATTR_LIGHTLEVEL);
	propWriteStream.write<uint32_t>(lightInfo.level);

	propWriteStream.write<uint8_t>(CONDITIONATTR_LIGHTTICKS);
	propWriteStream.write<uint32_t>(internalLightTicks);

	propWriteStream.write<uint8_t>(CONDITIONATTR_LIGHTINTERVAL);
	propWriteStream.write<uint32_t>(lightChangeInterval);
}

void ConditionSpellCooldown::addCondition(const CreaturePtr creature, const Condition* condition)
{
	if (updateCondition(condition)) {
		setTicks(condition->getTicks());

		if (subId != 0 && ticks > 0) {
			if (const auto player = creature->getPlayer()) {
				player->sendSpellCooldown(subId, ticks);
			}
		}
	}
}

bool ConditionSpellCooldown::startCondition(const CreaturePtr creature)
{
	if (!Condition::startCondition(creature)) {
		return false;
	}

	if (subId != 0 && ticks > 0) {
		if (const auto player = creature->getPlayer()) {
			player->sendSpellCooldown(subId, ticks);
		}
	}
	return true;
}

void ConditionSpellGroupCooldown::addCondition(const CreaturePtr creature, const Condition* condition)
{
	if (updateCondition(condition)) {
		setTicks(condition->getTicks());

		if (subId != 0 && ticks > 0) {
			if (const auto player = creature->getPlayer()) {
				player->sendSpellGroupCooldown(static_cast<SpellGroup_t>(subId), ticks);
			}
		}
	}
}

bool ConditionSpellGroupCooldown::startCondition(const CreaturePtr creature)
{
	if (!Condition::startCondition(creature)) {
		return false;
	}

	if (subId != 0 && ticks > 0) {
		if (const auto player = creature->getPlayer()) {
			player->sendSpellGroupCooldown(static_cast<SpellGroup_t>(subId), ticks);
		}
	}
	return true;
}

bool ConditionDrunk::startCondition(const CreaturePtr creature)
{
	if (!Condition::startCondition(creature)) {
		return false;
	}

	creature->setDrunkenness(drunkenness);
	return true;
}

bool ConditionDrunk::updateCondition(const Condition* addCondition)
{
	const ConditionDrunk* conditionDrunk = static_cast<const ConditionDrunk*>(addCondition);
	return conditionDrunk->drunkenness > drunkenness;
}

void ConditionDrunk::addCondition(const CreaturePtr creature, const Condition* condition)
{
	if (!updateCondition(condition)) {
		return;
	}

	const ConditionDrunk* conditionDrunk = static_cast<const ConditionDrunk*>(condition);
	setTicks(conditionDrunk->getTicks());
	creature->setDrunkenness(conditionDrunk->drunkenness);
}

void ConditionDrunk::endCondition(const CreaturePtr creature)
{
	creature->setDrunkenness(0);
}

uint32_t ConditionDrunk::getIcons() const
{
	return ICON_DRUNK;
}

bool ConditionDrunk::setParam(const ConditionParam_t param, const int32_t value)
{
	bool ret = Condition::setParam(param, value);

	switch (param) {
		case CONDITION_PARAM_DRUNKENNESS: {
			drunkenness = value;
			return true;
		}

		default: {
			return ret;
		}
	}
}
