// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_SPECTATORS_H
#define FS_SPECTATORS_H

#include <vector>

class Creature;

class SpectatorVec
{
	using Vec = std::vector<Creature*>;
	using Iterator = Vec::iterator;
	using ConstIterator = Vec::const_iterator;
public:
	SpectatorVec() {
		vec.reserve(32);
	}

	void addSpectators(const SpectatorVec& spectators) {
		for (Creature* spectator : spectators.vec) {
			auto it = std::find(vec.begin(), vec.end(), spectator);
			if (it != end()) {
				continue;
			}
			vec.emplace_back(spectator);
		}
	}

	void erase(Creature* spectator) {
		auto it = std::find(vec.begin(), vec.end(), spectator);
		if (it == end()) {
			return;
		}
		std::iter_swap(it, end() - 1);
		vec.pop_back();
	}

	Creature* operator[] (uint8_t index) {
		return vec[index];
	}

	size_t size() const { return vec.size(); }
	bool empty() const { return vec.empty(); }
	Iterator begin() { return vec.begin(); }
	ConstIterator begin() const { return vec.begin(); }
	Iterator end() { return vec.end(); }
	ConstIterator end() const { return vec.end(); }
	void emplace_back(Creature* c) { vec.emplace_back(c); }

private:
	Vec vec;
};

#endif
