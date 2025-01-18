// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_SPECTATORS_H
#define FS_SPECTATORS_H

#include <vector>

class Creature;

class SpectatorVec
{
	using Vec = std::vector<CreaturePtr>;
	using Iterator = Vec::iterator;
	using ConstIterator = Vec::const_iterator;
public:
	SpectatorVec() {
		vec.reserve(32);
	}

	void addSpectators(const SpectatorVec& spectators) {
		for (CreaturePtr spectator : spectators.vec) {
			if (auto it = std::ranges::find(vec, spectator); it != end()) {
				continue;
			}
			vec.emplace_back(spectator);
		}
	}

	void erase(const CreaturePtr& spectator) {
		const auto it = std::ranges::find(vec, spectator);
		if (it == end()) {
			return;
		}
		std::iter_swap(it, end() - 1);
		vec.pop_back();
	}

	CreaturePtr operator[] (const uint8_t index) {
		return vec[index];
	}

	size_t size() const { return vec.size(); }
	bool empty() const { return vec.empty(); }
	Iterator begin() { return vec.begin(); }
	ConstIterator begin() const { return vec.begin(); }
	Iterator end() { return vec.end(); }
	ConstIterator end() const { return vec.end(); }
	void emplace_back(CreaturePtr c) { vec.emplace_back(c); }

private:
	Vec vec;
};

#endif
