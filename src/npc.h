// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_NPC_H
#define FS_NPC_H

#include "creature.h"
#include "luascript.h"

#include <set>

class Npc;
class Player;

class Npcs
{
	public:
		static void reload();
};

class NpcScriptInterface final : public LuaScriptInterface
{
	public:
		NpcScriptInterface();

		bool loadNpcLib(const std::string& file);

	private:
		void registerFunctions() const;

		static int luaActionSay(lua_State* L);
		static int luaActionMove(lua_State* L);
		static int luaActionMoveTo(lua_State* L);
		static int luaActionTurn(lua_State* L);
		static int luaActionFollow(lua_State* L);
		static int luagetDistanceTo(lua_State* L);
		static int luaSetNpcFocus(lua_State* L);
		static int luaGetNpcCid(lua_State* L);
		static int luaGetNpcParameter(lua_State* L);
		static int luaOpenShopWindow(lua_State* L);
		static int luaCloseShopWindow(lua_State* L);
		static int luaDoSellItem(lua_State* L);

		// metatable
		static int luaNpcGetParameter(lua_State* L);
		static int luaNpcSetFocus(lua_State* L);

		static int luaNpcOpenShopWindow(lua_State* L);
		static int luaNpcCloseShopWindow(lua_State* L);

	private:
		bool initState() override;
		bool closeState() override;

		bool libLoaded;
};

class NpcEventsHandler
{
	public:
		NpcEventsHandler(const std::string& file, NpcPtr npc);

		std::unique_ptr<NpcScriptInterface> scriptInterface;

		void onCreatureAppear(const CreaturePtr& creature);
		void onCreatureDisappear(const CreaturePtr& creature);
		void onCreatureMove(const CreaturePtr& creature, const Position& oldPos, const Position& newPos);
		void onCreatureSay(const CreaturePtr& creature, SpeakClasses, const std::string& text);
		void onPlayerTrade(const PlayerPtr& player, int32_t callback, uint16_t itemId, uint8_t count, uint8_t amount, bool ignore = false, bool inBackpacks = false);
		void onPlayerCloseChannel(const PlayerPtr& player);
		void onPlayerEndTrade(const PlayerPtr& player);
		void onThink();

		bool isLoaded() const;

	private:

		NpcPtr npc;
		int32_t creatureAppearEvent = -1;
		int32_t creatureDisappearEvent = -1;
		int32_t creatureMoveEvent = -1;
		int32_t creatureSayEvent = -1;
		int32_t playerCloseChannelEvent = -1;
		int32_t playerEndTradeEvent = -1;
		int32_t thinkEvent = -1;
		bool loaded = false;
};

class Npc final : public Creature
{
	public:
		explicit Npc(const std::string& name);
		~Npc() override;

		// non-copyable
		Npc(const Npc&) = delete;
		Npc& operator=(const Npc&) = delete;

		NpcPtr getNpc() override {
			return dynamic_shared_this<Npc>();
		}
	
		NpcConstPtr getNpc() const override {
			return dynamic_shared_this<Npc>();
		}

		bool isPushable() const override {
			return pushable && walkTicks != 0;
		}

		bool isPhaseable() const {
			return phaseable;
		}

		void setID() override {
			if (id == 0) {
				id = ++npcAutoID;
			}
		}

		void removeList() override;
		void addList() override;
		void makePhaseable() {
			phaseable = true;
		}

		static NpcPtr createNpc(const std::string& name);

		bool canSee(const Position& pos) const override;
		bool isPlayer() const override { return false; }
		bool isMonster() const override { return false; }
		bool isNpc() const override { return true; }

		bool load();
		void reload();

		const std::string& getRegisteredName() const override {
			return getName();
		}

		const std::string& getName() const override {
			return name;
		}
	
		const std::string& getNameDescription() const override {
			return name;
		}

		CreatureType_t getType() const override {
			return CREATURETYPE_NPC;
		}

		uint8_t getSpeechBubble() const override {
			return speechBubble;
		}
	
		void setSpeechBubble(const uint8_t bubble) {
			speechBubble = bubble;
		}

		const auto& getSpectators() { 
			return spectators; 
		}

		void doSay(const std::string& text);
		void doSayToPlayer(const PlayerPtr& player, const std::string& text);

		bool doMoveTo(const Position& pos, int32_t minTargetDist = 1, int32_t maxTargetDist = 1,
		              bool fullPathSearch = true, bool clearSight = true, int32_t maxSearchDist = 0);

		int32_t getMasterRadius() const {
			return masterRadius;
		}
	
		const Position& getMasterPos() const {
			return masterPos;
		}
	
		void setMasterPos(Position pos, int32_t radius = 1) {
			masterPos = pos;
			if (masterRadius == -1) {
				masterRadius = radius;
			}
		}

		void onPlayerCloseChannel(const PlayerPtr& player) const;
		void onPlayerTrade(const PlayerPtr& player, int32_t callback, uint16_t itemId, uint8_t count,
		                   uint8_t amount, bool ignore = false, bool inBackpacks = false) const;
		void onPlayerEndTrade(const PlayerPtr& player, int32_t buyCallback, int32_t sellCallback);

		void turnToCreature(const CreaturePtr& creature);
		void setCreatureFocus(const CreaturePtr& creature);

		auto& getScriptInterface() const { return npcEventHandler->scriptInterface; }

		static uint32_t npcAutoID;

	private:
		

		void onCreatureAppear(const CreaturePtr& creature, bool isLogin) override;
		void onRemoveCreature(const CreaturePtr& creature, bool isLogout) override;
		void onCreatureMove(const CreaturePtr& creature, const TilePtr& newTile, const Position& newPos,
		                            const TilePtr& oldTile, const Position& oldPos, bool teleport) override;

		void onCreatureSay(const CreaturePtr& creature, SpeakClasses type, const std::string& text) override;
		void onThink(uint32_t interval) override;
		std::string getDescription(int32_t lookDistance) const override;

		bool isImmune(CombatType_t) const override {
			return !attackable;
		}
	
		bool isImmune(ConditionType_t) const override {
			return !attackable;
		}
	
		bool isAttackable() const override {
			return attackable;
		}
	
		bool getNextStep(Direction& dir, uint32_t& flags) override;

		void setIdle(const bool idle);

		bool canWalkTo(const Position& fromPos, Direction dir);
		bool getRandomStep(Direction& direction);

		void reset();
		bool loadFromXml();

		void addShopPlayer(const PlayerPtr& player);
		void removeShopPlayer(const PlayerPtr& player);
		void closeAllShopWindows();

		std::map<std::string, std::string> parameters;

		std::set<PlayerPtr> shopPlayerSet;
		std::set<PlayerPtr> spectators;

		std::string name;
		std::string filename;

		std::unique_ptr<NpcEventsHandler> npcEventHandler;

		Position masterPos;

		uint32_t walkTicks;
		int32_t focusCreature;
		int32_t masterRadius;

		uint8_t speechBubble;

		bool floorChange;
		bool attackable;
		bool ignoreHeight;
		bool loaded;
		bool isIdle;
		bool pushable;
		bool phaseable = false;

		friend class Npcs;
		friend class NpcScriptInterface;
};

#endif
