// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_VOCATION_H
#define FS_VOCATION_H

#include "enums.h"
#include "item.h"
#include "skills.h"

extern gtl::flat_hash_map<uint16_t, SkillRegistry> vocation_skills;

struct DualWieldConfig
{
	bool     enabled             = false;
	float    primaryMultiplier   = 1.0f;
	float    secondaryMultiplier = 1.0f;
	uint32_t delay               = 300;
	// Bitmask of WeaponType_t values permitted in each dual-wield slot.
	// 0 = no restriction (all types allowed); otherwise (1u << WeaponType_t) must be set.
	uint16_t allowed_weapon_mask = 0;
	// When true AND WEAPON_DISTANCE is permitted, only throwables (ammoType == AMMO_NONE) qualify.
	bool     throwable_only           = false;
	// Shield in off-hand: on BLOCK_DEFENSE, fire a counter-attack scaled by shield skill + defense.
	// 0 = disabled; set > 0 to enable the parry counter.
	float    parry_counter_multiplier = 0.0f;
	uint32_t parry_counter_delay      = 200;
};

class Vocation
{
	public:
		explicit Vocation(uint16_t id) : id(id) {}

		const std::string& getVocName() const {
			return name;
		}
	
		const std::string& getVocDescription() const {
			return description;
		}

		uint64_t getReqSkillTries(uint8_t skill, uint16_t level) const;
		uint64_t getReqMana(uint32_t magLevel) const;

		uint16_t getId() const {
			return id;
		}

		uint8_t getClientId() const {
			return clientId;
		}

		uint32_t getHPGain() const {
			return gainHP;
		}
	
		uint32_t getManaGain() const {
			return gainMana;
		}
	
		uint32_t getCapGain() const {
			return gainCap;
		}

		uint32_t getManaGainTicks() const {
			return gainManaTicks;
		}
	
		uint32_t getManaGainAmount() const {
			return gainManaAmount;
		}

		uint32_t getSoulGainAmount() const {
			return gainSoulAmount;
		}
	
		uint32_t getHealthGainTicks() const {
			return gainHealthTicks;
		}
	
		uint32_t getHealthGainAmount() const {
			return gainHealthAmount;
		}

		uint8_t getSoulMax() const {
			return soulMax;
		}
	
		uint16_t getSoulGainTicks() const {
			return gainSoulTicks;
		}

		uint32_t getAttackSpeed() const {
			return attackSpeed;
		}

		uint32_t getBaseSpeed() const {
			return baseSpeed;
		}

		uint32_t getDefenseChargeInterval() const {
			return defenseChargeInterval;
		}

		uint32_t getDefenseChargesCap() const {
			return defenseChargesCap;
		}

		uint32_t getArmorChargesCap() const {
			return armorChargesCap;
		}

		uint32_t getDefModifierChargesCap() const {
			return defModifierChargesCap;
		}

		uint32_t getAtkModifierChargesCap() const {
			return atkModifierChargesCap;
		}

		float getDefenseChargeCostMultiplier() const {
			return defenseChargeCostMultiplier;
		}

		float getArmorChargeCostMultiplier() const {
			return armorChargeCostMultiplier;
		}

		float getDefModifierChargeCostMultiplier() const {
			return defModifierChargeCostMultiplier;
		}

		float getAtkModifierChargeCostMultiplier() const {
			return atkModifierChargeCostMultiplier;
		}

		uint32_t getFromVocation() const {
			return fromVocation;
		}

		uint32_t getNoPongKickTime() const {
			return noPongKickTime;
		}

		bool allowsPvp() const {
			return allowPvp;
		}

		float meleeDamageMultiplier = 1.0f;
		float distDamageMultiplier = 1.0f;
		float defenseMultiplier = 1.0f;
		float armorMultiplier = 1.0f;
		DualWieldConfig dualWield{};

	private:
		friend class Vocations;

		std::string name = "none";
		std::string description;

		double skillMultipliers[SKILL_LAST + 1] = {1.5, 2.0, 2.0, 2.0, 2.0, 1.5, 1.1};
		float manaMultiplier = 4.0f;

		uint32_t gainHealthTicks = 6;
		uint32_t gainHealthAmount = 1;
		uint32_t gainManaTicks = 6;
		uint32_t gainManaAmount = 1;
		uint32_t gainSoulAmount = 1;
		uint32_t gainCap = 500;
		uint32_t gainMana = 5;
		uint32_t gainHP = 5;
		uint32_t fromVocation = VOCATION_NONE;
		uint32_t attackSpeed = 1500;
		uint32_t baseSpeed = 220;
		uint32_t noPongKickTime = 60000;
		uint32_t defenseChargeInterval = 0;
		uint32_t defenseChargesCap = 0;
		uint32_t armorChargesCap = 0;
		uint32_t defModifierChargesCap = 0;
		uint32_t atkModifierChargesCap = 0;

		float defenseChargeCostMultiplier = 1.0f;
		float armorChargeCostMultiplier = 1.0f;
		float defModifierChargeCostMultiplier = 1.0f;
		float atkModifierChargeCostMultiplier = 1.0f;

		uint16_t id;
		uint16_t gainSoulTicks = 120;

		uint8_t soulMax = 100;
		uint8_t clientId = 0;

		bool allowPvp = true;
};

using VocationMap = std::map<uint16_t, Vocation>;

class Vocations
{
	public:
		bool loadFromToml();
		Vocation* getVocation(uint16_t id);
		int32_t getVocationId(std::string_view name) const;
		uint16_t getPromotedVocation(uint16_t vocationId) const;
		const VocationMap& getVocations() const { return vocationsMap; }
		static bool addVocationSkill(uint32_t vocationId, std::string_view name, const std::shared_ptr<CustomSkill>& skill);
		static std::optional<std::shared_ptr<CustomSkill>> getVocationSkill(std::string_view name, uint16_t vocation_id);
		SkillRegistry getRegisteredSkills(uint16_t vocation_id);

	private:
		VocationMap vocationsMap;
		static constexpr auto folder = "data/vocations/";
};

#endif
