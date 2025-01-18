// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_MOVEMENT_H_
#define FS_MOVEMENT_H_

#include "baseevents.h"
#include "item.h"
#include "luascript.h"
#include "vocation.h"

extern Vocations g_vocations;

enum MoveEvent_t {
	MOVE_EVENT_STEP_IN,
	MOVE_EVENT_STEP_OUT,
	MOVE_EVENT_EQUIP,
	MOVE_EVENT_DEEQUIP,
	MOVE_EVENT_ADD_ITEM,
	MOVE_EVENT_REMOVE_ITEM,
	MOVE_EVENT_ADD_ITEM_ITEMTILE,
	MOVE_EVENT_REMOVE_ITEM_ITEMTILE,

	MOVE_EVENT_LAST,
	MOVE_EVENT_NONE
};

class MoveEvent;
using MoveEvent_ptr = std::unique_ptr<MoveEvent>;

struct MoveEventList {
	std::list<MoveEvent> moveEvent[MOVE_EVENT_LAST];
};

using VocEquipMap = std::map<uint16_t, bool>;

class MoveEvents final : public BaseEvents
{
	public:
		MoveEvents();
		~MoveEvents() override;

		// non-copyable
		MoveEvents(const MoveEvents&) = delete;
		MoveEvents& operator=(const MoveEvents&) = delete;

		uint32_t onCreatureMove(const CreaturePtr& creature, const TilePtr& tile, MoveEvent_t eventType);
		ReturnValue onPlayerEquip(const PlayerPtr& player, const ItemPtr& item, slots_t slot, bool isCheck);
		ReturnValue onPlayerDeEquip(const PlayerPtr& player, const ItemPtr& item, slots_t slot);
		uint32_t onItemMove(const ItemPtr& item, const TilePtr& tile, bool isAdd);

		MoveEvent* getEvent(const ItemPtr& item, MoveEvent_t eventType);

		bool registerLuaEvent(MoveEvent* event);
		bool registerLuaFunction(MoveEvent* event);
		void clear(bool fromLua) override final;

	private:
		using MoveListMap = std::map<int32_t, MoveEventList>;
		using MovePosListMap = std::map<Position, MoveEventList>;
		void clearMap(MoveListMap& map, bool fromLua);
		void clearPosMap(MovePosListMap& map, bool fromLua);

		LuaScriptInterface& getScriptInterface() override;
		std::string_view getScriptBaseName() const override { return "movements"; }
		Event_ptr getEvent(const std::string& nodeName) override;
		bool registerEvent(Event_ptr event, const pugi::xml_node& node) override;

		void addEvent(MoveEvent moveEvent, int32_t id, MoveListMap& map);

		void addEvent(MoveEvent moveEvent, const Position& pos, MovePosListMap& map);
		MoveEvent* getEvent(const TileConstPtr& tile, MoveEvent_t eventType);

		MoveEvent* getEvent(const ItemPtr& item, MoveEvent_t eventType, slots_t slot);

		MoveListMap uniqueIdMap;
		MoveListMap actionIdMap;
		MoveListMap itemIdMap;
		MovePosListMap positionMap;

		LuaScriptInterface scriptInterface;
};

using StepFunction = std::function<uint32_t(const CreaturePtr& creature, const ItemPtr& item, const Position& pos)>;
using MoveFunction = std::function<uint32_t(const ItemPtr& item, const ItemPtr& tileItem, const Position& pos)>;
using EquipFunction = std::function<ReturnValue(MoveEvent* moveEvent, const PlayerPtr& player, const ItemPtr& item, slots_t slot, bool boolean)>;

class MoveEvent final : public Event
{
	public:
		explicit MoveEvent(LuaScriptInterface* interface);

		MoveEvent_t getEventType() const;
		void setEventType(MoveEvent_t type);

		bool configureEvent(const pugi::xml_node& node) override;
		bool loadFunction(const pugi::xml_attribute& attr, bool isScripted) override;

		uint32_t fireStepEvent(const CreaturePtr& creature, const ItemPtr& item, const Position& pos);
		uint32_t fireAddRemItem(const ItemPtr& item, const ItemPtr& tileItem, const Position& pos);
		ReturnValue fireEquip(const PlayerPtr& player, const ItemPtr& item, slots_t slot, bool isCheck);

		uint32_t getSlot() const {
			return slot;
		}

		//scripting
		bool executeStep(const CreaturePtr& creature, const ItemPtr& item, const Position& pos);
		bool executeEquip(const PlayerPtr& player, const ItemPtr& item, slots_t slot, bool isCheck);
		bool executeAddRemItem(const ItemPtr& item, const ItemPtr& tileItem, const Position& pos);
		//

		//onEquip information
		uint32_t getReqLevel() const {
			return reqLevel;
		}
	
		uint32_t getReqMagLv() const {
			return reqMagLevel;
		}
	
		bool isPremium() const {
			return premium;
		}
	
		const std::string& getVocationString() const {
			return vocationString;
		}
	
		void setVocationString(const std::string& str) {
			vocationString = str;
		}
	
		uint32_t getWieldInfo() const {
			return wieldInfo;
		}
	
		const VocEquipMap& getVocEquipMap() const {
			return vocEquipMap;
		}
	
		void addVocEquipMap(const std::string& vocName) {
			int32_t vocationId = g_vocations.getVocationId(vocName);
			if (vocationId != -1) {
				vocEquipMap[vocationId] = true;
			}
		}
	
		bool getTileItem() const {
			return tileItem;
		}
	
		void setTileItem(const bool b) {
			tileItem = b;
		}
	
		void clearItemIdRange() {
			return itemIdRange.clear();
		}
	
		const std::vector<uint32_t>& getItemIdRange() const {
			return itemIdRange;
		}
	
		void addItemId(uint32_t id) {
			itemIdRange.emplace_back(id);
		}
	
		void clearActionIdRange() {
			return actionIdRange.clear();
		}
	
		const std::vector<uint32_t>& getActionIdRange() const {
			return actionIdRange;
		}
	
		void addActionId(uint32_t id) {
			actionIdRange.emplace_back(id);
		}
	
		void clearUniqueIdRange() {
			return uniqueIdRange.clear();
		}
	
		const std::vector<uint32_t>& getUniqueIdRange() const {
			return uniqueIdRange;
		}
	
		void addUniqueId(uint32_t id) {
			uniqueIdRange.emplace_back(id);
		}
	
		void clearPosList() {
			return posList.clear();
		}
	
		const std::vector<Position>& getPosList() const {
			return posList;
		}
	
		void addPosList(Position pos) {
			posList.emplace_back(pos);
		}
	
		void setSlot(uint32_t s) {
			slot = s;
		}
	
		uint32_t getRequiredLevel() const
		{
			return reqLevel;
		}
	
		void setRequiredLevel(uint32_t level) {
			reqLevel = level;
		}
	
		uint32_t getRequiredMagLevel() const
		{
			return reqMagLevel;
		}
	
		void setRequiredMagLevel(const uint32_t level) {
			reqMagLevel = level;
		}
	
		bool needPremium() const
		{
			return premium;
		}
	
		void setNeedPremium(const bool b) {
			premium = b;
		}
	
		uint32_t getWieldInfo() {
			return wieldInfo;
		}
	
		void setWieldInfo(const WieldInfo_t info) {
			wieldInfo |= info;
		}

		static uint32_t StepInField(const CreaturePtr& creature, const ItemPtr& item, const Position& pos);
		static uint32_t StepOutField(const CreaturePtr& creature, const ItemPtr& item, const Position& pos);

		static uint32_t AddItemField(const ItemPtr& item, const ItemPtr& tileItem, const Position& pos);
		static uint32_t RemoveItemField(const ItemPtr& item, const ItemPtr& tileItem, const Position& pos);

		static ReturnValue EquipItem(MoveEvent* moveEvent, const PlayerPtr& player, const ItemPtr& item, slots_t slot, bool isCheck);
		static ReturnValue DeEquipItem(MoveEvent* moveEvent, const PlayerPtr& player, const ItemPtr& item, slots_t slot, bool);

		MoveEvent_t eventType = MOVE_EVENT_NONE;
		StepFunction stepFunction;
		MoveFunction moveFunction;
		EquipFunction equipFunction;

	private:
		std::string_view getScriptEventName() const override;

		uint32_t slot = SLOTP_WHEREEVER;

		//onEquip information
		uint32_t reqLevel = 0;
		uint32_t reqMagLevel = 0;
		bool premium = false;
		std::string vocationString;
		uint32_t wieldInfo = 0;
		VocEquipMap vocEquipMap;
		bool tileItem = false;

		std::vector<uint32_t> itemIdRange;
		std::vector<uint32_t> actionIdRange;
		std::vector<uint32_t> uniqueIdRange;
		std::vector<Position> posList;
};

#endif
