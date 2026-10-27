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

#include "augment.h"
#include <gtl/phmap.hpp>

namespace BlackTek
{

	class Augments 
	{

	public:
		// No Constructors! Purely static class.
		Augments() = delete;
		~Augments() = delete;
		Augments(const Augments&) = delete;
		Augments& operator=(const Augments&) = delete;
		Augments(Augments&&) = delete;
		Augments& operator=(Augments&&) = delete;

		static constexpr auto path = "data/augments/";
		static const uint8_t ParseStance(std::string_view stanceName) noexcept;
		static const uint8_t ParseFactor(std::string_view factor) noexcept;
		static const uint8_t ParseAttackModifier(std::string_view modName) noexcept;
		static const uint8_t ParseDefenseModifier(std::string_view modName) noexcept;
		static const uint16_t ParseDamage(std::string_view damageName) noexcept;
		static const uint8_t ParseOrigin(std::string_view originName) noexcept;
		static const uint8_t ParseRaceType(std::string_view raceType) noexcept;
		static const uint8_t ParseCreatureType(std::string_view creatureType) noexcept;

		static std::shared_ptr<Augment> MakeAugment(std::string_view augmentName);

		static void loadAll();
		static void clearAll();
		static void reload();
		static size_t count();
		static void AddAugment(std::shared_ptr<Augment> augment);
		static void RemoveAugment(const std::shared_ptr<Augment>& augment);
		static void RemoveAugment(std::string_view augName);
		static void RemoveAugment(const std::string& augName);
		static std::shared_ptr<Augment> GetAugment(std::string_view augName);
	};
}