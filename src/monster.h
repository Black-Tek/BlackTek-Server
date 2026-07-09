// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_MONSTER_H
#define FS_MONSTER_H

#include <bitset>

#include "tile.h"
#include "map.h"
#include "monsters.h"
#include <gtl/phmap.hpp>

class Creature;
class Game;
class Spawn;

using CreatureHashSet = gtl::flat_hash_set<CreaturePtr>;
using CreatureList = std::vector<CreatureWeakPtr>;

enum TargetSearchType_t {
	TARGETSEARCH_DEFAULT,
	TARGETSEARCH_RANDOM,
	TARGETSEARCH_ATTACKRANGE,
	TARGETSEARCH_NEAREST,
};

class Monster final : public Creature
{
	public:
		explicit Monster(MonsterType* mType);
		~Monster() override;

		// non-copyable
		Monster(const Monster&) = delete;
		Monster& operator=(const Monster&) = delete;

		// static
		static int32_t despawnRange;
		static int32_t despawnRadius;
		static uint32_t monsterAutoID;
		static MonsterPtr createMonster(const std::string& name);

		const std::string&	getName() const override;
		const std::string&	getNameDescription() const override;
		const std::string&	getRegisteredName() const override	{ return mType->registeredName; };
		std::string			getDescription(int32_t) override	{ return nameDescription + '.'; }

		MonsterPtr			getMonster() override				{ return static_shared_this<Monster>(); }
		MonsterConstPtr		getMonster() const override			{ return static_shared_this<const Monster>(); }

		CreatureType_t		getType(CreaturePtr caller = nullptr) const override;
		BlockType_t			blockHit(const CreaturePtr& attacker, CombatType_t combatType, int32_t& damage, bool checkDefense = false, bool checkArmor = false, bool field = false, bool ignoreResistances = false) override;
		RaceType_t			getRace() const override			{ return mType->info.race; }

		int32_t				getWalkCache(const Position& pos) const;
		int32_t				getArmor() const override			{ return mType->info.armor; }
		int32_t				getDefense() const override			{ return mType->info.defense; }

		uint32_t get_defense_charge_interval()	const noexcept override;
		uint32_t get_defense_charges_cap()		const noexcept override;
		uint32_t get_armor_charges_cap()		const noexcept override;
		uint32_t getManaCost() const							{ return mType->info.manaCost; }


		float get_defense_charge_cost_multiplier() const noexcept override;
		float get_armor_charge_cost_multiplier() const noexcept override;

		const Position& getMasterPos() const { return masterPos; }

		const CreatureList&		getTargetList() const		{ return targetList; }
		const CreatureHashSet&	getFriendList() const		{ return friendList; }
		CreatureHashSet&		getFriendList()				{ return friendList; }

		[[nodiscard]] bool canPushItems() const;
		[[nodiscard]] bool canSee(const Position& pos) const override;
		[[nodiscard]] bool canWalkOnFieldType(CombatType_t combatType) const;
		[[nodiscard]] bool walkToSpawn();
		[[nodiscard]] bool getNextStep(Direction& direction, uint32_t& flags) override;
		[[nodiscard]] bool challengeCreature(const CreaturePtr& creature, bool force = false) override;
		[[nodiscard]] bool getCombatValues(int32_t& min, int32_t& max) override;
		[[nodiscard]] bool searchTarget(TargetSearchType_t searchType = TARGETSEARCH_DEFAULT);
		[[nodiscard]] bool selectTarget(const CreaturePtr& creature);
		[[nodiscard]] bool canTarget(const CreatureConstPtr& creature) const;
		[[nodiscard]] bool followTargetFromDistance(const Position& targetPos, Direction& direction) noexcept;

		[[nodiscard]] bool isBoss(bool countReward = true) const	{ return mType->info.isBoss or ( countReward and mType->info.isRewardBoss); }
		[[nodiscard]] bool isPushable() const override				{ return mType->info.pushable and baseSpeed != 0; }
		[[nodiscard]] bool isAttackable() const override			{ return mType->info.isAttackable; }
		[[nodiscard]] bool canPushCreatures() const					{ return mType->info.canPushCreatures; }
		[[nodiscard]] bool isHostile() const						{ return mType->info.isHostile; }
		[[nodiscard]] bool isRewardBoss() const						{ return mType->info.isRewardBoss; }
		[[nodiscard]] bool canSeeInvisibility() const override		{ return isImmune(CONDITION_INVISIBLE); }
		[[nodiscard]] bool isWalkingToSpawn() const					{ return walkingToSpawn; }
		[[nodiscard]] bool hasExtraSwing() override					{ return lastMeleeAttack == 0; }
		[[nodiscard]] bool isFleeing() const						{ return not isSummon() and getHealth() <= mType->info.runAwayHealth and challengeFocusDuration <= 0; }
		[[nodiscard]] bool isTargetNearby() const					{ return stepDuration >= 1; }
		[[nodiscard]] bool isIgnoringFieldDamage() const			{ return ignoreFieldDamage; }

		void addList() override;
		void removeList() override;
		void setName(const std::string& name);

		void onAttackedCreatureDisappear(bool isLogout) override;
		void onCreatureAppear(const CreaturePtr& creature, bool isLogin) override;
		void onRemoveCreature(const CreaturePtr& creature, bool isLogout) override;
		void onCreatureMove(const CreaturePtr& creature, const TilePtr& newTile, const Position& newPos, const TilePtr& oldTile, const Position& oldPos, bool teleport) override;
		void onCreatureSay(const CreaturePtr& creature, SpeakClasses type, const std::string& text) override;

		void drainHealth(const CreaturePtr& attacker, int32_t damage) override;
		void changeHealth(int32_t healthChange, bool sendHealthChange = true) override;

		void onWalk() override;
		void onWalkComplete() override;
		void onFollowCreatureComplete(const CreatureConstPtr& creature) override;

		void onThink(uint32_t interval) override;
		void setNormalCreatureLight() override;
		void doAttacking(uint32_t interval) override;
		void fleeFromTarget(const Position& targetPos, Direction& direction) noexcept;
		void setIdle(bool idle);

		void setID() override										{ if (id == 0) id = monsterAutoID++; }
		void setNameDescription(const std::string& nameDescription) { this->nameDescription = nameDescription; };

		void setMasterPos(Position pos)								{ masterPos = pos; }
		void setSpawn(Spawn* spawn)									{ this->spawn = spawn; }

	private:

		static constexpr int32_t mapWalkWidth = Map::maxViewportX * 2 + 1;
		static constexpr int32_t mapWalkHeight = Map::maxViewportY * 2 + 1;
		static constexpr int32_t maxWalkCacheWidth = (mapWalkWidth - 1) / 2;
		static constexpr int32_t maxWalkCacheHeight = (mapWalkHeight - 1) / 2;

		static bool	pushItem(const ItemPtr& item);
		static void	pushItems(const TilePtr& tile);
		static bool	pushCreature(const CreaturePtr& creature);
		static void	pushCreatures(const TilePtr& tile);

		std::string name;
		std::string nameDescription;

		ItemPtr getCorpse(const CreaturePtr& lastHitCreature, const CreaturePtr& mostDamageCreature) override;

		MonsterType* mType;
		Spawn* spawn = nullptr;

		uint64_t getLostExperience() const override { return skillLoss ? mType->info.experience : 0; }

		int64_t lastMeleeAttack = 0;

		int32_t minCombatValue = 0;
		int32_t maxCombatValue = 0;
		int32_t targetChangeCooldown = 0;
		int32_t challengeFocusDuration = 0;
		int32_t stepDuration = 0;

		uint32_t attackTicks = 0;
		uint32_t targetTicks = 0;
		uint32_t targetChangeTicks = 0;
		uint32_t defenseTicks = 0;
		uint32_t yellTicks = 0;

		uint32_t getDamageImmunities() const override		{ return mType->info.damageImmunities; }
		uint32_t getConditionImmunities() const override	{ return mType->info.conditionImmunities; }

		uint16_t getLookCorpse() const override				{ return mType->info.lookcorpse; }

		Position masterPos;

		CreatureHashSet friendList;
		CreatureList targetList;
		gtl::flat_hash_set<CreaturePtr> targetSet;
		std::bitset<mapWalkHeight * mapWalkWidth> localMapCache;

		bool ignoreFieldDamage = false;
		bool isIdle = true;
		bool isMapLoaded = false;
		bool isMasterInRange = false;
		bool randomStepping = false;
		bool walkingToSpawn = false;

		bool canUseAttack(const Position& pos, const CreatureConstPtr& target) const;
		bool canUseSpell(const Position& pos, const Position& targetPos, const spellBlock_t& sb, uint32_t interval, bool& inRange, bool& resetTicks) const;

		bool getRandomStep(const Position& creaturePos, Direction& direction);
		bool getDanceStep(const Position& creaturePos, Direction& direction, bool keepAttack = true, bool keepDistance = true);
		bool isInSpawnRange(const Position& pos) const;
		bool canWalkTo(Position pos, Direction direction);

		bool isFriend(const CreatureConstPtr& creature) const;
		bool isOpponent(const CreatureConstPtr& creature) const;

		bool getIdleStatus() const	{ return isIdle; }
		bool useCacheMap() const	{ return not randomStepping; }

		void onCreatureEnter(const CreaturePtr& creature);
		void onCreatureLeave(const CreaturePtr& creature);
		void onCreatureFound(const CreaturePtr& creature, bool pushFront = false);
		void updateLookDirection();

		void addFriend(const CreaturePtr& creature);
		void removeFriend(const CreaturePtr& creature);
		void addTarget(const CreaturePtr& creature, bool pushFront = false);
		void removeTarget(const CreaturePtr& creature);
		void updateTargetList();
		void clearTargetList();
		void clearFriendList();

		void death(const CreaturePtr& lastHitCreature) override;
		void updateIdleStatus();
		void onAddCondition(ConditionType_t type) override;
		void onEndCondition(ConditionType_t type) override;

		void updateMapCache();
		void updateTileCache(TilePtr tile, int32_t dx, int32_t dy);
		void updateTileCache(const TilePtr& tile, int32_t dx, int32_t dy, const MonsterPtr& self);
		void updateTileCache(TilePtr tile, const Position& pos);

		void onThinkTarget(uint32_t interval);
		void onThinkYell(uint32_t interval);
		void onThinkDefense(uint32_t interval);

		void dropLoot(const ContainerPtr& corpse, const CreaturePtr& lastHitCreature) override;
		void getPathSearchParams(const CreatureConstPtr& creature, FindPathParams& fpp) const override;

		friend class LuaScriptInterface;
};

#endif
