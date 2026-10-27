/**
 * BlackTek Server Engine
 * https://github.com/Black-Tek/BlackTek-Server
 *
 * ============================================================================
 * COPYRIGHT & ATTRIBUTION NOTICE
 * ============================================================================
 *
 * Copyright (C) 2024-2026 BlackTek (Author: Codinablack)
 *
 * This file is an original BlackTek contribution, created since this project's
 * inception, and contains no code inherited from The Forgotten Server.
 * It is licensed solely under the MIT License (see the licensing/License file
 * in the project root for the full text).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
