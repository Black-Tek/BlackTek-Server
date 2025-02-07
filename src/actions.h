// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_ACTIONS_H
#define FS_ACTIONS_H

#include "baseevents.h"
#include "enums.h"
#include "luascript.h"

class Action;
using Action_ptr = std::unique_ptr<Action>;
using ActionFunction = std::function<bool(PlayerPtr player, ItemPtr item, const Position& fromPosition, ThingPtr target, const Position& toPosition, bool isHotkey)>;

class Action : public Event
{
	public:
		explicit Action(LuaScriptInterface* interface);

		bool configureEvent(const pugi::xml_node& node) override;
		bool loadFunction(const pugi::xml_attribute& attr, bool isScripted) override;

		//scripting
		virtual bool executeUse(const PlayerPtr& player, const ItemPtr& item, const Position& fromPosition,
			const ThingPtr& target, const Position& toPosition, bool isHotkey);

		bool getAllowFarUse() const {
			return allowFarUse;
		}
	
		void setAllowFarUse(bool v) {
			allowFarUse = v;
		}

		bool getCheckLineOfSight() const {
			return checkLineOfSight;
		}
	
		void setCheckLineOfSight(bool v) {
			checkLineOfSight = v;
		}

		bool getCheckFloor() const {
			return checkFloor;
		}
	
		void setCheckFloor(bool v) {
			checkFloor = v;
		}

		void clearItemIdRange() {
			return ids.clear();
		}
	
		const std::vector<uint16_t>& getItemIdRange() const {
			return ids;
		}
	
		void addItemId(uint16_t id) {
			ids.emplace_back(id);
		}

		void clearUniqueIdRange() {
			return uids.clear();
		}
	
		const std::vector<uint16_t>& getUniqueIdRange() const {
			return uids;
		}
	
		void addUniqueId(uint16_t id) {
			uids.emplace_back(id);
		}

		void clearActionIdRange() {
			return aids.clear();
		}
	
		const std::vector<uint16_t>& getActionIdRange() const {
			return aids;
		}
	
		void addActionId(uint16_t id) {
			aids.emplace_back(id);
		}

		virtual ReturnValue canExecuteAction(const PlayerConstPtr& player, const Position& toPos);
	
		virtual bool hasOwnErrorHandler() {
			return false;
		}
	
		virtual ThingPtr getTarget(const PlayerPtr& player, const CreaturePtr& targetCreature, const Position& toPosition, uint8_t toStackPos) const;

		ActionFunction function;

	private:
		std::string_view getScriptEventName() const override { return "onUse"; }

		bool allowFarUse = false;
		bool checkFloor = true;
		bool checkLineOfSight = true;
		std::vector<uint16_t> ids;
		std::vector<uint16_t> uids;
		std::vector<uint16_t> aids;
};

class Actions final : public BaseEvents
{
	public:
		Actions();
		~Actions();

		// non-copyable
		Actions(const Actions&) = delete;
		Actions& operator=(const Actions&) = delete;

		bool useItem(PlayerPtr player, const Position& pos, uint8_t index, const ItemPtr& item, bool isHotkey);
		bool useItemEx(const PlayerPtr& player, const Position& fromPos, const Position& toPos, uint8_t toStackPos, const ItemPtr& item, bool isHotkey, const CreaturePtr& creature = nullptr);

		ReturnValue canUse(const PlayerConstPtr& player, const Position& pos);
		ReturnValue canUse(const PlayerConstPtr& player, const Position& pos, const ItemConstPtr& item);
		ReturnValue canUseFar(const CreatureConstPtr& creature, const Position& toPos, bool checkLineOfSight, bool checkFloor);

		bool registerLuaEvent(Action* event);
		void clear(bool fromLua) override final;

	private:
		ReturnValue internalUseItem(PlayerPtr player, const Position& pos, uint8_t index, const ItemPtr& item, bool isHotkey);

		LuaScriptInterface& getScriptInterface() override;
		std::string_view getScriptBaseName() const override { return "actions"; }
		Event_ptr getEvent(const std::string& nodeName) override;
		bool registerEvent(Event_ptr event, const pugi::xml_node& node) override;

		using ActionUseMap = std::map<uint16_t, Action>;
		ActionUseMap useItemMap;
		ActionUseMap uniqueItemMap;
		ActionUseMap actionItemMap;

		Action* getAction(const ItemConstPtr& item);
		void clearMap(ActionUseMap& map, bool fromLua);

		LuaScriptInterface scriptInterface;
};

#endif
