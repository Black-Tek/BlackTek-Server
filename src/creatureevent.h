// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_CREATUREEVENT_H
#define FS_CREATUREEVENT_H

#include "baseevents.h"
#include "enums.h"
#include "luascript.h"

class CreatureEvent;
using CreatureEvent_ptr = std::unique_ptr<CreatureEvent>;

enum CreatureEventType_t {
	CREATURE_EVENT_NONE,
	CREATURE_EVENT_LOGIN,
	CREATURE_EVENT_LOGOUT,
	CREATURE_EVENT_THINK,
	CREATURE_EVENT_PREPAREDEATH,
	CREATURE_EVENT_DEATH,
	CREATURE_EVENT_KILL,
	CREATURE_EVENT_ADVANCE,
	CREATURE_EVENT_MODALWINDOW,
	CREATURE_EVENT_TEXTEDIT,
	CREATURE_EVENT_HEALTHCHANGE,
	CREATURE_EVENT_MANACHANGE,
	CREATURE_EVENT_EXTENDED_OPCODE, // otclient additional network opcodes
};

class CreatureEvent final : public Event
{
	public:
		explicit CreatureEvent(LuaScriptInterface* interface);

		bool configureEvent(const pugi::xml_node& node) override;

		CreatureEventType_t getEventType() const {
			return type;
		}
		void setEventType(CreatureEventType_t eventType) {
			type = eventType;
		}
		const std::string& getName() const {
			return eventName;
		}
		void setName(const std::string& name) {
			eventName = name;
		}
		bool isLoaded() const {
			return loaded;
		}
		void setLoaded(bool b) {
			loaded = b;
		}

		void clearEvent();
		void copyEvent(const CreatureEvent_ptr& creatureEvent);

		//scripting
		bool executeOnLogin(const PlayerPtr& player) const;
		bool executeOnLogout(const PlayerPtr& player) const;
		bool executeOnThink(const CreaturePtr& creature, uint32_t interval) const;
		bool executeOnPrepareDeath(const CreaturePtr& creature, const CreaturePtr& killer) const;
		bool executeOnDeath(const CreaturePtr& creature, const ItemPtr& corpse, const CreaturePtr& killer, const CreaturePtr& mostDamageKiller, bool lastHitUnjustified, bool mostDamageUnjustified) const;
		void executeOnKill(const CreaturePtr& creature, const CreaturePtr& target) const;
		bool executeAdvance(const PlayerPtr& player, skills_t, uint32_t, uint32_t) const;
		void executeModalWindow(const PlayerPtr& player, uint32_t modalWindowId, uint8_t buttonId, uint8_t choiceId) const;
		bool executeTextEdit(const PlayerPtr& player, const ItemPtr& item, const std::string& text) const;
		void executeHealthChange(const CreaturePtr& creature, const CreaturePtr& attacker, CombatDamage& damage) const;
		void executeManaChange(const CreaturePtr& creature, const CreaturePtr& attacker, CombatDamage& damage) const;
		void executeExtendedOpcode(const PlayerPtr& player, uint8_t opcode, const std::string& buffer) const;
		//

	private:
		std::string_view getScriptEventName() const override;

		std::string eventName;
		CreatureEventType_t type;
		bool loaded;
};

class CreatureEvents final : public BaseEvents
{
	public:
		CreatureEvents();

		// non-copyable
		CreatureEvents(const CreatureEvents&) = delete;
		CreatureEvents& operator=(const CreatureEvents&) = delete;

		// global events
		bool playerLogin(const PlayerPtr& player) const;
		bool playerLogout(const PlayerPtr& player) const;
		bool playerAdvance(const PlayerPtr& player, skills_t, uint32_t, uint32_t);

		CreatureEvent* getEventByName(const std::string& name, bool forceLoaded = true);

		bool registerLuaEvent(CreatureEvent* event);
		void clear(bool fromLua) override final;

		void removeInvalidEvents();

	private:
		LuaScriptInterface& getScriptInterface() override;
		std::string_view getScriptBaseName() const override { return "creaturescripts"; }
		Event_ptr getEvent(const std::string& nodeName) override;
		bool registerEvent(Event_ptr event, const pugi::xml_node& node) override;

		//creature events
		using CreatureEventMap = std::map<std::string, CreatureEvent>;
		CreatureEventMap creatureEvents;

		LuaScriptInterface scriptInterface;
};

#endif
