#ifndef BT_DECLARATIONS_H
#define BT_DECLARATIONS_H
#include <memory>
#include <vector>


class Thing;
using ThingPtr = std::shared_ptr<class Thing>;
using ThingConstPtr = std::shared_ptr<const Thing>;

class Creature;
using CreaturePtr = std::shared_ptr<Creature>;
using CreatureConstPtr = std::shared_ptr<const Creature>;
using CreatureWeakPtr = std::weak_ptr<Creature>;

class Player;
using PlayerPtr = std::shared_ptr<Player>;
using PlayerConstPtr = std::shared_ptr<const Player>;

class Monster;
using MonsterPtr = std::shared_ptr<Monster>;
using MonsterConstPtr = std::shared_ptr<const Monster>;

class Npc;
using NpcPtr = std::shared_ptr<Npc>;
using NpcConstPtr = std::shared_ptr<const Npc>;

class Tile;
using TilePtr = std::shared_ptr<Tile>;
using TileConstPtr = std::shared_ptr<const Tile>;
using TileWeakPtr = std::weak_ptr<Tile>;

class Cylinder;
using CylinderPtr = std::shared_ptr<Cylinder>;
using CylinderConstPtr = std::shared_ptr<const Cylinder>;
using CylinderWeakPtr = std::weak_ptr<Cylinder>;

class Item;
using ItemPtr = std::shared_ptr<Item>;
using ItemConstPtr = std::shared_ptr<const Item>;

class Container;
using ContainerPtr = std::shared_ptr<Container>;
using ContainerConstPtr = std::shared_ptr<const Container>;

class Depot;
using DepotPtr = std::shared_ptr<Depot>;
using DepotConstPtr = std::shared_ptr<const Depot>;

class Teleport;
using TeleportPtr = std::shared_ptr<Teleport>;
using TeleportConstPtr = std::shared_ptr<const Teleport>;

class TrashHolder;
using TrashHolderPtr = std::shared_ptr<TrashHolder>;
using TrashHolderConstPtr = std::shared_ptr<const TrashHolder>;

class Mailbox;
using MailboxPtr = std::shared_ptr<Mailbox>;
using MailboxConstPtr = std::shared_ptr<const Mailbox>;

class Door;
using DoorPtr = std::shared_ptr<Door>;
using DoorConstPtr = std::shared_ptr<const Door>;

class MagicField;
using MagicFieldPtr = std::shared_ptr<MagicField>;
using MagicFieldConstPtr = std::shared_ptr<const MagicField>;

class BedItem;
using BedItemPtr = std::shared_ptr<BedItem>;
using BedItemConstPtr = std::shared_ptr<const BedItem>;

class HouseTile;
using HouseTilePtr = std::shared_ptr<Tile>;
using HouseTileConstPtr = std::shared_ptr<const Tile>;

class DepotLocker;
using DepotLockerPtr = std::shared_ptr<DepotLocker>;
using DepotLockerConstPtr = std::shared_ptr<const DepotLocker>;

class StoreInbox;
using StoreInboxPtr = std::shared_ptr<StoreInbox>;
using StoreInboxConstPtr = std::shared_ptr<const StoreInbox>;

class RewardChest;
using RewardChestPtr = std::shared_ptr<RewardChest>;
using RewardChestConstPtr = std::shared_ptr<const RewardChest>;

class Party;
using PartyPtr = std::shared_ptr<Party>;
using PartyConsPtr = std::shared_ptr<const Party>;

/// Object Containers
class TileItemVector;
using CreatureVector = std::vector<CreaturePtr>;
using ItemVector = std::vector<ItemPtr>;
using TileItemsPtr = std::shared_ptr<TileItemVector>;
using TileItemsConstPtr = std::shared_ptr<const TileItemVector>;
using TileCreaturesPtr = std::shared_ptr<CreatureVector>;
using TileCreaturesConstPtr = std::shared_ptr<const CreatureVector>;


#endif