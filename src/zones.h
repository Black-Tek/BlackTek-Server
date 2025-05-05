// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

// alex rider

#include "otpch.h"
#include "position.h"

struct Zone {
	Zone() : id(0) {}
	Zone(int id) : id(id) {};
	int id = 0;
	std::vector<Position> positions{};
};

class Zones {
public:
	static bool registerZone(Zone zone);
	static Zone& createZone(int id, std::vector<Position> positions);
	static void load();
	static void clear();
	static void reload();
	static size_t count();
	static std::vector<Zone>& get();
	static Zone& getZone(int id);
	static std::vector<int> getZonesByPosition(const Position& position);
};