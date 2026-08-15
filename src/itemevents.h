// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_ITEMEVENTS_H
#define FS_ITEMEVENTS_H

#include "baseevents.h"
#include "combat.h"
#include "const.h"
#include "gamemodel.h"
#include "item.h"
#include "luascript.h"
#include "luavariant.h"
#include "vocation.h"

#include <algorithm>
#include <array>
#include <limits>
#include <optional>

#include <gtl/phmap.hpp>

extern Vocations g_vocations;

using VocEquipList = std::vector<uint16_t>;	// sorted, unique

namespace BlackTek { class Augment; struct DamageModifier; }

class ItemEvent;

namespace BlackTek::ItemEvents
{
	enum class HookType : uint8_t
	{
		OnUse,
		OnUseAsWeapon,
		OnEquip,
		OnDeEquip,
		OnStepOn,
		OnStepOff,
		OnAddItem,
		OnRemoveItem,
		OnAttack,
		OnDefend,
		OnAugment,
		OnRemoveAugment,
		OnAttackMod,
		OnDefenseMod,
		Last
	};

	using HookCounts = std::array<uint32_t, static_cast<size_t>(HookType::Last)>;

	[[nodiscard]] inline constexpr bool IsSlotSelected(HookType hook) noexcept
	{
		return hook == HookType::OnEquip or hook == HookType::OnDeEquip;
	}

	inline constexpr size_t HookCount = static_cast<size_t>(HookType::Last);

	using HookMask = uint16_t;

	static_assert(HookCount <= std::numeric_limits<HookMask>::digits, "HookMask too narrow for HookType::Last");

	[[nodiscard]] inline constexpr HookMask ToHookMask(HookType hook) noexcept
	{
		return static_cast<HookMask>(1u << std::to_underlying(hook));
	}

	inline constexpr HookMask CombatHookMask = ToHookMask(HookType::OnAttack) | ToHookMask(HookType::OnDefend)
		| ToHookMask(HookType::OnAttackMod) | ToHookMask(HookType::OnDefenseMod);

	class HookMaskTable
	{
		public:
			[[nodiscard]] bool Test(uint16_t id, HookType hook) const noexcept
			{
				return id < masks.size() and (masks[id] & ToHookMask(hook)) != 0;
			}

			void Set(uint16_t id, HookType hook);
			void Clear() noexcept { masks.clear(); }

		private:
			std::vector<HookMask> masks;
	};

	inline constexpr uint32_t AnySlot = SLOTP_WHEREEVER;

	[[nodiscard]] inline constexpr uint64_t PackKey(uint32_t key, HookType hook) noexcept
	{
		return (static_cast<uint64_t>(key) << 4) | std::to_underlying(hook);
	}

	[[nodiscard]] inline constexpr uint64_t PackKey(const Position& pos, HookType hook) noexcept
	{
		return (static_cast<uint64_t>(pos.z) << 36) | (static_cast<uint64_t>(pos.y) << 20)
			| (static_cast<uint64_t>(pos.x) << 4) | std::to_underlying(hook);
	}

	struct EventRef
	{
		const ItemEvent* event = nullptr;
		uint32_t select_mask = AnySlot;
	};

	using EventRefs = std::vector<EventRef>;

	[[nodiscard]] const ItemEvent* SelectEvent(const EventRefs& refs, uint32_t probe) noexcept;

	class RefIndex
	{
		public:
			[[nodiscard]] const EventRefs* Find(uint64_t key) const noexcept;
			void Add(uint64_t key, EventRef ref);
			void Clear() noexcept;

		private:
			gtl::flat_hash_map<uint64_t, EventRefs> entries;
	};
}

using ItemEvent_ptr = std::unique_ptr<ItemEvent>;

using ItemEventStepFunction = uint32_t (*)(const CreaturePtr& creature, const ItemPtr& item, const Position& pos);
using ItemEventMoveFunction = uint32_t (*)(const ItemPtr& item, const ItemPtr& tileItem, const Position& pos);
using ItemEventEquipFunction = ReturnValue (*)(const ItemEvent* itemEvent, const PlayerPtr& player, const ItemPtr& item, slots_t slot, bool isCheck);
using ItemEventActionFunction = bool (*)(PlayerPtr player, ItemPtr item, const Position& fromPosition, BlackTek::GameModel target, const Position& toPosition, bool isHotkey);

namespace BlackTek::ItemEvents::Defaults
{
	bool EnterMarket(PlayerPtr player, ItemPtr item, const Position& fromPosition, BlackTek::GameModel target, const Position& toPosition, bool isHotkey);

	uint32_t StepOnField(const CreaturePtr& creature, const ItemPtr& item, const Position& pos);
	uint32_t StepOffField(const CreaturePtr& creature, const ItemPtr& item, const Position& pos);
	uint32_t AddItemField(const ItemPtr& item, const ItemPtr& tileItem, const Position& pos);
	uint32_t RemoveItemField(const ItemPtr& item, const ItemPtr& tileItem, const Position& pos);

	ReturnValue EquipItem(const ItemEvent* itemEvent, const PlayerPtr& player, const ItemPtr& item, slots_t slot, bool isCheck);
	ReturnValue DeEquipItem(const ItemEvent* itemEvent, const PlayerPtr& player, const ItemPtr& item, slots_t slot, bool isCheck);
}

class ItemEvent final : public Event
{
	public:
		struct Register
		{
			enum class Type : uint8_t
			{
				ItemId,
				ActionId,
				UniqueId,
				Position,

				Sword,
				Club,
				Axe,
				Shield,
				Distance,
				Wand,
				Ammo,

				Any,
			};
		};

		struct WeaponConfig
		{
			WeaponType_t weaponType = WEAPON_NONE;
			bool weaponTypeSet = false;
			BlackTek::CombatHandle combat;

			uint32_t mana = 0;
			uint32_t manaPercent = 0;
			uint32_t health = 0;
			uint32_t healthPercent = 0;
			uint32_t soul = 0;
			int32_t breakChance = 0;
			int32_t hitChance = -1;
			int32_t maxHitChance = -1;
			bool wieldUnproperly = false;

			int32_t attack = 0;
			bool attackSet = false;
			int32_t defense = 0;
			int32_t extraDefense = 0;
			bool defenseSet = false;
			uint8_t range = 0;
			bool rangeSet = false;
			uint32_t charges = 0;
			bool showCharges = true;
			bool chargesSet = false;
			uint32_t duration = 0;
			bool showDuration = true;
			bool durationSet = false;
			int32_t decayTo = -1;
			bool decayToSet = false;
			uint16_t transformEquipTo = 0;
			uint16_t transformDeEquipTo = 0;
			Ammo_t ammoType = AMMO_NONE;
			bool ammoTypeSet = false;
			ShootType_t shootType = CONST_ANI_NONE;
			bool shootTypeSet = false;

			WeaponAction_t action = WEAPONACTION_NONE;

			bool twoHanded = false;
			bool slotTypeSet = false;

			int32_t extraElementDamage = 0;
			CombatType_t extraElementType = COMBAT_NONE;
			bool extraElementSet = false;

			int32_t minChange = 0;
			int32_t maxChange = 0;
		};

		explicit ItemEvent(LuaScriptInterface* interface);

		ItemEvent(const ItemEvent&) = delete;
		ItemEvent& operator=(const ItemEvent&) = delete;

		BlackTek::ItemEvents::HookType hook = BlackTek::ItemEvents::HookType::Last;

		std::vector<uint16_t> itemIds;
		std::vector<uint16_t> actionIds;
		std::vector<uint16_t> uniqueIds;
		std::vector<Position> positions;
		std::vector<Register::Type> categories;

		bool allowFarUse = false;
		bool checkFloor = true;
		bool checkLineOfSight = true;

		uint32_t requiredLevel = 0;
		uint32_t requiredMagicLevel = 0;
		bool premium = false;
		uint32_t slot = SLOTP_WHEREEVER;
		VocEquipList vocationEquipMap;
		std::string vocationString;
		uint32_t wieldInfo = 0;
		bool tileItem = false;

		ItemEventStepFunction stepFunction = nullptr;
		ItemEventMoveFunction moveFunction = nullptr;
		ItemEventEquipFunction equipFunction = nullptr;
		ItemEventActionFunction actionFunction = nullptr;

		std::optional<WeaponConfig> weaponConfig;

		void addVocEquipMap(const std::string& vocName)
		{
			const int32_t vocationId = g_vocations.getVocationId(vocName);
			if (vocationId == -1)
			{
				return;
			}

			const auto id = static_cast<uint16_t>(vocationId);
			if (const auto it = std::ranges::lower_bound(vocationEquipMap, id); it == vocationEquipMap.end() or *it != id)
			{
				vocationEquipMap.insert(it, id);
			}
		}

		[[nodiscard]] bool HasVocation(uint16_t vocationId) const noexcept
		{
			return std::ranges::binary_search(vocationEquipMap, vocationId);
		}

		bool executeUse(const PlayerPtr& player, const ItemPtr& item, const Position& fromPosition, const BlackTek::GameModel& target, const Position& toPosition, bool isHotkey) const;
		bool executeUseAsWeapon(const PlayerPtr& player, const ItemPtr& item, const LuaVariant& var) const;
		bool executeEquip(const PlayerPtr& player, const ItemPtr& item, slots_t slot, bool isCheck) const;
		bool executeStep(const CreaturePtr& creature, const ItemPtr& item, const Position& pos) const;
		bool executeAddRemItem(const ItemPtr& item, const ItemPtr& tileItem, const Position& pos) const;
		void executeAugment(const ItemPtr& item, const std::shared_ptr<BlackTek::Augment>& augment) const;
		void executeRemoveAugment(const ItemPtr& item, const std::shared_ptr<BlackTek::Augment>& augment) const;

		void executeAttack(const ItemPtr& item, const PlayerPtr& holder, const CreaturePtr& other, BlockType_t blockType, CombatType_t combatType, BlackTek::Combat::Origin origin, bool criticalDamage, bool leechedDamage) const;
		void executeAttackMod(const ItemPtr& item, const PlayerPtr& holder, const CreaturePtr& other, const std::shared_ptr<BlackTek::DamageModifier>& modifier, uint32_t damage) const;

	protected:
		std::string_view getScriptEventName() const override;
};

class ItemEvents final : public BaseEvents
{
	public:
		ItemEvents();
		~ItemEvents() override;

		ItemEvents(const ItemEvents&) = delete;
		ItemEvents& operator=(const ItemEvents&) = delete;

		bool registerLuaEvent(ItemEvent* event);
		void clear(bool fromLua) override;
		bool reload() override;

		bool fireUse(const PlayerPtr& player, const ItemPtr& item, const Position& fromPosition, const BlackTek::GameModel& target, const Position& toPosition, bool isHotkey);
		ReturnValue fireEquip(const PlayerPtr& player, const ItemPtr& item, slots_t slot, bool isCheck);
		ReturnValue fireDeEquip(const PlayerPtr& player, const ItemPtr& item, slots_t slot);
		uint32_t fireStep(const CreaturePtr& creature, const ItemPtr& item, const Position& pos, BlackTek::ItemEvents::HookType type);
		uint32_t fireAddRemItem(const ItemPtr& item, const ItemPtr& tileItem, const Position& pos, BlackTek::ItemEvents::HookType type);
		bool fireUseAsWeapon(const PlayerPtr& player, const ItemPtr& item, const LuaVariant& var);

		[[nodiscard]] bool hasHook(BlackTek::ItemEvents::HookType type) const noexcept
		{
			return hook_counts[static_cast<size_t>(type)] != 0;
		}
		[[nodiscard]] BlackTek::ItemEvents::HookMask getCombatHookMask(const ItemConstPtr& item) noexcept;
		void fireAttack(const ItemPtr& item, const PlayerPtr& holder, const CreaturePtr& defender, BlockType_t blockType, CombatType_t combatType, BlackTek::Combat::Origin origin, bool criticalDamage, bool leechedDamage);
		void fireDefend(const ItemPtr& item, const PlayerPtr& holder, const CreaturePtr& attacker, BlockType_t blockType, CombatType_t combatType, BlackTek::Combat::Origin origin, bool criticalDamage, bool leechedDamage);
		void fireAttackMod(const ItemPtr& item, const PlayerPtr& holder, const CreaturePtr& defender, const std::shared_ptr<BlackTek::DamageModifier>& modifier, uint32_t damage);
		void fireDefenseMod(const ItemPtr& item, const PlayerPtr& holder, const CreaturePtr& attacker, const std::shared_ptr<BlackTek::DamageModifier>& modifier, uint32_t damage);

		[[nodiscard]] bool hasWeaponBehavior(const ItemConstPtr& item);
		bool useAsWeapon(const PlayerPtr& player, const ItemPtr& item, const CreaturePtr& target);
		[[nodiscard]] bool ammoCheck(const ItemConstPtr& item, const PlayerConstPtr& player);
		[[nodiscard]] static bool interruptSwing(const ItemConstPtr& item);
		static bool useFist(const PlayerPtr& player, const CreaturePtr& target);
		[[nodiscard]] static int32_t getMaxMeleeDamage(int32_t attackSkill, int32_t attackValue) noexcept;
		[[nodiscard]] static int32_t getMaxWeaponDamage(uint32_t level, int32_t attackSkill, int32_t attackValue, float attackFactor) noexcept;

		uint32_t onCreatureMove(const CreaturePtr& creature, const TilePtr& tile, BlackTek::ItemEvents::HookType type);
		uint32_t onItemMove(const ItemPtr& item, const TilePtr& tile, bool isAdd);

		void fireAugment(const ItemPtr& item, const std::shared_ptr<BlackTek::Augment>& augment);
		void fireRemoveAugment(const ItemPtr& item, const std::shared_ptr<BlackTek::Augment>& augment);

		bool useItem(PlayerPtr player, const Position& pos, uint8_t index, const ItemPtr& item, bool isHotkey);
		bool useItemEx(const PlayerPtr& player, const Position& fromPos, const Position& toPos, uint8_t toStackPos, const ItemPtr& item, bool isHotkey, const CreaturePtr& creature = nullptr);
		[[nodiscard]] ReturnValue canUse(const PlayerConstPtr& player, const Position& pos);
		[[nodiscard]] ReturnValue canUse(const PlayerConstPtr& player, const Position& pos, const ItemConstPtr& item);
		[[nodiscard]] ReturnValue canUseFar(const CreatureConstPtr& creature, const Position& toPos, bool checkLineOfSight, bool checkFloor);

	private:
		std::vector<std::unique_ptr<ItemEvent>> event_store;
		std::vector<std::unique_ptr<ItemEvent>> retired_events;

		BlackTek::ItemEvents::RefIndex item_refs;
		BlackTek::ItemEvents::RefIndex tile_item_refs;
		BlackTek::ItemEvents::RefIndex action_refs;
		BlackTek::ItemEvents::RefIndex unique_refs;
		BlackTek::ItemEvents::RefIndex position_refs;
		BlackTek::ItemEvents::RefIndex category_refs;

		BlackTek::ItemEvents::HookMaskTable item_hook_masks;
		BlackTek::ItemEvents::HookMaskTable tile_item_hook_masks;

		BlackTek::ItemEvents::HookCounts hook_counts{};

		BlackTek::ItemEvents::HookCounts category_hook_counts{};
		BlackTek::ItemEvents::HookCounts item_hook_counts{};
		BlackTek::ItemEvents::HookCounts tile_item_hook_counts{};
		BlackTek::ItemEvents::HookCounts action_hook_counts{};
		BlackTek::ItemEvents::HookCounts unique_hook_counts{};
		BlackTek::ItemEvents::HookCounts position_hook_counts{};

		LuaScriptInterface scriptInterface { "ItemEvent Interface" };

		[[nodiscard]] const ItemEvent* getEvent(const ItemConstPtr& item, BlackTek::ItemEvents::HookType hook, slots_t slot = CONST_SLOT_WHEREEVER);
		[[nodiscard]] const ItemEvent* getPositionEvent(const Position& pos, BlackTek::ItemEvents::HookType hook);
		[[nodiscard]] const ItemEvent* getTileItemEvent(const ItemConstPtr& item, BlackTek::ItemEvents::HookType hook);

		bool useMeleeWeapon(const ItemEvent* event, const PlayerPtr& player, const ItemPtr& item, const CreaturePtr& target);
		bool useDistanceWeapon(const ItemEvent* event, const PlayerPtr& player, const ItemPtr& item, const CreaturePtr& target);
		bool useWandWeapon(const ItemEvent* event, const PlayerPtr& player, const ItemPtr& item, const CreaturePtr& target);

		uint32_t fireGroundAddRemItem(const ItemPtr& item, const Position& pos, BlackTek::ItemEvents::HookType type);
		uint32_t fireTileItemAddRem(const ItemPtr& item, const ItemPtr& tileItem, const Position& pos, BlackTek::ItemEvents::HookType type);

		ReturnValue internalUseItem(PlayerPtr player, const Position& pos, uint8_t index, const ItemPtr& item, bool isHotkey);
		static BlackTek::GameModel getUseTarget(const PlayerPtr& player, const CreaturePtr& targetCreature, const Position& toPosition, uint8_t toStackPos);

		bool AddRegistration(ItemEvent& definition);
		void RebuildIndexes();

		[[nodiscard]] bool CanFilterByItemId(BlackTek::ItemEvents::HookType hook) const noexcept;

		static void applyWeaponConfig(const ItemEvent& event, uint16_t id);

		LuaScriptInterface& getScriptInterface() override;
};

extern ItemEvents* g_itemEvents;

#endif
