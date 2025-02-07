// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_WEAPONS_H
#define FS_WEAPONS_H

#include "player.h"
#include "baseevents.h"
#include "combat.h"
#include "const.h"
#include "vocation.h"

extern Vocations g_vocations;

class Weapon;
class WeaponMelee;
class WeaponDistance;
class WeaponWand;

using Weapon_ptr = std::unique_ptr<Weapon>;

class Weapons final : public BaseEvents
{
	public:
		Weapons();
		~Weapons();

		// non-copyable
		Weapons(const Weapons&) = delete;
		Weapons& operator=(const Weapons&) = delete;

		void loadDefaults();
		const Weapon* getWeapon(const ItemConstPtr& item) const;

		static int32_t getMaxMeleeDamage(int32_t attackSkill, int32_t attackValue);
		static int32_t getMaxWeaponDamage(uint32_t level, int32_t attackSkill, int32_t attackValue, float attackFactor);

		bool registerLuaEvent(Weapon* event);
		void clear(bool fromLua) override final;

	private:
		LuaScriptInterface& getScriptInterface() override;
		std::string_view getScriptBaseName() const override { return "weapons"; }
		Event_ptr getEvent(const std::string& nodeName) override;
		bool registerEvent(Event_ptr event, const pugi::xml_node& node) override;

		std::map<uint32_t, Weapon*> weapons;

		LuaScriptInterface scriptInterface { "Weapon Interface" };
};

class Weapon : public Event
{
	public:
		explicit Weapon(LuaScriptInterface* interface) : Event(interface) {}

		bool configureEvent(const pugi::xml_node& node) override;
		bool loadFunction(const pugi::xml_attribute&, bool) final {
			return true;
		}
		virtual void configureWeapon(const ItemType& it);
		virtual bool interruptSwing() const {
			return false;
		}
		bool ammoCheck(const PlayerConstPtr& player) const;

		int32_t playerWeaponCheck(const PlayerConstPtr& player,const CreatureConstPtr& target, uint8_t shootRange) const;
		static bool useFist(const PlayerPtr& player, const CreaturePtr& target);
		virtual bool useWeapon(PlayerPtr player, ItemPtr item, CreaturePtr target) const;

		virtual int32_t getWeaponDamage(const PlayerConstPtr& player, const CreatureConstPtr& target, const ItemConstPtr& item, bool maxDamage = false) const = 0;
		virtual int32_t getElementDamage(const PlayerConstPtr& player, const CreatureConstPtr& target, const ItemConstPtr& item) const = 0;
		virtual CombatType_t getElementType() const = 0;

		uint16_t getID() const {
			return id;
		}
	
		void setID(uint16_t newId) {
			id = newId;
		}

		uint32_t getReqLevel() const {
			return level;
		}
	
		void setRequiredLevel(uint32_t reqlvl) {
			level = reqlvl;
		}

		uint32_t getReqMagLv() const {
			return magLevel;
		}
	
		void setRequiredMagLevel(uint32_t reqlvl) {
			magLevel = reqlvl;
		}

		bool isPremium() const {
			return premium;
		}
	
		void setNeedPremium(bool prem) {
			premium = prem;
		}

		bool isWieldedUnproperly() const {
			return wieldUnproperly;
		}
	
		void setWieldUnproperly(bool unproperly) {
			wieldUnproperly = unproperly;
		}

		uint32_t getMana() const {
			return mana;
		}
	
		void setMana(uint32_t m) {
			mana = m;
		}

		uint32_t getManaPercent() const {
			return manaPercent;
		}
	
		void setManaPercent(uint32_t m) {
			manaPercent = m;
		}

		int32_t getHealth() const {
			return health;
		}
	
		void setHealth(int32_t h) {
			health = h;
		}

		uint32_t getHealthPercent() const {
			return healthPercent;
		}
	
		void setHealthPercent(uint32_t m) {
			healthPercent = m;
		}

		uint32_t getSoul() const {
			return soul;
		}
	
		void setSoul(uint32_t s) {
			soul = s;
		}

		uint8_t getBreakChance() const {
			return breakChance;
		}
	
		void setBreakChance(uint8_t b) {
			breakChance = b;
		}

		bool isEnabled() const {
			return enabled;
		}
	
		void setIsEnabled(bool e) {
			enabled = e;
		}

		uint32_t getWieldInfo() const {
			return wieldInfo;
		}
	
		void setWieldInfo(uint32_t info) {
			wieldInfo |= info;
		}

		void addVocWeaponMap(const std::string& vocName) {
			int32_t vocationId = g_vocations.getVocationId(vocName);
			if (vocationId != -1) {
				vocWeaponMap[vocationId] = true;
			}
		}

		const std::string& getVocationString() const {
			return vocationString;
		}
	
		void setVocationString(const std::string& str) {
			vocationString = str;
		}

		WeaponAction_t action = WEAPONACTION_NONE;
		CombatParams params;
		WeaponType_t weaponType;
		std::map<uint16_t, bool> vocWeaponMap;

	protected:
		void internalUseWeapon(const PlayerPtr& player, const ItemPtr& item, const CreaturePtr& target, int32_t damageModifier) const;
		void internalUseWeapon(const PlayerPtr& player, const ItemPtr& item, const TilePtr& tile) const;

		uint16_t id = 0;

	private:
		virtual bool getSkillType(const PlayerConstPtr&, const ItemConstPtr&, skills_t&, uint32_t&) const {
			return false;
		}

		uint32_t getManaCost(const PlayerConstPtr& player) const;
		int32_t getHealthCost(const PlayerConstPtr& player) const;

		uint32_t level = 0;
		uint32_t magLevel = 0;
		uint32_t mana = 0;
		uint32_t manaPercent = 0;
		uint32_t health = 0;
		uint32_t healthPercent = 0;
		uint32_t soul = 0;
		uint32_t wieldInfo = WIELDINFO_NONE;
		uint8_t breakChance = 0;
		bool enabled = true;
		bool premium = false;
		bool wieldUnproperly = false;
		std::string vocationString = "";

		std::string_view getScriptEventName() const override final { return "onUseWeapon"; }

		bool executeUseWeapon(const PlayerPtr& player, const LuaVariant& var) const;
		void onUsedWeapon(const PlayerPtr& player, const ItemPtr& item, const TilePtr& destTile) const;

		static void decrementItemCount(const ItemPtr& item);

		friend class Combat;
};

class WeaponMelee final : public Weapon
{
	public:
		explicit WeaponMelee(LuaScriptInterface* interface);

		void configureWeapon(const ItemType& it) override;

		bool useWeapon(const PlayerPtr player, const ItemPtr item, const CreaturePtr target) const override;

		int32_t getWeaponDamage(const PlayerConstPtr& player, const CreatureConstPtr& target, const ItemConstPtr& item, bool maxDamage = false) const override;
		int32_t getElementDamage(const PlayerConstPtr& player, const CreatureConstPtr& target, const ItemConstPtr& item) const override;
		CombatType_t getElementType() const override { return elementType; }

	private:
		bool getSkillType(const PlayerConstPtr& player, const ItemConstPtr& item, skills_t& skill, uint32_t& skillpoint) const override;

		CombatType_t elementType = COMBAT_NONE;
		uint16_t elementDamage = 0;
};

class WeaponDistance final : public Weapon
{
	public:
		explicit WeaponDistance(LuaScriptInterface* interface);

		void configureWeapon(const ItemType& it) override;
		bool interruptSwing() const override {
			return true;
		}

		bool useWeapon(PlayerPtr player, ItemPtr item, CreaturePtr target) const override;

		int32_t getWeaponDamage(const PlayerConstPtr& player, const CreatureConstPtr& target, const ItemConstPtr& item, bool maxDamage = false) const override;
		int32_t getElementDamage(const PlayerConstPtr& player, const CreatureConstPtr& target, const ItemConstPtr& item) const override;
		CombatType_t getElementType() const override { return elementType; }

	private:
		bool getSkillType(const PlayerConstPtr& player, const ItemConstPtr& item, skills_t& skill, uint32_t& skillpoint) const override;

		CombatType_t elementType = COMBAT_NONE;
		uint16_t elementDamage = 0;
};

class WeaponWand final : public Weapon
{
	public:
		explicit WeaponWand(LuaScriptInterface* interface) : Weapon(interface) {}

		bool configureEvent(const pugi::xml_node& node) override;
		void configureWeapon(const ItemType& it) override;

		int32_t getWeaponDamage(const PlayerConstPtr& player, const CreatureConstPtr& target, const ItemConstPtr& item, bool maxDamage = false) const override;
		int32_t getElementDamage(const PlayerConstPtr&, const CreatureConstPtr&, const ItemConstPtr&) const override { return 0; }
		CombatType_t getElementType() const override { return COMBAT_NONE; }

		void setMinChange(int32_t change) {
			minChange = change;
		}

		void setMaxChange(int32_t change) {
			maxChange = change;
		}

	private:
		bool getSkillType(const PlayerConstPtr&, const ItemConstPtr&, skills_t&, uint32_t&) const override {
			return false;
		}

		int32_t minChange = 0;
		int32_t maxChange = 0;
};

#endif
