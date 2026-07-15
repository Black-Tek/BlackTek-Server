#pragma once
#include <memory>
#include <vector>

class Creature;
using CreaturePtr = std::shared_ptr<Creature>;
using CreatureConstPtr = std::shared_ptr<const Creature>;
using CreatureWeakPtr = std::weak_ptr<Creature>;

class Player;
using PlayerPtr = std::shared_ptr<Player>;
using PlayerConstPtr = std::shared_ptr<const Player>;
using PlayerWeakPtr = std::weak_ptr<Player>;

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

class CreatureContainer;

class Item;
using ItemPtr = std::shared_ptr<Item>;
using ItemConstPtr = std::shared_ptr<const Item>;
using ItemWeakPtr = std::weak_ptr<Item>;

class ItemContainer;
using ContainerPtr = std::shared_ptr<ItemContainer>;
using ContainerConstPtr = std::shared_ptr<const ItemContainer>;

class Depot;
using DepotPtr = std::shared_ptr<Depot>;
using DepotConstPtr = std::shared_ptr<const Depot>;

using TeleportPtr = ItemPtr;
using TeleportConstPtr = ItemConstPtr;

using TrashHolderPtr = ItemPtr;
using TrashHolderConstPtr = ItemConstPtr;

using MailboxPtr = ItemPtr;
using MailboxConstPtr = ItemConstPtr;

using DoorPtr = ItemPtr;
using DoorConstPtr = ItemConstPtr;

using MagicFieldPtr = ItemPtr;
using MagicFieldConstPtr = ItemConstPtr;

using BedItemPtr = ItemPtr;
using BedItemConstPtr = ItemConstPtr;

class HouseTile;
using HouseTilePtr = std::shared_ptr<Tile>;
using HouseTileConstPtr = std::shared_ptr<const Tile>;

/// Object Containers
class TileItemVector;
using CreatureVector = std::vector<CreaturePtr>;
using ItemVector = std::vector<ItemPtr>;
using TileItemsPtr = std::shared_ptr<TileItemVector>;
using TileItemsConstPtr = std::shared_ptr<const TileItemVector>;
