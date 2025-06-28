// MIT License

// Author : Codinablack@github.com

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#include <concepts>
#include <limits>
#include <cmath>
#include <functional>
#include <string>
#include <vector>
#include <filesystem>
#include <gtl/phmap.hpp>
#include "const.h"

namespace Components {
    namespace Skills {

        static constexpr auto path = "data/skills/";
        FormulaType const ParseFormula(std::string_view modName) noexcept;

        // Maximum possible points, used for overflow prevention
        static constexpr uint64_t PointMax = UINT64_MAX;
        static constexpr uint16_t LevelMax = UINT16_MAX;

        class CustomSkill {
        public:

            CustomSkill(uint8_t form = FormulaType::EXPONENTIAL, uint16_t max = 0, float x = 1.0, float y = 50, float z = 10) :
                _multiplier(x),
                _difficulty(y),
                _threshold(z),
                max_level(max),
                _formula(static_cast<FormulaType>(form))
            {
                //
            }

            static std::shared_ptr<CustomSkill> make_skill(const CustomSkill& skill) 
            {
                return std::make_shared<CustomSkill>(skill);
            }

            static std::shared_ptr<CustomSkill> make_skill(const std::shared_ptr<CustomSkill>& skill)
            {
                return std::make_shared<CustomSkill>(*skill.get());
            }

            static std::shared_ptr<CustomSkill> make_skill(uint8_t form = FormulaType::EXPONENTIAL, uint16_t max = 0, float x = 1.0, float y = 50, float z = 10)
            {
                return std::make_shared<CustomSkill>(form, max, x, y, z);
            }

            [[nodiscard]]
            const uint64_t points() const noexcept 
            {
                [[unlikely]]
                if (max_level > 0 and current_level >= max_level) 
                {
                    auto current = pointsRequired(max_level);
                    return current;
                }
                [[likely]]
                return current_points;
            }

            [[nodiscard]]
            const uint64_t needed() const noexcept
            {
                [[likely]]
                if (LevelMax - 1 >= current_level + 1)
                {
                    return pointsRequired(current_level + 1);
                }
                [[unlikely]]
                return pointsRequired(LevelMax);
            }

            [[nodiscard]]
            const float multiplier() const noexcept
            {
                return _multiplier;
            }

            [[nodiscard]]
            const float difficulty() const noexcept
            {
                return _difficulty;
            }

            [[nodiscard]]
            const float threshold() const noexcept
            {
                return _threshold;
            }

            // It's worth noting that even tho we have a bonus level
            // along with a current level, we are still returning a max of a uint16_t,
            // so maxing both out will not result in an overflow, it's still hard capped
            [[nodiscard]]
            constexpr uint16_t level(bool count_bonus = true) const noexcept
            {
                if (not count_bonus or bonus_level == 0)
                {
                    return current_level;
                }

                // Cast to signed type in case bonus is negative value
                int32_t total = static_cast<int32_t>(current_level) + static_cast<int32_t>(bonus_level);

                // If result is negative or zero, return level 1 (minimum level)
                if (total <= 0)
                {
                    return 1;
                }

                return total >= LevelMax ? LevelMax : static_cast<uint16_t>(total);
            }

            [[nodiscard]]
            const int16_t bonus() const noexcept
            {
                return bonus_level;
            }

            [[nodiscard]]
            const uint16_t max() const noexcept
            {
                return max_level;
            }

            [[nodiscard]]
            const FormulaType formula() const noexcept
            {
                return _formula;
            }

            bool addPoints(uint32_t points) noexcept
            {
                [[unlikely]]
                if (not points) 
                {
                    return false;
                }

                auto temp_level = static_cast<uint64_t>(current_level);
                auto temp_current_points = current_points;

                while (true) 
                {
                    if (max_level and temp_level >= max_level) 
                    {
                        temp_current_points = 0;
                        break;
                    }

                    uint64_t points_required = pointsRequired(temp_level + 1);
                    if (points_required == std::numeric_limits<uint64_t>::max() or points_required == 0)
                    {
                        temp_current_points += points;
                        break;
                    }

                    uint64_t excess_points = points_required - temp_current_points;
                    if (points >= excess_points)
                    {
                        points -= excess_points;
                        temp_level++;
                        temp_current_points = 0;
                    }
                    else 
                    {
                        temp_current_points += points;
                        points = 0;
                        break;
                    }
                }

                current_points = temp_current_points;
                current_level = static_cast<uint16_t>(temp_level);
                return true;
            }

            bool removePoints(uint32_t points_to_remove) noexcept
            {
                [[unlikely]]
                if (not points_to_remove) 
                {
                    return false;
                }

                auto temp_level = static_cast<uint64_t>(current_level);
                auto temp_points = current_points;

                // We remove first to avoid the loop if we can.
                if (points_to_remove >= temp_points)
                {
                    points_to_remove -= temp_points;
                    temp_points = 0;
                }
                else 
                {
                    temp_points -= points_to_remove;
                    points_to_remove = 0;
                }

                while (points_to_remove > 0 and temp_level > 0) 
                {
                    auto required_points = pointsRequired(temp_level);
                    if (points_to_remove >= required_points)
                    {
                        points_to_remove -= required_points;
                        temp_level--; 
                        temp_points = 0;
                    }
                    else 
                    {
                        temp_points = required_points - points_to_remove;
                        points_to_remove = 0;
                        break;
                    }
                }

                current_level = temp_level > 0 ? static_cast<uint16_t>(temp_level) : 1;
                current_points = temp_level > 0 ? temp_points : 0;
                return true;
            }

            bool addLevels(uint16_t levels, bool save_progress = false) noexcept
            {
                [[unlikely]] 
                if (not levels) 
                {
                    return false;
                }

                uint64_t required = pointsRequired(current_level);
                double percent = save_progress and required > 0 ? static_cast<double>(current_points) / required : 0.0;
                current_level = max_level and (levels + current_level) >= max_level ? max_level : levels + current_level;
                current_points = save_progress ? static_cast<uint64_t>(pointsRequired(current_level) * percent) : 0;
                return true;
            }


            bool removeLevels(uint16_t levels, bool save_progress = false) noexcept 
            {
                [[unlikely]]
                if (not levels) 
                {
                    return false;
                }
                uint64_t required = pointsRequired(current_level);
                double percent = save_progress and required > 0 ? static_cast<double>(current_points) / required : 0.0;
                current_level = std::max<uint16_t>(1, current_level - levels);
                current_points = save_progress ? static_cast<uint64_t>(pointsRequired(current_level) * percent) : 0;
                return true;
            }

            template<typename Number = uint8_t>
            Number percent() const noexcept 
            {
                static_assert(std::is_arithmetic_v<Number>, "percent() requires an arithmetic return type");
                auto level = (current_level + 1);
                auto required = pointsRequired(level);

                [[likely]] 
                if (current_points and required)
                {
                    auto raw_percent = (current_points * 100ULL) / required;
                    return static_cast<Number>(raw_percent);
                }
                [[unlikely]]
                return static_cast<Number>(0);
            }

            void setBonus(int16_t level) noexcept
            {
                bonus_level = level;
            }

            bool addBonusLevels(int16_t level) noexcept
            {
                [[unlikely]]
                if (not level)
                {
                    return false;
                }

                int32_t new_bonus = static_cast<int32_t>(bonus_level) + level;

                if (new_bonus > std::numeric_limits<int16_t>::max())
                {
                    bonus_level = std::numeric_limits<int16_t>::max();
                }
                else if (new_bonus < std::numeric_limits<int16_t>::min())
                {
                    bonus_level = std::numeric_limits<int16_t>::min();
                }
                else
                {
                    bonus_level = static_cast<int16_t>(new_bonus);
                }

                return true;
            }

            bool removeBonusLevels(int16_t level) noexcept
            {
                [[unlikely]]
                if (not level)
                {
                    return false;
                }

                int32_t new_bonus = static_cast<int32_t>(bonus_level) - level;

                if (new_bonus > std::numeric_limits<int16_t>::max())
                {
                    bonus_level = std::numeric_limits<int16_t>::max();
                }
                else if (new_bonus < std::numeric_limits<int16_t>::min())
                {
                    bonus_level = std::numeric_limits<int16_t>::min();
                }
                else
                {
                    bonus_level = static_cast<int16_t>(new_bonus);
                }

                return true;
            }

            void clearLevels(bool include_bonus)
            {
                current_points = 0;
                current_level = 1;
                if (include_bonus)
                {
                    bonus_level = 0;
                }
            }

            void clearPoints()
            {
                current_points = 0;
            }

        private:

            uint64_t current_points = 0;
            float _multiplier = 1;
            float _difficulty = 1;
            float _threshold = 1;
            uint16_t current_level = 1;
            int16_t bonus_level = 0;
            uint16_t max_level = 0;  // Maximum allowed level, if 0, limit is numerical limit;
            FormulaType _formula = FormulaType::EXPONENTIAL;

            [[nodiscard]]
            constexpr uint64_t safeRound(double value) const
            {
                if (value >= static_cast<double>(PointMax)) {
                    return PointMax;
                }
                if (value <= 0.0) {
                    return 0;
                }
                return static_cast<uint64_t>(std::round(value));
            }


            [[nodiscard]] 
            uint64_t pointsRequired(uint64_t target_level) const
            {
                [[unlikely]]
                if (not target_level)
                {
                    return 0;
                }

                switch (_formula) 
                {
                    case FormulaType::LINEAR: return linearGrowth(target_level);
                    case FormulaType::LOGARITHMIC: return logarithmicGrowth(target_level);
                    case FormulaType::EXPONENTIAL: return exponentialGrowth(target_level);
                    case FormulaType::QUADRATIC: return quadraticGrowth(target_level);
                    case FormulaType::CUBIC: return cubicGrowth(target_level);
                    case FormulaType::STEP: return stepGrowth(target_level);
                    case FormulaType::ROOT: return rootGrowth(target_level);
                    case FormulaType::INVERSE: return inverseGrowth(target_level);
                }
                return 0;
            }

            static constexpr uint64_t integerSqrt(uint64_t n)
            {
                uint64_t left = 0, right = n, ans = 0;
                while (left <= right) 
                {
                    uint64_t mid = left + (right - left) / 2;
                    if (mid <= n / mid) 
                    {
                        ans = mid;
                        left = mid + 1;
                    }
                    else 
                    {
                        right = mid - 1;
                    }
                }
                return ans;
            }

            static constexpr uint64_t integerPow(uint64_t base, uint64_t exp)
            {
                uint64_t result = 1;
                while (exp) 
                {
                    if (exp & 1) 
                    {
                        [[unlikely]]
                        if (result > PointMax / base)
                        {
                            return PointMax;
                        }     
                        result *= base;
                    }
                    exp >>= 1;

                    [[unlikely]]
                    if (exp and base > PointMax / base)
                    {
                        return PointMax;
                    }
                    base *= base;
                }
                return result;
            }

            [[nodiscard ("This is an internal method that should only be called when the return value is intended for use.")]]
            const uint64_t linearGrowth(uint64_t target_level) const
            {
                // this is redundant extra protection, in case someone uses method elsewhere
                [[unlikely]]
                if (not target_level)
                {
                    return 0;
                }

                double result = static_cast<double>(_multiplier) * static_cast<double>(_difficulty)
                    + static_cast<double>(_threshold) * static_cast<double>(target_level);

                return safeRound(result);
            }

            [[nodiscard("This is an internal method that should only be called when the return value is intended for use.")]]
            const uint64_t logarithmicGrowth(uint64_t target_level) const
            {
                // this is redundant extra protection, in case someone uses method elsewhere
                [[unlikely]]
                if (not target_level)
                {
                    return 0;
                }

                double logLevel = std::log(static_cast<double>(target_level));
                double result = static_cast<double>(_multiplier) * static_cast<double>(_difficulty)
                    + static_cast<double>(_threshold) * logLevel;

                return safeRound(result);
            }

            [[nodiscard("This is an internal method that should only be called when the return value is intended for use.")]]
            const uint64_t exponentialGrowth(uint64_t target_level) const
            {
                // this is redundant extra protection, in case someone uses method elsewhere
                [[unlikely]]
                if (not target_level)
                {
                    return 0;
                }

                double base = static_cast<double>(_difficulty);
                double result = static_cast<double>(_threshold) * std::pow(base, (target_level - (static_cast<double>(_multiplier) + 1)));

                return safeRound(result);
            }

            [[nodiscard("This is an internal method that should only be called when the return value is intended for use.")]]
            const uint64_t quadraticGrowth(uint64_t target_level) const
            {
                // this is redundant extra protection, in case someone uses method elsewhere
                [[unlikely]]
                if (not target_level)
                {
                    return 0;
                }

                double level = static_cast<double>(target_level);
                double result = static_cast<double>(_multiplier)
                    + static_cast<double>(_difficulty) * level * level
                    + static_cast<double>(_threshold) * level;

                return safeRound(result);
            }

            [[nodiscard("This is an internal method that should only be called when the return value is intended for use.")]]
            const uint64_t cubicGrowth(uint64_t target_level) const
            {
                // this is redundant extra protection, in case someone uses method elsewhere
                [[unlikely]]
                if (not target_level)
                {
                    return 0;
                }

                double level = static_cast<double>(target_level);
                double result = static_cast<double>(_multiplier)
                    + static_cast<double>(_difficulty) * level * level * level
                    + static_cast<double>(_threshold) * level;

                return safeRound(result);
            }

            [[nodiscard("This is an internal method that should only be called when the return value is intended for use.")]]
            const uint64_t stepGrowth(uint64_t target_level) const
            {
                // this is redundant extra protection, in case someone uses method elsewhere
                [[unlikely]]
                if (not target_level)
                {
                    return 0;
                }

                double steps = std::floor(static_cast<double>(target_level) / static_cast<double>(_multiplier));
                double result = steps * static_cast<double>(_difficulty) + static_cast<double>(_threshold);

                return safeRound(result);
            }

            [[nodiscard("This is an internal method that should only be called when the return value is intended for use.")]]
            const uint64_t rootGrowth(uint64_t target_level) const
            {
                // this is redundant extra protection, in case someone uses method elsewhere
                [[unlikely]]
                if (not target_level)
                {
                    return 0;
                }

                double sqrtLevel = std::sqrt(static_cast<double>(target_level));
                double result = static_cast<double>(_multiplier) * sqrtLevel + static_cast<double>(_threshold);

                return safeRound(result);
            }

            [[nodiscard("This is an internal method that should only be called when the return value is intended for use.")]]
            const uint64_t inverseGrowth(uint64_t target_level) const
            {
                // this is redundant extra protection, in case someone uses method elsewhere
                [[unlikely]]
                if (not target_level)
                {
                    return 0;
                }

                double inverse = 1.0 / static_cast<double>(target_level);
                double result = static_cast<double>(_multiplier) * inverse * static_cast<double>(_difficulty)
                    + static_cast<double>(_threshold);

                return safeRound(result);
            }
        };
    }
}

using SkillRegistry = gtl::flat_hash_map<std::string, std::shared_ptr<Components::Skills::CustomSkill>>;