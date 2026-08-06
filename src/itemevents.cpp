// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "itemevents.h"

#include "augment.h"
#include "configmanager.h"
#include "console.h"
#include "creaturecontainer.h"
#include "game.h"
#include "itemcontainer.h"
#include "spells.h"

#include <algorithm>
#include <fmt/format.h>
#include <optional>
#include <ranges>

extern ConfigManager g_config;
extern Spells* g_spells;
extern Game g_game;

namespace
{
	constexpr std::array<uint32_t, 12> SlotBitmasks
	{
		0, SLOTP_HEAD, SLOTP_NECKLACE, SLOTP_BACKPACK, SLOTP_ARMOR, SLOTP_RIGHT,
		SLOTP_LEFT, SLOTP_LEGS, SLOTP_FEET, SLOTP_RING, SLOTP_AMMO, 0
	};

	[[nodiscard]] constexpr uint32_t ToSlotBitmask(slots_t slot) noexcept
	{
		const auto index = static_cast<size_t>(slot);
		return index < SlotBitmasks.size() ? SlotBitmasks[index] : 0;
	}

	std::optional<ItemEvent::Register::Type> ToRegisterCategory(WeaponType_t type)
	{
		switch (type)
		{
			case WEAPON_SWORD: return ItemEvent::Register::Type::Sword;
			case WEAPON_CLUB: return ItemEvent::Register::Type::Club;
			case WEAPON_AXE: return ItemEvent::Register::Type::Axe;
			case WEAPON_SHIELD: return ItemEvent::Register::Type::Shield;
			case WEAPON_DISTANCE: return ItemEvent::Register::Type::Distance;
			case WEAPON_WAND: return ItemEvent::Register::Type::Wand;
			case WEAPON_AMMO: return ItemEvent::Register::Type::Ammo;
			default: return std::nullopt;
		}
	}
}

const BlackTek::ItemEvents::EventRefs* BlackTek::ItemEvents::RefIndex::Find(uint64_t key) const noexcept
{
	const auto it = entries.find(key);
	return it != entries.end() ? &it->second : nullptr;
}

void BlackTek::ItemEvents::RefIndex::Add(uint64_t key, EventRef ref)
{
	entries[key].push_back(ref);
}

void BlackTek::ItemEvents::RefIndex::Clear() noexcept
{
	entries.clear();
}

const ItemEvent* BlackTek::ItemEvents::SelectEvent(const EventRefs& refs, uint32_t probe) noexcept
{
	auto matches = refs | std::views::filter([probe](const EventRef& ref) { return (ref.select_mask & probe) != 0; });
	auto match = std::ranges::begin(matches);
	return match != std::ranges::end(matches) ? match->event : nullptr;
}

void BlackTek::ItemEvents::HookMaskTable::Set(uint16_t id, HookType hook)
{
	if (id >= masks.size())
		masks.resize(id + 1, 0);

	masks[id] |= ToHookMask(hook);
}

bool BlackTek::ItemEvents::Defaults::EnterMarket(PlayerPtr player, ItemPtr, const Position&, BlackTek::GameModel, const Position&, bool)
{
	player->sendMarketEnter();
	return true;
}

uint32_t BlackTek::ItemEvents::Defaults::StepOnField(const CreaturePtr& creature, const ItemPtr& item, const Position&)
{
	if (const auto field = item->getMagicField())
	{
		field->onStepInField(creature);
		return 1;
	}

	return LUA_ERROR_ITEM_NOT_FOUND;
}

uint32_t BlackTek::ItemEvents::Defaults::StepOffField(const CreaturePtr&, const ItemPtr&, const Position&)
{
	return 1;
}

uint32_t BlackTek::ItemEvents::Defaults::AddItemField(const ItemPtr& item, const ItemPtr&, const Position&)
{
	if (const auto& field = item->getMagicField())
	{
		const auto& tile = item->getTile();
		if (const auto& creatures = tile->getCreatures())
		{
			for (const auto& creature : creatures->getList())
				field->onStepInField(creature);
		}
		return 1;
	}
	return LUA_ERROR_ITEM_NOT_FOUND;
}

uint32_t BlackTek::ItemEvents::Defaults::RemoveItemField(const ItemPtr&, const ItemPtr&, const Position&)
{
	return 1;
}

ReturnValue BlackTek::ItemEvents::Defaults::EquipItem(const ItemEvent* itemEvent, const PlayerPtr& player, const ItemPtr& item, slots_t slot, bool isCheck)
{
	if (not player->hasFlag(PlayerFlag_IgnoreWeaponCheck) and itemEvent->wieldInfo != 0)
	{
		if (not itemEvent->vocationEquipMap.empty() and not itemEvent->HasVocation(player->getVocationId()))
			return RETURNVALUE_YOUDONTHAVEREQUIREDPROFESSION;

		if (player->getLevel() < itemEvent->requiredLevel)
			return RETURNVALUE_NOTENOUGHLEVEL;

		if (player->getMagicLevel() < itemEvent->requiredMagicLevel)
			return RETURNVALUE_NOTENOUGHMAGICLEVEL;

		if (itemEvent->premium and not player->isPremium())
			return RETURNVALUE_YOUNEEDPREMIUMACCOUNT;
	}

	if (isCheck)
		return RETURNVALUE_NOERROR;

	if (player->isItemAbilityEnabled(slot))
		return RETURNVALUE_NOERROR;

	const ItemType& it = Item::items[item->getID()];

	if (it.transformEquipTo != 0)
	{
		const auto& newItem = g_game.transformItem(item, it.transformEquipTo);
		g_game.startDecay(newItem);
	}
	else
	{
		player->setItemAbility(slot, true);
	}

	if (not it.abilities)
		return RETURNVALUE_NOERROR;

	if (it.abilities->invisible)
	{
		auto condition = Condition::createCondition(static_cast<ConditionId_t>(slot), CONDITION_INVISIBLE, -1, 0);
		player->addCondition(std::move(condition));
	}

	if (it.abilities->manaShield)
	{
		auto condition = Condition::createCondition(static_cast<ConditionId_t>(slot), CONDITION_MANASHIELD, -1, 0);
		player->addCondition(std::move(condition));
	}

	if (it.abilities->speed != 0)
		g_game.changeSpeed(player, it.abilities->speed);

	if (it.abilities->conditionSuppressions != 0)
	{
		player->addConditionSuppressions(it.abilities->conditionSuppressions);
		player->sendIcons();
	}

	if (it.abilities->regeneration)
	{
		auto condition = Condition::createCondition(static_cast<ConditionId_t>(slot), CONDITION_REGENERATION, -1, 0);

		if (it.abilities->healthGain != 0)
			condition->setParam(CONDITION_PARAM_HEALTHGAIN, it.abilities->healthGain);

		if (it.abilities->healthTicks != 0)
			condition->setParam(CONDITION_PARAM_HEALTHTICKS, it.abilities->healthTicks);

		if (it.abilities->manaGain != 0)
			condition->setParam(CONDITION_PARAM_MANAGAIN, it.abilities->manaGain);

		if (it.abilities->manaTicks != 0)
			condition->setParam(CONDITION_PARAM_MANATICKS, it.abilities->manaTicks);

		player->addCondition(std::move(condition));
	}

	bool needUpdateSkills = false;

	for (int32_t i = SKILL_FIRST; i <= SKILL_LAST; ++i)
	{
		if (it.abilities->skills[i])
		{
			needUpdateSkills = true;
			player->setVarSkill(static_cast<skills_t>(i), it.abilities->skills[i]);
		}
	}

	for (int32_t i = SPECIALSKILL_FIRST; i <= SPECIALSKILL_LAST; ++i)
	{
		if (it.abilities->specialSkills[i])
		{
			needUpdateSkills = true;
			player->setVarSpecialSkill(static_cast<SpecialSkills_t>(i), it.abilities->specialSkills[i]);
		}
	}

	bool needUpdateStats = false;

	for (int32_t s = STAT_FIRST; s <= STAT_LAST; ++s)
	{
		if (it.abilities->stats[s])
		{
			needUpdateStats = true;
			player->setVarStats(static_cast<stats_t>(s), it.abilities->stats[s]);
		}

		if (it.abilities->statsPercent[s])
		{
			needUpdateStats = true;
			player->setVarStats(static_cast<stats_t>(s), static_cast<int32_t>(player->getDefaultStats(static_cast<stats_t>(s)) * ((it.abilities->statsPercent[s] - 100) / 100.f)));
		}
	}

	if (needUpdateSkills)
		player->sendSkills();

	if (needUpdateStats)
		player->sendStats();

	return RETURNVALUE_NOERROR;
}

ReturnValue BlackTek::ItemEvents::Defaults::DeEquipItem(const ItemEvent*, const PlayerPtr& player, const ItemPtr& item, slots_t slot, bool)
{
	if (not player->isItemAbilityEnabled(slot))
		return RETURNVALUE_NOERROR;

	player->setItemAbility(slot, false);

	const ItemType& it = Item::items[item->getID()];
	if (it.transformDeEquipTo != 0)
	{
		g_game.transformItem(item, it.transformDeEquipTo);
		g_game.startDecay(item);
	}

	if (not it.abilities)
		return RETURNVALUE_NOERROR;

	if (it.abilities->invisible)
		player->removeCondition(CONDITION_INVISIBLE, static_cast<ConditionId_t>(slot));

	if (it.abilities->manaShield)
		player->removeCondition(CONDITION_MANASHIELD, static_cast<ConditionId_t>(slot));

	if (it.abilities->speed != 0)
		g_game.changeSpeed(player, -it.abilities->speed);

	if (it.abilities->conditionSuppressions != 0)
	{
		player->removeConditionSuppressions(it.abilities->conditionSuppressions);
		player->sendIcons();
	}

	if (it.abilities->regeneration)
		player->removeCondition(CONDITION_REGENERATION, static_cast<ConditionId_t>(slot));

	bool needUpdateSkills = false;

	for (int32_t i = SKILL_FIRST; i <= SKILL_LAST; ++i)
	{
		if (it.abilities->skills[i] != 0)
		{
			needUpdateSkills = true;
			player->setVarSkill(static_cast<skills_t>(i), -it.abilities->skills[i]);
		}
	}

	for (int32_t i = SPECIALSKILL_FIRST; i <= SPECIALSKILL_LAST; ++i)
	{
		if (it.abilities->specialSkills[i] != 0)
		{
			needUpdateSkills = true;
			player->setVarSpecialSkill(static_cast<SpecialSkills_t>(i), -it.abilities->specialSkills[i]);
		}
	}

	bool needUpdateStats = false;

	for (int32_t s = STAT_FIRST; s <= STAT_LAST; ++s)
	{
		if (it.abilities->stats[s])
		{
			needUpdateStats = true;
			player->setVarStats(static_cast<stats_t>(s), -it.abilities->stats[s]);
		}

		if (it.abilities->statsPercent[s])
		{
			needUpdateStats = true;
			player->setVarStats(static_cast<stats_t>(s), -static_cast<int32_t>(player->getDefaultStats(static_cast<stats_t>(s)) * ((it.abilities->statsPercent[s] - 100) / 100.f)));
		}
	}

	if (needUpdateSkills)
		player->sendSkills();

	if (needUpdateStats)
		player->sendStats();

	return RETURNVALUE_NOERROR;
}

ItemEvent::ItemEvent(LuaScriptInterface* interface) : Event(interface) {}

std::string_view ItemEvent::getScriptEventName() const
{
	switch (hook)
	{
		case BlackTek::ItemEvents::HookType::OnUse:					return "onUse";
		case BlackTek::ItemEvents::HookType::OnUseAsWeapon:			return "onUseAsWeapon";
		case BlackTek::ItemEvents::HookType::OnEquip:				return "onEquip";
		case BlackTek::ItemEvents::HookType::OnDeEquip:				return "onDeEquip";
		case BlackTek::ItemEvents::HookType::OnStepOn:				return "onStepOn";
		case BlackTek::ItemEvents::HookType::OnStepOff:				return "onStepOff";
		case BlackTek::ItemEvents::HookType::OnAddItem:				return "onAddItem";
		case BlackTek::ItemEvents::HookType::OnRemoveItem:			return "onRemoveItem";
		case BlackTek::ItemEvents::HookType::OnAttack:				return "onAttack";
		case BlackTek::ItemEvents::HookType::OnDefend:				return "onDefend";
		case BlackTek::ItemEvents::HookType::OnAugment:				return "onAugment";
		case BlackTek::ItemEvents::HookType::OnRemoveAugment:		return "onRemoveAugment";
		case BlackTek::ItemEvents::HookType::OnAttackMod:			return "onAttackMod";
		case BlackTek::ItemEvents::HookType::OnDefenseMod:			return "onDefenseMod";
		default:
			BlackTek::Console::Error("ItemEvent::getScriptEventName: Invalid hook type");
			return "";
	}
}

bool ItemEvent::executeUse(const PlayerPtr& player, const ItemPtr& item, const Position& fromPosition, const BlackTek::GameModel& target, const Position& toPosition, bool isHotkey) const
{
	if (not scriptInterface->reserveScriptEnv())
	{
		BlackTek::Console::Error("ItemEvent::executeUse: Call stack overflow");
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);

	LuaScriptInterface::pushSharedPtr(L, player);
	LuaScriptInterface::setMetatable(L, -1, "Player");

	LuaScriptInterface::pushItem(L, item);
	LuaScriptInterface::pushPosition(L, fromPosition);

	LuaScriptInterface::pushGameModel(L, target);
	LuaScriptInterface::pushPosition(L, toPosition);

	LuaScriptInterface::pushBoolean(L, isHotkey);
	return scriptInterface->callFunction(6);
}

bool ItemEvent::executeUseAsWeapon(const PlayerPtr& player, const ItemPtr& item, const LuaVariant& var) const
{
	if (not scriptInterface->reserveScriptEnv())
	{
		BlackTek::Console::Error("ItemEvent::executeUseAsWeapon: Call stack overflow");
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);
	LuaScriptInterface::pushSharedPtr(L, player);
	LuaScriptInterface::setMetatable(L, -1, "Player");
	LuaScriptInterface::pushItem(L, item);
	scriptInterface->pushVariant(L, var);

	return scriptInterface->callFunction(3);
}

bool ItemEvent::executeEquip(const PlayerPtr& player, const ItemPtr& item, slots_t slot, bool isCheck) const
{
	if (not scriptInterface->reserveScriptEnv())
	{
		BlackTek::Console::Error("ItemEvent::executeEquip: Call stack overflow");
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);
	LuaScriptInterface::pushSharedPtr(L, player);
	LuaScriptInterface::setMetatable(L, -1, "Player");
	LuaScriptInterface::pushItem(L, item);
	lua_pushinteger(L, slot);
	LuaScriptInterface::pushBoolean(L, isCheck);

	return scriptInterface->callFunction(4);
}

bool ItemEvent::executeStep(const CreaturePtr& creature, const ItemPtr& item, const Position& pos) const
{
	if (not scriptInterface->reserveScriptEnv())
	{
		BlackTek::Console::Error("ItemEvent::executeStep: Call stack overflow");
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);
	LuaScriptInterface::pushSharedPtr(L, creature);
	LuaScriptInterface::setCreatureMetatable(L, -1, creature);
	LuaScriptInterface::pushItem(L, item);
	LuaScriptInterface::pushPosition(L, pos);
	LuaScriptInterface::pushPosition(L, creature->getLastPosition());

	return scriptInterface->callFunction(4);
}

bool ItemEvent::executeAddRemItem(const ItemPtr& item, const ItemPtr& tileItem, const Position& pos) const
{
	if (not scriptInterface->reserveScriptEnv())
	{
		BlackTek::Console::Error("ItemEvent::executeAddRemItem: Call stack overflow");
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);
	LuaScriptInterface::pushItem(L, item);
	LuaScriptInterface::pushItem(L, tileItem);
	LuaScriptInterface::pushPosition(L, pos);

	return scriptInterface->callFunction(3);
}

void ItemEvent::executeAugment(const ItemPtr& item, const std::shared_ptr<BlackTek::Augment>& augment) const
{
	if (not scriptInterface->reserveScriptEnv())
	{
		BlackTek::Console::Error("ItemEvent::executeAugment: Call stack overflow");
		return;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();
	scriptInterface->pushFunction(scriptId);

	LuaScriptInterface::pushSharedPtr(L, item);
	LuaScriptInterface::setMetatable(L, -1, "Item");

	LuaScriptInterface::pushSharedPtr(L, augment);
	LuaScriptInterface::setMetatable(L, -1, "Augment");

	scriptInterface->callVoidFunction(2);
}

void ItemEvent::executeRemoveAugment(const ItemPtr& item, const std::shared_ptr<BlackTek::Augment>& augment) const
{
	if (not scriptInterface->reserveScriptEnv())
	{
		BlackTek::Console::Error("ItemEvent::executeRemoveAugment: Call stack overflow");
		return;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();
	scriptInterface->pushFunction(scriptId);

	LuaScriptInterface::pushSharedPtr(L, item);
	LuaScriptInterface::setMetatable(L, -1, "Item");

	LuaScriptInterface::pushSharedPtr(L, augment);
	LuaScriptInterface::setMetatable(L, -1, "Augment");

	scriptInterface->callVoidFunction(2);
}

void ItemEvent::executeAttack(const ItemPtr& item, const PlayerPtr& holder, const CreaturePtr& other, BlockType_t blockType, CombatType_t combatType, BlackTek::Combat::Origin origin, bool criticalDamage, bool leechedDamage) const
{
	if (not scriptInterface->reserveScriptEnv())
	{
		BlackTek::Console::Error("ItemEvent::executeAttack: Call stack overflow");
		return;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();
	scriptInterface->pushFunction(scriptId);

	LuaScriptInterface::pushItem(L, item);

	LuaScriptInterface::pushSharedPtr(L, holder);
	LuaScriptInterface::setMetatable(L, -1, "Player");

	if (other)
	{
		LuaScriptInterface::pushSharedPtr(L, other);
		LuaScriptInterface::setCreatureMetatable(L, -1, other);
	}
	else
	{
		lua_pushnil(L);
	}

	lua_pushinteger(L, static_cast<uint8_t>(blockType));
	lua_pushinteger(L, static_cast<uint8_t>(combatType));
	lua_pushinteger(L, std::to_underlying(origin));

	LuaScriptInterface::pushBoolean(L, criticalDamage);
	LuaScriptInterface::pushBoolean(L, leechedDamage);

	scriptInterface->callVoidFunction(8);
}

void ItemEvent::executeAttackMod(const ItemPtr& item, const PlayerPtr& holder, const CreaturePtr& other, const std::shared_ptr<BlackTek::DamageModifier>& modifier, uint32_t damage) const
{
	if (not scriptInterface->reserveScriptEnv())
	{
		BlackTek::Console::Error("ItemEvent::executeAttackMod: Call stack overflow");
		return;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();
	scriptInterface->pushFunction(scriptId);

	LuaScriptInterface::pushItem(L, item);

	LuaScriptInterface::pushSharedPtr(L, holder);
	LuaScriptInterface::setMetatable(L, -1, "Player");

	if (other)
	{
		LuaScriptInterface::pushSharedPtr(L, other);
		LuaScriptInterface::setCreatureMetatable(L, -1, other);
	}
	else
	{
		lua_pushnil(L);
	}

	LuaScriptInterface::pushDamageModifier(L, modifier);
	lua_pushinteger(L, damage);

	scriptInterface->callVoidFunction(5);
}

ItemEvents::ItemEvents()
{
	scriptInterface.initState();
}

ItemEvents::~ItemEvents()
{
	clear(false);
}

LuaScriptInterface& ItemEvents::getScriptInterface()
{
	return scriptInterface;
}

bool ItemEvents::AddRegistration(ItemEvent& definition)
{
	if ((definition.hook == BlackTek::ItemEvents::HookType::OnEquip or definition.hook == BlackTek::ItemEvents::HookType::OnDeEquip) and definition.slot == SLOTP_WHEREEVER and not definition.itemIds.empty())
		definition.slot = Item::items.getItemType(definition.itemIds.front()).slotPosition;

	const auto hookIdx = static_cast<size_t>(definition.hook);
	const uint32_t selectMask = BlackTek::ItemEvents::IsSlotSelected(definition.hook) ? definition.slot : BlackTek::ItemEvents::AnySlot;
	bool registered = false;

	for (auto id : definition.itemIds)
	{
		if (definition.hook == BlackTek::ItemEvents::HookType::OnEquip and definition.wieldInfo != 0)
		{
			ItemType& it = Item::items.getItemType(id);
			it.wieldInfo = definition.wieldInfo;
			it.minReqLevel = definition.requiredLevel;
			it.minReqMagicLevel = definition.requiredMagicLevel;
			it.vocationString = definition.vocationString;
		}

		if (definition.hook == BlackTek::ItemEvents::HookType::OnUseAsWeapon)
			applyWeaponConfig(definition, id);

		const bool isTileItemHook = definition.tileItem
			and (definition.hook == BlackTek::ItemEvents::HookType::OnAddItem or definition.hook == BlackTek::ItemEvents::HookType::OnRemoveItem);

		BlackTek::ItemEvents::RefIndex& refs = isTileItemHook ? tile_item_refs : item_refs;
		const uint64_t key = BlackTek::ItemEvents::PackKey(id, definition.hook);

		if (const auto* existing = refs.Find(key))
		{
			for (const auto& ref : *existing)
			{
				if (ref.select_mask == selectMask)
				{
					BlackTek::Console::Warn("ItemEvents::addEvent: Duplicate registered item event found for id {}", id);
					break;
				}
			}
		}

		refs.Add(key, BlackTek::ItemEvents::EventRef{ &definition, selectMask });
		(isTileItemHook ? tile_item_hook_masks : item_hook_masks).Set(id, definition.hook);
		++hook_counts[hookIdx];
		++(isTileItemHook ? tile_item_hook_counts : item_hook_counts)[hookIdx];
		registered = true;
	}

	for (auto id : definition.actionIds)
	{
		const uint64_t key = BlackTek::ItemEvents::PackKey(id, definition.hook);

		if (const auto* existing = action_refs.Find(key))
		{
			for (const auto& ref : *existing)
			{
				if (ref.select_mask == selectMask)
				{
					BlackTek::Console::Warn("ItemEvents::addEvent: Duplicate registered item event found for id {}", id);
					break;
				}
			}
		}

		action_refs.Add(key, BlackTek::ItemEvents::EventRef{ &definition, selectMask });
		++hook_counts[hookIdx];
		++action_hook_counts[hookIdx];
		registered = true;
	}

	for (auto id : definition.uniqueIds)
	{
		const uint64_t key = BlackTek::ItemEvents::PackKey(id, definition.hook);

		if (const auto* existing = unique_refs.Find(key))
		{
			for (const auto& ref : *existing)
			{
				if (ref.select_mask == selectMask)
				{
					BlackTek::Console::Warn("ItemEvents::addEvent: Duplicate registered item event found for id {}", id);
					break;
				}
			}
		}

		unique_refs.Add(key, BlackTek::ItemEvents::EventRef{ &definition, selectMask });
		++hook_counts[hookIdx];
		++unique_hook_counts[hookIdx];
		registered = true;
	}

	for (const auto& pos : definition.positions)
	{
		const uint64_t key = BlackTek::ItemEvents::PackKey(pos, definition.hook);

		if (const auto* existing = position_refs.Find(key); existing and not existing->empty())
			BlackTek::Console::Warn("ItemEvents::addEvent: Duplicate registered item event found for position ({}, {}, {})", pos.x, pos.y, pos.z);

		position_refs.Add(key, BlackTek::ItemEvents::EventRef{ &definition, BlackTek::ItemEvents::AnySlot });
		++hook_counts[hookIdx];
		++position_hook_counts[hookIdx];
		registered = true;
	}

	for (auto category : definition.categories)
	{
		const uint64_t key = BlackTek::ItemEvents::PackKey(std::to_underlying(category), definition.hook);

		if (const auto* existing = category_refs.Find(key); existing and not existing->empty())
			BlackTek::Console::Warn("ItemEvents::addEvent: Duplicate registered item event found for category {}", std::to_underlying(category));

		category_refs.Add(key, BlackTek::ItemEvents::EventRef{ &definition, selectMask });
		++hook_counts[hookIdx];
		++category_hook_counts[hookIdx];
		registered = true;
	}

	return registered;
}

void ItemEvents::RebuildIndexes()
{
	item_refs.Clear();
	tile_item_refs.Clear();
	action_refs.Clear();
	unique_refs.Clear();
	position_refs.Clear();
	category_refs.Clear();

	item_hook_masks.Clear();
	tile_item_hook_masks.Clear();

	hook_counts = {};
	item_hook_counts = {};
	tile_item_hook_counts = {};
	action_hook_counts = {};
	unique_hook_counts = {};
	position_hook_counts = {};
	category_hook_counts = {};

	for (auto& stored : event_store)
		AddRegistration(*stored);
}

bool ItemEvents::CanFilterByItemId(BlackTek::ItemEvents::HookType hook) const noexcept
{
	const auto hookIndex = static_cast<size_t>(hook);
	return (unique_hook_counts[hookIndex] | action_hook_counts[hookIndex] | category_hook_counts[hookIndex]) == 0;
}

void ItemEvents::clear(bool fromLua)
{
	retired_events.clear();

	for (auto& stored : event_store)
	{
		if (stored and stored->fromLua == fromLua)
			retired_events.push_back(std::move(stored));
	}

	std::erase_if(event_store, [](const std::unique_ptr<ItemEvent>& ptr) { return ptr == nullptr; });

	RebuildIndexes();
	reInitState(fromLua);
}

void ItemEvents::applyWeaponConfig(const ItemEvent& event, uint16_t id)
{
	if (not event.weaponConfig)
		return;

	const auto& config = *event.weaponConfig;
	ItemType& it = Item::items.getItemType(id);

	if (config.weaponTypeSet)
		it.weaponType = config.weaponType;

	if (event.wieldInfo != 0)
	{
		it.wieldInfo = event.wieldInfo;
		it.vocationString = event.vocationString;
		it.minReqLevel = event.requiredLevel;
		it.minReqMagicLevel = event.requiredMagicLevel;
	}

	if (config.attackSet)
		it.attack = config.attack;

	if (config.defenseSet)
	{
		it.defense = config.defense;
		it.extraDefense = config.extraDefense;
	}

	if (config.rangeSet)
		it.shootRange = config.range;

	if (config.chargesSet)
	{
		it.charges = config.charges;
		it.showCharges = config.showCharges;
	}

	if (config.durationSet)
	{
		it.decayTime = config.duration;
		it.showDuration = config.showDuration;
	}

	if (config.decayToSet)
		it.decayTo = config.decayTo;

	if (config.transformEquipTo != 0)
		it.transformEquipTo = config.transformEquipTo;

	if (config.transformDeEquipTo != 0)
		it.transformDeEquipTo = config.transformDeEquipTo;

	if (config.ammoTypeSet)
		it.ammoType = config.ammoType;

	if (config.shootTypeSet)
		it.shootType = config.shootType;

	if (config.hitChance != -1)
		it.hitChance = static_cast<int8_t>(config.hitChance);

	if (config.maxHitChance != -1)
		it.maxHitChance = config.maxHitChance;

	if (config.slotTypeSet)
		it.slotPosition |= (config.twoHanded ? SLOTP_TWO_HAND : SLOTP_HAND);

	if (config.extraElementSet)
	{
		Abilities& abilities = it.getAbilities();
		abilities.elementDamage = static_cast<uint16_t>(config.extraElementDamage);
		abilities.elementType = config.extraElementType;
	}
}

bool ItemEvents::registerLuaEvent(ItemEvent* event)
{
	ItemEvent_ptr itemEvent{ event };

	if (itemEvent->itemIds.empty() and itemEvent->actionIds.empty() and itemEvent->uniqueIds.empty() and itemEvent->positions.empty() and itemEvent->categories.empty())
	{
		BlackTek::Console::Warn("ItemEvents::registerLuaEvent: No id / aid / uid / position / category set for this event");
		return false;
	}

	event_store.push_back(std::move(itemEvent));
	AddRegistration(*event_store.back());
	return true;
}

const ItemEvent* ItemEvents::getEvent(const ItemConstPtr& item, BlackTek::ItemEvents::HookType hook, slots_t slot)
{
	const auto hookIndex = static_cast<size_t>(hook);
	const uint32_t probe = BlackTek::ItemEvents::IsSlotSelected(hook) ? ToSlotBitmask(slot) : BlackTek::ItemEvents::AnySlot;
	const uint16_t itemId = item->getID();

	if (unique_hook_counts[hookIndex] != 0 and item->hasAttribute(ITEM_ATTRIBUTE_UNIQUEID))
	{
		if (const auto* refs = unique_refs.Find(BlackTek::ItemEvents::PackKey(item->getUniqueId(), hook)))
		{
			if (const auto* event = BlackTek::ItemEvents::SelectEvent(*refs, probe))
				return event;
		}
	}

	if (action_hook_counts[hookIndex] != 0 and item->hasAttribute(ITEM_ATTRIBUTE_ACTIONID))
	{
		if (const auto* refs = action_refs.Find(BlackTek::ItemEvents::PackKey(item->getActionId(), hook)))
		{
			if (const auto* event = BlackTek::ItemEvents::SelectEvent(*refs, probe))
				return event;
		}
	}

	if (item_hook_masks.Test(itemId, hook))
	{
		if (const auto* refs = item_refs.Find(BlackTek::ItemEvents::PackKey(itemId, hook)))
		{
			if (const auto* event = BlackTek::ItemEvents::SelectEvent(*refs, probe))
				return event;
		}
	}

	if (category_hook_counts[hookIndex] != 0)
	{
		if (const auto category = ToRegisterCategory(Item::items[itemId].weaponType))
		{
			if (const auto* refs = category_refs.Find(BlackTek::ItemEvents::PackKey(std::to_underlying(*category), hook)))
			{
				if (const auto* event = BlackTek::ItemEvents::SelectEvent(*refs, probe))
					return event;
			}
		}

		if (const auto* refs = category_refs.Find(BlackTek::ItemEvents::PackKey(std::to_underlying(ItemEvent::Register::Type::Any), hook)))
		{
			if (const auto* event = BlackTek::ItemEvents::SelectEvent(*refs, probe))
				return event;
		}
	}

	return nullptr;
}

const ItemEvent* ItemEvents::getPositionEvent(const Position& pos, BlackTek::ItemEvents::HookType hook)
{
	const auto hookIndex = static_cast<size_t>(hook);

	if (position_hook_counts[hookIndex] != 0)
	{
		if (const auto* refs = position_refs.Find(BlackTek::ItemEvents::PackKey(pos, hook)))
			return BlackTek::ItemEvents::SelectEvent(*refs, BlackTek::ItemEvents::AnySlot);
	}

	return nullptr;
}

const ItemEvent* ItemEvents::getTileItemEvent(const ItemConstPtr& item, BlackTek::ItemEvents::HookType hook)
{
	const uint16_t itemId = item->getID();

	if (tile_item_hook_masks.Test(itemId, hook))
	{
		if (const auto* refs = tile_item_refs.Find(BlackTek::ItemEvents::PackKey(itemId, hook)))
			return BlackTek::ItemEvents::SelectEvent(*refs, BlackTek::ItemEvents::AnySlot);
	}

	return nullptr;
}

bool ItemEvents::fireUse(const PlayerPtr& player, const ItemPtr& item, const Position& fromPosition, const BlackTek::GameModel& target, const Position& toPosition, bool isHotkey)
{
	const ItemEvent* event = getEvent(item, BlackTek::ItemEvents::HookType::OnUse);

	if (not event)
		return false;

	if (event->isScripted())
		return event->executeUse(player, item, fromPosition, target, toPosition, isHotkey);

	if (event->actionFunction)
		return event->actionFunction(player, item, fromPosition, target, toPosition, isHotkey);

	return false;
}

ReturnValue ItemEvents::fireEquip(const PlayerPtr& player, const ItemPtr& item, slots_t slot, bool isCheck)
{
	const ItemEvent* event = getEvent(item, BlackTek::ItemEvents::HookType::OnEquip, slot);

	if (not event)
		return RETURNVALUE_NOERROR;

	ReturnValue ret = RETURNVALUE_NOERROR;

	if (event->equipFunction)
		ret = event->equipFunction(event, player, item, slot, isCheck);

	if (event->isScripted() and ret == RETURNVALUE_NOERROR and not event->executeEquip(player, item, slot, isCheck))
		ret = RETURNVALUE_CANNOTBEDRESSED;

	return ret;
}

ReturnValue ItemEvents::fireDeEquip(const PlayerPtr& player, const ItemPtr& item, slots_t slot)
{
	const ItemEvent* event = getEvent(item, BlackTek::ItemEvents::HookType::OnDeEquip, slot);

	if (not event)
	{
		player->setItemAbility(slot, false);
		return RETURNVALUE_NOERROR;
	}

	ReturnValue ret = RETURNVALUE_NOERROR;

	if (event->equipFunction)
		ret = event->equipFunction(event, player, item, slot, false);

	if (event->isScripted() and ret == RETURNVALUE_NOERROR and not event->executeEquip(player, item, slot, false))
		ret = RETURNVALUE_CANNOTBEDRESSED;

	return ret;
}

uint32_t ItemEvents::fireStep(const CreaturePtr& creature, const ItemPtr& item, const Position& pos, BlackTek::ItemEvents::HookType type)
{
	const ItemEvent* event = item ? getEvent(item, type) : getPositionEvent(pos, type);

	if (not event)
		return 1;

	if (event->isScripted())
		return event->executeStep(creature, item, pos);

	if (event->stepFunction)
		return event->stepFunction(creature, item, pos);

	return 1;
}

uint32_t ItemEvents::fireAddRemItem(const ItemPtr& item, const ItemPtr& tileItem, const Position& pos, BlackTek::ItemEvents::HookType type)
{
	const ItemEvent* event = item ? getEvent(item, type) : getPositionEvent(pos, type);

	if (not event)
		return 1;

	if (event->isScripted())
		return event->executeAddRemItem(item, tileItem, pos);

	if (event->moveFunction)
		return event->moveFunction(item, tileItem, pos);

	return 1;
}

uint32_t ItemEvents::fireGroundAddRemItem(const ItemPtr& item, const Position& pos, BlackTek::ItemEvents::HookType type)
{
	const ItemEvent* event = getPositionEvent(pos, type);

	if (not event)
		return 1;

	if (event->isScripted())
		return event->executeAddRemItem(item, nullptr, pos);

	if (event->moveFunction)
		return event->moveFunction(item, nullptr, pos);

	return 1;
}

uint32_t ItemEvents::fireTileItemAddRem(const ItemPtr& item, const ItemPtr& tileItem, const Position& pos, BlackTek::ItemEvents::HookType type)
{
	const ItemEvent* event = getTileItemEvent(tileItem, type);

	if (not event)
		return 1;

	if (event->isScripted())
		return event->executeAddRemItem(item, tileItem, pos);

	if (event->moveFunction)
		return event->moveFunction(item, tileItem, pos);

	return 1;
}

uint32_t ItemEvents::onCreatureMove(const CreaturePtr& creature, const TilePtr& tile, BlackTek::ItemEvents::HookType type)
{
	if (not hasHook(type))
		return 1;

	const Position& pos = tile->getPosition();
	const auto hookIndex = static_cast<size_t>(type);

	uint32_t ret = (position_hook_counts[hookIndex] != 0) ? fireStep(creature, nullptr, pos, type) : 1;

	const bool filterById = CanFilterByItemId(type);

	const auto wantsHook = [this, &tile, type, filterById](size_t index)
	{
		if (not filterById)
			return true;

		const std::optional<uint16_t> stackItemId = tile->getItemIdAt(index);
		return stackItemId.has_value() and item_hook_masks.Test(*stackItemId, type);
	};

	for (const size_t index : std::views::iota(size_t{ 0 }, tile->getStackSize()) | std::views::filter(wantsHook))
	{
		if (const auto& tileItem = tile->getGameModelAt(index).item)
			ret &= fireStep(creature, tileItem, pos, type);
	}

	return ret;
}

uint32_t ItemEvents::onItemMove(const ItemPtr& item, const TilePtr& tile, bool isAdd)
{
	const auto type = isAdd ? BlackTek::ItemEvents::HookType::OnAddItem : BlackTek::ItemEvents::HookType::OnRemoveItem;

	if (not hasHook(type))
		return 1;

	const Position& pos = tile->getPosition();
	const auto hookIndex = static_cast<size_t>(type);

	uint32_t ret = (position_hook_counts[hookIndex] != 0) ? fireGroundAddRemItem(item, pos, type) : 1;
	ret &= fireAddRemItem(item, nullptr, pos, type);

	const auto wantsTileItemHook = [this, &tile, type](size_t index)
	{
		const std::optional<uint16_t> stackItemId = tile->getItemIdAt(index);
		return stackItemId.has_value() and tile_item_hook_masks.Test(*stackItemId, type);
	};

	for (const size_t index : std::views::iota(size_t{ 0 }, tile->getStackSize()) | std::views::filter(wantsTileItemHook))
	{
		if (const auto& tileItem = tile->getGameModelAt(index).item; tileItem and tileItem != item)
			ret &= fireTileItemAddRem(item, tileItem, pos, type);
	}

	return ret;
}

namespace
{
	namespace StandardHitChance
	{
		constexpr uint32_t MAX_SKILL = 110;
		constexpr uint32_t DISTANCES = 8;

		enum class HitChanceModel : uint8_t
		{
			MODEL_75 = 0,
			MODEL_90 = 1,
			MODEL_100 = 2,
		};

		constexpr std::array<std::pair<int32_t, int32_t>, 9> DirectionOffsets
		{ {
			{-1, -1}, {0, -1}, {1, -1},
			{-1,  0}, {0,  0}, {1,  0},
			{-1,  1}, {0,  1}, {1,  1}
		} };

		constexpr std::array<std::array<uint32_t, MAX_SKILL + 1>, DISTANCES> Model75()
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

		constexpr std::array<std::array<uint32_t, MAX_SKILL + 1>, DISTANCES> Model90()
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

		constexpr std::array<std::array<uint32_t, MAX_SKILL + 1>, DISTANCES> Model100()
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

		constexpr auto Model75Table = Model75();
		constexpr auto Model90Table = Model90();
		constexpr auto Model100Table = Model100();

		uint32_t getChance(HitChanceModel model, uint32_t distance, uint32_t skill)
		{
			distance = std::clamp(distance, 0u, DISTANCES - 1);
			skill = std::min(skill, MAX_SKILL);

			switch (model)
			{
				case HitChanceModel::MODEL_75: return Model75Table[distance][skill];
				case HitChanceModel::MODEL_90: return Model90Table[distance][skill];
				case HitChanceModel::MODEL_100: return Model100Table[distance][skill];
				[[unlikely]] default: return 0;
			}
		}
	}

	struct EffectiveWeaponFields
	{
		uint32_t mana = 0;
		uint32_t mana_percent = 0;
		uint32_t health = 0;
		uint32_t health_percent = 0;
		uint32_t soul = 0;
		int32_t break_chance = 0;
		bool wield_unproperly = false;
		WeaponAction_t action = WEAPONACTION_NONE;
		uint32_t required_level = 0;
		uint32_t required_magic_level = 0;
		bool premium = false;
		const VocEquipList* vocation_map = nullptr;
		uint16_t primary_damage_type = COMBAT_PHYSICALDAMAGE;
	};

	EffectiveWeaponFields GetEffectiveFields(const ItemEvent* event)
	{
		EffectiveWeaponFields fields;

		if (not event)
			return fields;

		fields.required_level = event->requiredLevel;
		fields.required_magic_level = event->requiredMagicLevel;
		fields.premium = event->premium;

		if (not event->vocationEquipMap.empty())
			fields.vocation_map = &event->vocationEquipMap;

		if (event->weaponConfig)
		{
			const auto& config = *event->weaponConfig;
			fields.mana = config.mana;
			fields.mana_percent = config.manaPercent;
			fields.health = config.health;
			fields.health_percent = config.healthPercent;
			fields.soul = config.soul;
			fields.break_chance = config.breakChance;
			fields.wield_unproperly = config.wieldUnproperly;
			fields.action = config.action;

			if (config.combat)
				fields.primary_damage_type = config.combat->GetDamageType();
		}

		return fields;
	}

	uint32_t GetManaCost(const EffectiveWeaponFields& fields, const PlayerConstPtr& player)
	{
		if (fields.mana != 0)
			return fields.mana;

		if (fields.mana_percent == 0)
			return 0;

		return (player->getMaxMana() * fields.mana_percent) / 100;
	}

	int32_t GetHealthCost(const EffectiveWeaponFields& fields, const PlayerConstPtr& player)
	{
		if (fields.health != 0)
			return static_cast<int32_t>(fields.health);

		if (fields.health_percent == 0)
			return 0;

		return static_cast<int32_t>((player->getMaxHealth() * fields.health_percent) / 100);
	}

	int32_t PlayerWeaponCheck(const EffectiveWeaponFields& fields, const PlayerConstPtr& player, const CreatureConstPtr& target, uint8_t shootRange)
	{
		const Position& playerPos = player->getPosition();
		const Position& targetPos = target->getPosition();

		if (playerPos.z != targetPos.z)
			return 0;

		if (std::max<uint32_t>(Position::getDistanceX(playerPos, targetPos), Position::getDistanceY(playerPos, targetPos)) > shootRange)
			return 0;

		if (not player->hasFlag(PlayerFlag_IgnoreWeaponCheck))
		{
			if (player->getMana() < GetManaCost(fields, player))
				return 0;

			if (static_cast<int32_t>(player->getHealth()) < GetHealthCost(fields, player))
				return 0;

			if (player->getSoul() < fields.soul)
				return 0;

			if (fields.premium and not player->isPremium())
				return 0;

			if (fields.vocation_map and not std::ranges::binary_search(*fields.vocation_map, player->getVocationId()))
				return 0;

			int32_t damageModifier = 100;

			if (player->getLevel() < fields.required_level)
				damageModifier = fields.wield_unproperly ? damageModifier / 2 : 0;

			if (player->getMagicLevel() < fields.required_magic_level)
				damageModifier = fields.wield_unproperly ? damageModifier / 2 : 0;

			return damageModifier;
		}

		return 100;
	}

	bool AmmoCheck(const EffectiveWeaponFields& fields, const PlayerConstPtr& player)
	{
		if (not player->hasFlag(PlayerFlag_IgnoreWeaponCheck))
		{
			if (player->getMana() < GetManaCost(fields, player) or
				static_cast<int32_t>(player->getHealth()) < GetHealthCost(fields, player) or
				(fields.premium and not player->isPremium()) or
				player->getLevel() < fields.required_level or
				player->getMagicLevel() < fields.required_magic_level or
				player->getSoul() < fields.soul)
			{
				return false;
			}

			if (fields.vocation_map and not std::ranges::binary_search(*fields.vocation_map, player->getVocationId()))
				return false;
		}

		return true;
	}

	void DecrementItemCount(const ItemPtr& item)
	{
		uint16_t count = item->getItemCount();

		if (count > 1)
			g_game.transformItem(item, item->getID(), count - 1);
		else
			g_game.internalRemoveItem(item);
	}

	void OnUsedWeapon(const EffectiveWeaponFields& fields, const PlayerPtr& player, const ItemPtr& item, const TilePtr& destTile, skills_t skillType, uint32_t skillPoint, bool hasSkillType)
	{
		if (not player->hasFlag(PlayerFlag_NotGainSkill) and hasSkillType)
			player->addSkillAdvance(skillType, skillPoint);

		uint32_t manaCost = GetManaCost(fields, player);

		if (manaCost != 0)
		{
			player->addManaSpent(manaCost);
			player->changeMana(-static_cast<int32_t>(manaCost));
		}

		int32_t healthCost = GetHealthCost(fields, player);

		if (healthCost != 0)
			player->changeHealth(-healthCost);

		if (not player->hasFlag(PlayerFlag_HasInfiniteSoul) and fields.soul > 0)
			player->changeSoul(-static_cast<int32_t>(fields.soul));

		if (fields.break_chance != 0 and uniform_random(1, 100) <= fields.break_chance)
		{
			DecrementItemCount(item);
			return;
		}

		switch (fields.action)
		{
			case WEAPONACTION_REMOVECOUNT:
			{
				if (g_config.GetBoolean(ConfigManager::REMOVE_WEAPON_AMMO))
					DecrementItemCount(item);
				break;
			}

			case WEAPONACTION_REMOVECHARGE:
			{
				uint16_t charges = item->getCharges();
				if (charges != 0 and g_config.GetBoolean(ConfigManager::REMOVE_WEAPON_CHARGES))
					g_game.transformItem(item, item->getID(), charges - 1);
				break;
			}

			case WEAPONACTION_MOVE: 
				g_game.internalMoveItem(item->getLocation(), { .tile = destTile }, INDEX_ANYWHERE, item, 1, std::nullopt, FLAG_NOLIMIT);
				break;

			default:
				break;
		}
	}

	int32_t GetMeleeWeaponDamage(const PlayerConstPtr& player, const ItemConstPtr& item, bool maxDamage)
	{
		const int32_t playerLevel = player->getLevel();
		const float playerAttackFactor = player->getAttackFactor();
		const auto& playerVocation = player->getVocation();
		int32_t attackSkill = player->getWeaponSkill(item);
		int32_t attackValue = std::max<int32_t>(0, item->getAttack());
		const float meleeDamageMultiplier = playerVocation->meleeDamageMultiplier;
		int32_t maxValue = static_cast<int32_t>(ItemEvents::getMaxWeaponDamage(playerLevel, attackSkill, attackValue, playerAttackFactor) * meleeDamageMultiplier * player->getDualWieldMultiplier());

		if (maxDamage)
			return -maxValue;

		return -normal_random(0, maxValue);
	}

	int32_t GetMeleeElementDamage(const PlayerConstPtr& player, const ItemConstPtr& item)
	{
		const ItemType& it = Item::items[item->getID()];

		if (not it.abilities or it.abilities->elementType == COMBAT_NONE)
			return 0;

		int32_t attackSkill = player->getWeaponSkill(item);
		int32_t attackValue = it.abilities->elementDamage;
		float attackFactor = player->getAttackFactor();

		int32_t maxValue = ItemEvents::getMaxWeaponDamage(player->getLevel(), attackSkill, attackValue, attackFactor);
		return -normal_random(0, static_cast<int32_t>(maxValue * player->getVocation()->meleeDamageMultiplier * player->getDualWieldMultiplier()));
	}

	bool GetMeleeSkillType(const PlayerConstPtr& player, const ItemConstPtr& item, skills_t& skill, uint32_t& skillpoint)
	{
		skillpoint = (player->getAddAttackSkill() and player->getLastAttackBlockType() != BLOCK_IMMUNITY) ? 1 : 0;

		switch (item->getWeaponType())
		{
			case WEAPON_SWORD: skill = SKILL_SWORD; return true;
			case WEAPON_CLUB: skill = SKILL_CLUB; return true;
			case WEAPON_AXE: skill = SKILL_AXE; return true;
			default: break;
		}
		return false;
	}

	int32_t GetDistanceWeaponDamage(const PlayerConstPtr& player, const CreatureConstPtr& target, const ItemConstPtr& item, bool maxDamage)
	{
		const int32_t playerLevel = player->getLevel();
		const float playerAttackFactor = player->getAttackFactor();
		const auto& playerVocation = player->getVocation();
		int32_t attackValue = item->getAttack();

		if (item->getWeaponType() == WEAPON_AMMO)
		{
			if (const auto& weapon = player->getWeapon(true))
				attackValue += weapon->getAttack();
		}

		int32_t attackSkill = player->getSkillLevel(SKILL_DISTANCE);
		const float distDamageMultiplier = playerVocation->distDamageMultiplier;
		int32_t maxValue = static_cast<int32_t>(ItemEvents::getMaxWeaponDamage(playerLevel, attackSkill, attackValue, playerAttackFactor) * distDamageMultiplier);

		if (maxDamage)
			return -maxValue;

		int32_t minValue = target ? static_cast<int32_t>(std::ceil(playerLevel * (target->getPlayer() ? 0.1 : 0.2))) : 0;
		return -normal_random(minValue, maxValue);
	}

	int32_t GetDistanceElementDamage(const PlayerConstPtr& player, const CreatureConstPtr& target, const ItemConstPtr& item)
	{
		const ItemType& it = Item::items[item->getID()];

		if (not it.abilities or it.abilities->elementType == COMBAT_NONE)
			return 0;

		int32_t attackValue = it.abilities->elementDamage;

		if (item->getWeaponType() == WEAPON_AMMO)
		{
			if (const auto weapon = player->getWeapon(true))
				attackValue += weapon->getAttack();
		}

		int32_t attackSkill = player->getSkillLevel(SKILL_DISTANCE);
		float attackFactor = player->getAttackFactor();

		int32_t minValue = 0;
		int32_t maxValue = ItemEvents::getMaxWeaponDamage(player->getLevel(), attackSkill, attackValue, attackFactor);

		if (target)
			minValue = static_cast<int32_t>(std::ceil(player->getLevel() * (target->getPlayer() ? 0.1 : 0.2)));

		return -normal_random(minValue, static_cast<int32_t>(maxValue * player->getVocation()->distDamageMultiplier));
	}

	bool GetDistanceSkillType(const PlayerConstPtr& player, uint32_t& skillpoint)
	{
		if (player->getAddAttackSkill())
		{
			switch (player->getLastAttackBlockType())
			{
				case BLOCK_NONE:
					skillpoint = 2;
					break;
				case BLOCK_DEFENSE:
				case BLOCK_ARMOR:
					skillpoint = 1;
					break;
				default:
					skillpoint = 0;
					break;
			}
		}
		else
		{
			skillpoint = 0;
		}
		return true;
	}

	int32_t GetWandDamage(const ItemEvent* event, bool maxDamage)
	{
		int32_t minChange = 0;
		int32_t maxChange = 0;
		if (event and event->weaponConfig)
		{
			minChange = event->weaponConfig->minChange;
			maxChange = event->weaponConfig->maxChange;
		}

		if (maxDamage)
			return -maxChange;

		return -normal_random(minChange, maxChange);
	}
}

bool ItemEvents::useMeleeWeapon(const ItemEvent* event, const PlayerPtr& player, const ItemPtr& item, const CreaturePtr& target)
{
	const EffectiveWeaponFields fields = GetEffectiveFields(event);
	const int32_t damageModifier = PlayerWeaponCheck(fields, player, target, item->getShootRange());

	if (damageModifier == 0)
		return false;

	if (event and event->isScripted())
	{
		LuaVariant var;
		var.setNumber(target->getID());
		event->executeUseAsWeapon(player, item, var);
	}
	else
	{
		const SpectatorVec spectators = g_game.map.fetchSpectators(target->getPosition(), true, true);
		const std::span<const CreaturePtr> spectatorSpan(spectators.begin(), spectators.end());
		const ItemType& it = Item::items[item->getID()];

		int32_t rawDmg = (GetMeleeWeaponDamage(player, item, false) * damageModifier) / 100;
		auto strike = BlackTek::g_combat_registry.Create(fields.primary_damage_type, static_cast<uint32_t>(std::abs(rawDmg)));
		strike->SetConfig(BlackTek::Combat::Config::TrueDamage);
		strike->SetConfig(BlackTek::Combat::Config::BlockedByArmor);
		strike->SetConfig(BlackTek::Combat::Config::BlockedByDefense);
		strike->SetConfig(BlackTek::Combat::Config::Aggressive);
		strike->setOrigin(BlackTek::Combat::Origin::Melee);

		if (it.abilities)
			strike->SetConfig(BlackTek::Combat::Config::UseCharges);

		strike->SetDistanceEffect(static_cast<uint8_t>(it.shootType));
		strike->strike_target(player, target, false, spectatorSpan);

		int32_t elemDmg = GetMeleeElementDamage(player, item);

		if (elemDmg != 0)
		{
			auto elemStrike = BlackTek::g_combat_registry.Create(static_cast<uint16_t>(it.abilities->elementType), static_cast<uint32_t>(std::abs(elemDmg)));
			elemStrike->SetConfig(BlackTek::Combat::Config::Aggressive);
			elemStrike->strike_target(player, target, false, spectatorSpan);
		}
	}

	skills_t skillType;
	uint32_t skillPoint;
	bool hasSkillType = GetMeleeSkillType(player, item, skillType, skillPoint);
	OnUsedWeapon(fields, player, item, target->getTile(), skillType, skillPoint, hasSkillType);
	return true;
}

bool ItemEvents::useWandWeapon(const ItemEvent* event, const PlayerPtr& player, const ItemPtr& item, const CreaturePtr& target)
{
	const EffectiveWeaponFields fields = GetEffectiveFields(event);
	const int32_t damageModifier = PlayerWeaponCheck(fields, player, target, item->getShootRange());

	if (damageModifier == 0)
		return false;

	if (event and event->isScripted())
	{
		LuaVariant var;
		var.setNumber(target->getID());
		event->executeUseAsWeapon(player, item, var);
	}
	else
	{
		const SpectatorVec spectators = g_game.map.fetchSpectators(target->getPosition(), true, true);
		const std::span<const CreaturePtr> spectatorSpan(spectators.begin(), spectators.end());
		const ItemType& it = Item::items[item->getID()];

		int32_t rawDmg = (GetWandDamage(event, false) * damageModifier) / 100;
		auto strike = BlackTek::g_combat_registry.Create(fields.primary_damage_type, static_cast<uint32_t>(std::abs(rawDmg)));
		strike->SetConfig(BlackTek::Combat::Config::TrueDamage);
		strike->SetConfig(BlackTek::Combat::Config::BlockedByArmor);
		strike->SetConfig(BlackTek::Combat::Config::BlockedByDefense);
		strike->SetConfig(BlackTek::Combat::Config::Aggressive);
		strike->SetDistanceEffect(static_cast<uint8_t>(it.shootType));
		strike->strike_target(player, target, false, spectatorSpan);
	}

	OnUsedWeapon(fields, player, item, target->getTile(), SKILL_FIST, 0, false);
	return true;
}

bool ItemEvents::useDistanceWeapon(const ItemEvent* event, const PlayerPtr& player, const ItemPtr& item, const CreaturePtr& target)
{
	if (not player or not item or not target)
		return false;

	const ItemType& it = Item::items[item->getID()];
	const Position& playerPos = player->getPosition();
	const Position& targetPos = target->getPosition();

	const EffectiveWeaponFields fields = GetEffectiveFields(event);

	int32_t damageModifier = 0;
	if (it.weaponType == WEAPON_AMMO)
	{
		const ItemPtr& mainWeaponItem = player->getWeapon(true);

		if (mainWeaponItem)
		{
			const ItemEvent* mainEvent = getEvent(mainWeaponItem, BlackTek::ItemEvents::HookType::OnUseAsWeapon);
			const EffectiveWeaponFields mainFields = mainEvent ? GetEffectiveFields(mainEvent) : fields;
			damageModifier = PlayerWeaponCheck(mainFields, player, target, mainWeaponItem->getShootRange());
		}
	}
	else
	{
		damageModifier = PlayerWeaponCheck(fields, player, target, item->getShootRange());
	}

	if (damageModifier == 0)
		return false;

	uint32_t chance;

	if (it.hitChance == 0)
	{
		const uint32_t skill = player->getSkillLevel(SKILL_DISTANCE);
		const uint32_t distance = std::min(7u, std::max<uint32_t>(
			Position::getDistanceX(playerPos, targetPos), Position::getDistanceY(playerPos, targetPos)));

		const uint32_t maxHitChance = (it.maxHitChance != -1) ? it.maxHitChance : (it.ammoType != AMMO_NONE ? 90 : 75);

		switch (maxHitChance)
		{
			case 75:
				chance = StandardHitChance::getChance(StandardHitChance::HitChanceModel::MODEL_75, distance, skill);
				break;
			case 90:
				chance = StandardHitChance::getChance(StandardHitChance::HitChanceModel::MODEL_90, distance, skill);
				break;
			case 100:
				chance = StandardHitChance::getChance(StandardHitChance::HitChanceModel::MODEL_100, distance, skill);
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
			chance += bow->getHitChance();
		}
	}

	if (chance >= uniform_random(1, 100))
	{
		if (event and event->isScripted())
		{
			LuaVariant var;
			var.setNumber(target->getID());
			event->executeUseAsWeapon(player, item, var);
		}
		else
		{
			const SpectatorVec spectators = g_game.map.fetchSpectators(target->getPosition(), true, true);
			const std::span<const CreaturePtr> spectatorSpan(spectators.begin(), spectators.end());

			int32_t rawDmg = (GetDistanceWeaponDamage(player, target, item, false) * damageModifier) / 100;
			auto strike = BlackTek::g_combat_registry.Create(fields.primary_damage_type, static_cast<uint32_t>(std::abs(rawDmg)));
			strike->SetConfig(BlackTek::Combat::Config::TrueDamage);
			strike->SetConfig(BlackTek::Combat::Config::BlockedByArmor);
			strike->SetConfig(BlackTek::Combat::Config::Aggressive);
			strike->setOrigin(BlackTek::Combat::Origin::Ranged);

			if (it.abilities)
				strike->SetConfig(BlackTek::Combat::Config::UseCharges);

			strike->SetDistanceEffect(static_cast<uint8_t>(it.shootType));
			strike->strike_target(player, target, false, spectatorSpan);
			int32_t elemDmg = GetDistanceElementDamage(player, target, item);

			if (elemDmg != 0)
			{
				auto elemStrike = BlackTek::g_combat_registry.Create(static_cast<uint16_t>(it.abilities->elementType), static_cast<uint32_t>(std::abs(elemDmg)));
				elemStrike->SetConfig(BlackTek::Combat::Config::Aggressive);
				elemStrike->strike_target(player, target, false, spectatorSpan);
			}
		}

		uint32_t skillPoint;
		bool hasSkillType = GetDistanceSkillType(player, skillPoint);
		OnUsedWeapon(fields, player, item, target->getTile(), SKILL_DISTANCE, skillPoint, hasSkillType);
		return true;
	}

	auto destTile = target->getTile();
	if (not Position::areInRange<1, 1, 0>(playerPos, targetPos))
	{
		std::array<std::pair<int32_t, int32_t>, 9> shuffledOffsets = StandardHitChance::DirectionOffsets;
		std::ranges::shuffle(shuffledOffsets, getRandomGenerator());
		const Position& base = target->getPosition();

		for (const auto& [dx, dy] : shuffledOffsets)
		{
			if (auto tile = g_game.map.getTile(base.x + dx, base.y + dy, base.z); tile and tile->getGround() and not tile->hasFlag(TILESTATE_IMMOVABLEBLOCKSOLID))
			{
				destTile = tile;
				break;
			}
		}
	}

	if (event and event->isScripted())
	{
		LuaVariant var;
		var.setTargetPosition(destTile->getPosition());
		event->executeUseAsWeapon(player, item, var);
	}
	else
	{
		const uint8_t shootType = static_cast<uint8_t>(it.shootType);

		if (shootType != CONST_ANI_NONE)
			BlackTek::Combat::addDistanceEffect(player, player->getPosition(), destTile->getPosition(), shootType);

		g_game.addMagicEffect(destTile->getPosition(), CONST_ME_POFF);
	}

	uint32_t skillPoint;
	bool hasSkillType = GetDistanceSkillType(player, skillPoint);
	OnUsedWeapon(fields, player, item, destTile, SKILL_DISTANCE, skillPoint, hasSkillType);
	return true;
}

bool ItemEvents::hasWeaponBehavior(const ItemConstPtr& item)
{
	if (not item)
		return false;

	switch (Item::items[item->getID()].weaponType)
	{
		case WEAPON_SWORD:
		case WEAPON_CLUB:
		case WEAPON_AXE:
		case WEAPON_DISTANCE:
		case WEAPON_AMMO:
			return true;
		case WEAPON_WAND:
			return getEvent(item, BlackTek::ItemEvents::HookType::OnUseAsWeapon) != nullptr;
		default:
			return false;
	}
}

bool ItemEvents::useAsWeapon(const PlayerPtr& player, const ItemPtr& item, const CreaturePtr& target)
{
	const ItemEvent* event = getEvent(item, BlackTek::ItemEvents::HookType::OnUseAsWeapon);

	switch (Item::items[item->getID()].weaponType)
	{
		case WEAPON_SWORD:
		case WEAPON_CLUB:
		case WEAPON_AXE:
			return useMeleeWeapon(event, player, item, target);
		case WEAPON_DISTANCE:
		case WEAPON_AMMO:
			return useDistanceWeapon(event, player, item, target);
		case WEAPON_WAND:
			return useWandWeapon(event, player, item, target);
		default:
			return false;
	}
}

bool ItemEvents::ammoCheck(const ItemConstPtr& item, const PlayerConstPtr& player)
{
	const ItemEvent* event = getEvent(item, BlackTek::ItemEvents::HookType::OnUseAsWeapon);
	return AmmoCheck(GetEffectiveFields(event), player);
}

bool ItemEvents::interruptSwing(const ItemConstPtr& item)
{
	if (not item)
		return false;

	const WeaponType_t weaponType = Item::items[item->getID()].weaponType;
	return weaponType == WEAPON_DISTANCE or weaponType == WEAPON_AMMO;
}

bool ItemEvents::useFist(const PlayerPtr& player, const CreaturePtr& target)
{
	if (not Position::areInRange<1, 1>(player->getPosition(), target->getPosition()))
		return false;

	float attackFactor = player->getAttackFactor();
	int32_t attackSkill = player->getSkillLevel(SKILL_FIST);
	int32_t attackValue = 7;

	int32_t maxDamage = ItemEvents::getMaxWeaponDamage(player->getLevel(), attackSkill, attackValue, attackFactor);

	auto fist = BlackTek::g_combat_registry.Create(
		static_cast<uint16_t>(BlackTek::Combat::DamageType::Physical),
		static_cast<uint32_t>(normal_random(0, maxDamage))
	);
	fist->SetConfig(BlackTek::Combat::Config::BlockedByArmor);
	fist->SetConfig(BlackTek::Combat::Config::BlockedByDefense);
	fist->SetConfig(BlackTek::Combat::Config::Aggressive);
	fist->setOrigin(BlackTek::Combat::Origin::Fist);

	const SpectatorVec fistSpectators = g_game.map.fetchSpectators(target->getPosition(), true, true);
	const std::span<const CreaturePtr> fistSpectatorSpan(fistSpectators.begin(), fistSpectators.end());
	fist->strike_target(player, target, false, fistSpectatorSpan);

	if (not player->hasFlag(PlayerFlag_NotGainSkill) and player->getAddAttackSkill())
		player->addSkillAdvance(SKILL_FIST, 1);

	return true;
}

int32_t ItemEvents::getMaxMeleeDamage(int32_t attackSkill, int32_t attackValue) noexcept
{
	return static_cast<int32_t>(std::ceil((attackSkill * (attackValue * 0.05)) + (attackValue * 0.5)));
}

int32_t ItemEvents::getMaxWeaponDamage(uint32_t level, int32_t attackSkill, int32_t attackValue, float attackFactor) noexcept
{
	return static_cast<int32_t>(std::round((level / 5) + (((((attackSkill / 4.) + 1) * (attackValue / 3.)) * 1.03) / attackFactor)));
}

bool ItemEvents::reload()
{
	RebuildIndexes();
	return true;
}

bool ItemEvents::fireUseAsWeapon(const PlayerPtr& player, const ItemPtr& item, const LuaVariant& var)
{
	const ItemEvent* event = getEvent(item, BlackTek::ItemEvents::HookType::OnUseAsWeapon);

	if (not event or not event->isScripted())
		return false;

	return event->executeUseAsWeapon(player, item, var);
}

void ItemEvents::fireAugment(const ItemPtr& item, const std::shared_ptr<BlackTek::Augment>& augment)
{
	const ItemEvent* event = getEvent(item, BlackTek::ItemEvents::HookType::OnAugment);

	if (not event or not event->isScripted())
		return;

	event->executeAugment(item, augment);
}

void ItemEvents::fireRemoveAugment(const ItemPtr& item, const std::shared_ptr<BlackTek::Augment>& augment)
{
	const ItemEvent* event = getEvent(item, BlackTek::ItemEvents::HookType::OnRemoveAugment);

	if (not event or not event->isScripted())
		return;

	event->executeRemoveAugment(item, augment);
}

void ItemEvents::fireAttack(const ItemPtr& item, const PlayerPtr& holder, const CreaturePtr& defender, BlockType_t blockType, CombatType_t combatType, BlackTek::Combat::Origin origin, bool criticalDamage, bool leechedDamage)
{
	const ItemEvent* event = getEvent(item, BlackTek::ItemEvents::HookType::OnAttack);

	if (not event or not event->isScripted())
		return;

	event->executeAttack(item, holder, defender, blockType, combatType, origin, criticalDamage, leechedDamage);
}

void ItemEvents::fireDefend(const ItemPtr& item, const PlayerPtr& holder, const CreaturePtr& attacker, BlockType_t blockType, CombatType_t combatType, BlackTek::Combat::Origin origin, bool criticalDamage, bool leechedDamage)
{
	const ItemEvent* event = getEvent(item, BlackTek::ItemEvents::HookType::OnDefend);

	if (not event or not event->isScripted())
		return;

	event->executeAttack(item, holder, attacker, blockType, combatType, origin, criticalDamage, leechedDamage);
}

void ItemEvents::fireAttackMod(const ItemPtr& item, const PlayerPtr& holder, const CreaturePtr& defender, const std::shared_ptr<BlackTek::DamageModifier>& modifier, uint32_t damage)
{
	const ItemEvent* event = getEvent(item, BlackTek::ItemEvents::HookType::OnAttackMod);

	if (not event or not event->isScripted())
		return;

	event->executeAttackMod(item, holder, defender, modifier, damage);
}

void ItemEvents::fireDefenseMod(const ItemPtr& item, const PlayerPtr& holder, const CreaturePtr& attacker, const std::shared_ptr<BlackTek::DamageModifier>& modifier, uint32_t damage)
{
	const ItemEvent* event = getEvent(item, BlackTek::ItemEvents::HookType::OnDefenseMod);

	if (not event or not event->isScripted())
		return;

	event->executeAttackMod(item, holder, attacker, modifier, damage);
}

BlackTek::ItemEvents::HookMask ItemEvents::getCombatHookMask(const ItemConstPtr& item) noexcept
{
	using namespace BlackTek::ItemEvents;

	HookMask mask = 0;

	if (hasHook(HookType::OnAttack) and getEvent(item, HookType::OnAttack))
		mask |= ToHookMask(HookType::OnAttack);

	if (hasHook(HookType::OnDefend) and getEvent(item, HookType::OnDefend))
		mask |= ToHookMask(HookType::OnDefend);

	if (hasHook(HookType::OnAttackMod) and getEvent(item, HookType::OnAttackMod))
		mask |= ToHookMask(HookType::OnAttackMod);

	if (hasHook(HookType::OnDefenseMod) and getEvent(item, HookType::OnDefenseMod))
		mask |= ToHookMask(HookType::OnDefenseMod);

	return mask;
}

namespace
{
	void ShowUseHotkeyMessage(const PlayerPtr& player, const ItemConstPtr& item, uint32_t count)
	{
		const ItemType& it = Item::items[item->getID()];

		if (not it.showCount)
			player->sendTextMessage(MESSAGE_INFO_DESCR, fmt::format("Using one of {:s}...", item->getName()));

		else if (count == 1)
			player->sendTextMessage(MESSAGE_INFO_DESCR, fmt::format("Using the last {:s}...", item->getName()));

		else
			player->sendTextMessage(MESSAGE_INFO_DESCR, fmt::format("Using one of {:d} {:s}...", count, item->getPluralName()));
	}
}

ReturnValue ItemEvents::canUse(const PlayerConstPtr& player, const Position& pos)
{
	if (pos.x != 0xFFFF)
	{
		const Position& playerPos = player->getPosition();

		if (playerPos.z != pos.z)
			return playerPos.z > pos.z ? RETURNVALUE_FIRSTGOUPSTAIRS : RETURNVALUE_FIRSTGODOWNSTAIRS;

		if (not Position::areInRange<1, 1>(playerPos, pos))
			return RETURNVALUE_TOOFARAWAY;
	}
	return RETURNVALUE_NOERROR;
}

ReturnValue ItemEvents::canUse(const PlayerConstPtr& player, const Position& pos, const ItemConstPtr& item)
{
	if (const ItemEvent* event = getEvent(item, BlackTek::ItemEvents::HookType::OnUse))
	{
		if (event->allowFarUse)
			return canUseFar(player, pos, event->checkLineOfSight, event->checkFloor);

		return canUse(player, pos);
	}

	if (RuneSpell* rune = g_spells->getRuneSpell(item->getID()))
	{
		if (rune->getAllowFarUse())
			return canUseFar(player, pos, rune->getCheckLineOfSight(), rune->getCheckFloor());

		return canUse(player, pos);
	}

	return RETURNVALUE_NOERROR;
}

ReturnValue ItemEvents::canUseFar(const CreatureConstPtr& creature, const Position& toPos, bool checkLineOfSight, bool checkFloor)
{
	if (toPos.x == 0xFFFF)
		return RETURNVALUE_NOERROR;

	const Position& creaturePos = creature->getPosition();

	if (checkFloor and creaturePos.z != toPos.z)
		return creaturePos.z > toPos.z ? RETURNVALUE_FIRSTGOUPSTAIRS : RETURNVALUE_FIRSTGODOWNSTAIRS;

	if (not Position::areInRange<7, 5>(toPos, creaturePos))
		return RETURNVALUE_TOOFARAWAY;

	if (checkLineOfSight and not g_game.canThrowObjectTo(creaturePos, toPos, checkLineOfSight, checkFloor))
		return RETURNVALUE_CANNOTTHROW;

	return RETURNVALUE_NOERROR;
}

BlackTek::GameModel ItemEvents::getUseTarget(const PlayerPtr& player, const CreaturePtr& targetCreature, const Position& toPosition, uint8_t toStackPos)
{
	if (targetCreature)
		return { targetCreature, nullptr };

	if (toPosition.x == 0xFFFF)
		return { nullptr, g_game.resolveItem(player, toPosition, toStackPos, 0, STACKPOS_USETARGET) };

	auto tile = g_game.map.getTile(toPosition);

	if (not tile)
		return {};

	if (auto creature = tile->getTopVisibleCreature(player))
		return { creature, nullptr };

	return { nullptr, g_game.filterHangableItem(player, tile, tile->getUseItem(toStackPos)) };
}

ReturnValue ItemEvents::internalUseItem(PlayerPtr player, const Position& pos, uint8_t index, const ItemPtr& item, bool isHotkey)
{
	if (auto door = item->getDoor())
	{
		if (not door->canUse(player))
			return RETURNVALUE_NOTPOSSIBLE;
	}

	if (const ItemEvent* event = getEvent(item, BlackTek::ItemEvents::HookType::OnUse))
	{
		if (event->isScripted())
		{
			if (event->executeUse(player, item, pos, {}, pos, isHotkey))
				return RETURNVALUE_NOERROR;

			if (item->isRemoved())
				return RETURNVALUE_CANNOTUSETHISOBJECT;
		}
		else if (event->actionFunction and event->actionFunction(player, item, pos, {}, pos, isHotkey))
		{
			return RETURNVALUE_NOERROR;
		}
	}
	else if (RuneSpell* rune = g_spells->getRuneSpell(item->getID()))
	{
		if (rune->isScripted())
		{
			if (rune->executeUse(player, item, pos, {}, pos, isHotkey))
				return RETURNVALUE_NOERROR;

			if (item->isRemoved())
				return RETURNVALUE_CANNOTUSETHISOBJECT;
		}
	}

	if (BedItemPtr bed = item->getBed())
	{
		if (not bed->canUse(player))
		{
			if (not bed->getHouse())
				return RETURNVALUE_YOUCANNOTUSETHISBED;

			if (not player->isPremium())
				return RETURNVALUE_YOUNEEDPREMIUMACCOUNT;

			return RETURNVALUE_CANNOTUSETHISOBJECT;
		}

		if (bed->trySleep(player))
		{
			player->setBedItem(bed);

			if (g_config.GetBoolean(ConfigManager::BED_OFFLINE_TRAINING))
			{
				g_game.sendOfflineTrainingDialog(player);
			}
			else
			{
				auto bedItem = player->getBedItem();

				if (bedItem)
					bedItem->sleep(player);
			}
		}

		return RETURNVALUE_NOERROR;
	}

	if (auto container = item->getContainer())
	{
		ContainerPtr openContainer;

		if (container->isDepotLocker())
		{
			ContainerPtr& myDepotLocker = player->getDepotLocker();
			auto sourceLocation = container->getOwner()->getLocation();

			if (sourceLocation.tile)
				myDepotLocker->getOwner()->setTileParent(sourceLocation.tile);

			else if (sourceLocation.player)
				myDepotLocker->getOwner()->setInventoryOwner(sourceLocation.player);

			else
				myDepotLocker->getOwner()->clearParent();

			openContainer = myDepotLocker;
		}
		else
		{
			openContainer = container;
		}

		uint32_t corpseOwner = container->getOwner()->getCorpseOwner();
		if (container->isRewardCorpse())
		{
			auto& myRewardChest = player->getRewardChest();

			for (auto subItem : container->getItemList())
			{
				if (subItem->getID() == ITEM_REWARD_CONTAINER)
				{
					int64_t rewardDate = subItem->getIntAttr(ITEM_ATTRIBUTE_DATE);
					bool foundMatch = false;

					for (auto rewardItem : myRewardChest->getItemList())
					{
						if (rewardItem->getID() == ITEM_REWARD_CONTAINER and rewardItem->getIntAttr(ITEM_ATTRIBUTE_DATE) == rewardDate)
						{
							foundMatch = true;
							break;
						}
					}

					if (not foundMatch)
						return RETURNVALUE_NOTPOSSIBLE;
				}
			}
		}
		else if (corpseOwner != 0 and not player->canOpenCorpse(corpseOwner))
		{
			return RETURNVALUE_YOUARENOTTHEOWNER;
		}

		if (container->isRewardChest())
		{
			auto& myRewardChest = player->getRewardChest();
			auto sourceLocation = container->getOwner()->getLocation();

			{
				if (sourceLocation.tile)
					myRewardChest->getOwner()->setTileParent(sourceLocation.tile);

				else if (sourceLocation.player)
					myRewardChest->getOwner()->setInventoryOwner(sourceLocation.player);

				else
					myRewardChest->getOwner()->clearParent();
			}

			if (myRewardChest->getItemList().empty())
				return RETURNVALUE_REWARDCHESTEMPTY;

			for (auto rewardItem : myRewardChest->getItemList())
			{
				if (rewardItem->getID() == ITEM_REWARD_CONTAINER)
				{
					auto rewardContainer = rewardItem->getContainer();

					if (rewardContainer)
						rewardContainer->getOwner()->setContainerParent(myRewardChest->getOwner());
				}
			}

			openContainer = myRewardChest;
		}
		else if (item->getID() == ITEM_REWARD_CONTAINER)
		{
			auto& myRewardChest = player->getRewardChest();
			int64_t rewardDate = item->getIntAttr(ITEM_ATTRIBUTE_DATE);

			for (auto rewardItem : myRewardChest->getItemList())
			{
				if (rewardItem->getID() == ITEM_REWARD_CONTAINER and rewardItem->getIntAttr(ITEM_ATTRIBUTE_DATE) == rewardDate and rewardItem->getIntAttr(ITEM_ATTRIBUTE_REWARDID) == item->getIntAttr(ITEM_ATTRIBUTE_REWARDID))
				{
					if (const auto rewardContainer = rewardItem->getContainer())
					{
						rewardContainer->getOwner()->setContainerParent(myRewardChest->getOwner());
						openContainer = rewardContainer;
					}
					break;
				}
			}
		}

		int32_t oldContainerId = player->getContainerID(std::static_pointer_cast<const ItemContainer>(openContainer));

		if (oldContainerId == -1)
		{
			player->addContainer(index, openContainer);
			player->onSendContainer(openContainer);
		}
		else
		{
			player->onCloseContainer(openContainer);
			player->closeContainer(oldContainerId);
		}

		return RETURNVALUE_NOERROR;
	}

	const ItemType& it = Item::items[item->getID()];

	if (it.canReadText)
	{
		if (it.canWriteText)
		{
			player->setWriteItem(item, it.maxTextLen);
			player->sendTextWindow(item, it.maxTextLen, true);
		}
		else
		{
			player->setWriteItem(nullptr);
			player->sendTextWindow(item, 0, false);
		}

		return RETURNVALUE_NOERROR;
	}

	return RETURNVALUE_CANNOTUSETHISOBJECT;
}

bool ItemEvents::useItem(PlayerPtr player, const Position& pos, uint8_t index, const ItemPtr& item, bool isHotkey)
{
	player->setNextAction(OTSYS_TIME() + g_config.GetNumber(ConfigManager::ACTIONS_DELAY_INTERVAL));

	if (not item)
	{
		player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
		return false;
	}

	if (isHotkey)
	{
		uint16_t subType = item->getSubType();
		ShowUseHotkeyMessage(player, item, player->getItemTypeCount(item->getID(), subType != item->getItemCount() ? subType : -1));
	}

	if (g_config.GetBoolean(ConfigManager::ONLY_INVITED_CAN_MOVE_HOUSE_ITEMS))
	{
		if (auto groundTile = item->getTile(); groundTile and groundTile->isHouseTile())
		{
			ItemPtr topContainerItem = item;

			while (auto containerOwner = topContainerItem->getContainerParent())
				topContainerItem = containerOwner;

			const bool nested = (topContainerItem != item);
			const bool topParentIsPlayer = (not nested) and static_cast<bool>(topContainerItem->getLocation().player);

			if (not topParentIsPlayer and not groundTile->getHouse()->isInvited(player))
			{
				player->sendCancelMessage(RETURNVALUE_PLAYERISNOTINVITED);
				return false;
			}
		}
	}

	ReturnValue ret = internalUseItem(player, pos, index, item, isHotkey);

	if (ret == RETURNVALUE_YOUCANNOTUSETHISBED)
	{
		g_game.internalCreatureSay(player, TALKTYPE_MONSTER_SAY, getReturnMessage(ret), false);
		return false;
	}

	if (ret != RETURNVALUE_NOERROR)
	{
		player->sendCancelMessage(ret);
		return false;
	}

	return true;
}

bool ItemEvents::useItemEx(const PlayerPtr& player, const Position& fromPos, const Position& toPos, uint8_t toStackPos, const ItemPtr& item, bool isHotkey, const CreaturePtr& creature)
{
	player->setNextAction(OTSYS_TIME() + g_config.GetNumber(ConfigManager::EX_ACTIONS_DELAY_INTERVAL));

	const ItemEvent* event = getEvent(item, BlackTek::ItemEvents::HookType::OnUse);
	RuneSpell* rune = event ? nullptr : g_spells->getRuneSpell(item->getID());

	if (not event and not rune)
	{
		player->sendCancelMessage(RETURNVALUE_CANNOTUSETHISOBJECT);
		return false;
	}

	ReturnValue ret;
	bool ownErrorHandler;

	if (event)
	{
		ret = event->allowFarUse ? canUseFar(player, toPos, event->checkLineOfSight, event->checkFloor) : canUse(player, toPos);
		ownErrorHandler = false;
	}
	else
	{
		ret = rune->canExecuteAction(player, toPos);
		ownErrorHandler = rune->hasOwnErrorHandler();
	}

	if (ret != RETURNVALUE_NOERROR)
	{
		player->sendCancelMessage(ret);
		return false;
	}

	if (isHotkey)
	{
		uint16_t subType = item->getSubType();
		ShowUseHotkeyMessage(player, item, player->getItemTypeCount(item->getID(), subType != item->getItemCount() ? subType : -1));
	}

	if (g_config.GetBoolean(ConfigManager::ONLY_INVITED_CAN_MOVE_HOUSE_ITEMS))
	{
		if (auto groundTile = item->getTile(); groundTile and groundTile->isHouseTile())
		{
			ItemPtr topContainerItem = item;

			while (auto containerOwner = topContainerItem->getContainerParent())
				topContainerItem = containerOwner;

			const bool nested = (topContainerItem != item);
			const bool topParentIsPlayer = (not nested) and static_cast<bool>(topContainerItem->getLocation().player);

			if (not topParentIsPlayer and not groundTile->getHouse()->isInvited(player))
			{
				player->sendCancelMessage(RETURNVALUE_PLAYERISNOTINVITED);
				return false;
			}
		}
	}

	const BlackTek::GameModel useTarget = event
		? getUseTarget(player, creature, toPos, toStackPos)
		: rune->getTarget(player, creature, toPos, toStackPos);

	const bool used = event
		? event->executeUse(player, item, fromPos, useTarget, toPos, isHotkey)
		: rune->executeUse(player, item, fromPos, useTarget, toPos, isHotkey);

	if (used)
		return true;

	if (not ownErrorHandler)
		player->sendCancelMessage(RETURNVALUE_CANNOTUSETHISOBJECT);

	return false;
}
