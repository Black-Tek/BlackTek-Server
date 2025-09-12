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


#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <utility>
#include <concepts>
#include <vector>
#include <memory>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <cmath>


template<class T>
concept PositiveNumber =
    std::is_integral_v<T> and
    std::is_unsigned_v<T> and
    // Ensures at least 16-bit width
    // prevents usage of bool and char
    sizeof(T) >= 2;

// Forward declaration
template<PositiveNumber NumberType>
class PointStat;

// Base modifier class
// We constrain everything to positive numbers 
// to build in safety rather than throw exceptions
template<PositiveNumber NumberType>
class Modifier 
{
public:
    enum class Type 
    {
        Multiply,
        Divide,
        Add,
        Subtract
    };

    // Lets build in some 0 value checks into the class,
    // to save from doing this work when applying the modifier.
    Modifier(   Type type, 
                NumberType value, 
                bool p_scale = true)
                : type_(type)
                , value_(value)
    {
        if (value == 0) 
        {
            // log::invalid_argument("0 being passed to Modifier constructor");
            switch(type_) 
            {

                case Type::Multiply: 
                {
                    // We can't allow 0 as a multiplier
                    value_ = 1;
                    break;
                }

                case Type::Divide:
                {
                    // We can't allow 0 as a divider
                    value_ = 1;
                    break;
                }

                case Type::Add:
                {
                    // zero is actually ok here, and we don't want to ruin a person's
                    // design on some sort of system by changing it so we will allow
                    // I would recommend logging here as well.
                    break;
                }

                case Type::Subtract:
                {
                    // another place you probably want to log. 
                    break;
                }

                default:
                {
                    // log::error("Type safety broken for Modifier");
                    break;
                }
            }
        }
    }

    ~Modifier() = default;

    Type getType() const { return type_; }
    NumberType getValue() const { return value_; }


private:
    Type type_;
    NumberType value_;
};

template<PositiveNumber NumberType>
class PointStat 
{
    using value_type = Modifier<NumberType>;

public:

    PointStat(NumberType initial, NumberType max, bool use_proportional_s = true)
        : current_(initial)
        , max_(max)
        , base_max_(max)
        , total_multiplier_(0)      // Start at 0 - no multipliers applied
        , total_divider_(0)         // Start at 0 - no dividers applied
        , total_additive_(0)
        , total_subtractive_(0)
        , proportional_scaling_(use_proportional_s)
    {
        // Again we are choosing to build in type safety to avoid paying costs
        // on checking if our values are safe to use everytime we want to use them.
        if (max <= 0) 
        {
            throw std::invalid_argument("PointStat max must be positive");
        }
        // Here you might rather just throw or set the max to initial
        // I can probably add "policies" for such things.. later..
        if (initial > max) {
           // log::invalid_argument("PointStat initial value set higher than max value during construction");
            current_ = max;
        }
    }

    void addModifier(value_type modifier) 
    {
        NumberType old_max = max_;
        
        // The way we accumulate modifiers here is not intuitive or standard
        // I do it this way because I feel this makes more sense logically, to me.
        // I can at any point add a flat value to the base, and it will affect the multipliers
        // I can remove a flat value at any point and the same.
        // The values can be safelly added and removed this way
        // Example : standard (2x * 3x = 6x) mine : (2x + 3x = 5x)
        // If I want to remove the "x2" after having added an "x3"
        // I will be left with an "x3" instead of an "x4"
        // Applying the modifiers in order: Add, Subtract, Multiply, Divide
        // ensures we don't lose precision with integer math
        // There are other ways to address this, but this was my chosen path
        switch(modifier.getType()) 
        {
            case value_type::Type::Multiply:
                total_multiplier_ += modifier.getValue();
                break;
            case value_type::Type::Divide:
                total_divider_ += modifier.getValue();
                break;
            case value_type::Type::Add:
                total_additive_ += modifier.getValue();
                break;
            case value_type::Type::Subtract:
                total_subtractive_ += modifier.getValue();
                break;
        }
        
        recalculateValues();
        
        if (getProportionalScaling() and old_max > 0) 
        {
            double ratio = static_cast<double>(current_) / old_max;
            current_ = static_cast<NumberType>(ratio * max_);
            
            if (current_ == 0 && ratio > 0) 
            {
                current_ = 1;
            }
            
            if (current_ > max_) 
            {
                current_ = max_;
            }
        }
        
        modifiers_.push_back(modifier);
    }

    bool removeModifier(const value_type modifier) 
    {
        auto it = std::find_if(modifiers_.begin(), modifiers_.end(), [modifier] (const auto& elem) { return elem == modifier; });
        
        if (it != modifiers_.end()) 
        {
            NumberType old_max = max_;
            
            switch((*it).getType()) 
            {
                case value_type::Type::Multiply:
                    if (total_multiplier_ >= (*it).getValue()) 
                    {
                        total_multiplier_ -= (*it).getValue();
                    } 
                    else 
                    {
                        total_multiplier_ = 0; // Reset to "no multipliers"
                    }
                    break;
                case value_type::Type::Divide:
                    if (total_divider_ >= (*it).getValue()) 
                    {
                        total_divider_ -= (*it).getValue();
                    } 
                    else 
                    {
                        total_divider_ = 0; // Reset to "no dividers"
                    }
                    break;
                case value_type::Type::Add:
                    if (total_additive_ >= (*it).getValue()) 
                    {
                        total_additive_ -= (*it).getValue();
                    } 
                    else 
                    {
                        total_additive_ = 0; // Reset to base if underflow would occur
                    }
                    break;
                case value_type::Type::Subtract:
                    if (total_subtractive_ >= (*it).getValue()) 
                    {
                        total_subtractive_ -= (*it).getValue();
                    } 
                    else 
                    {
                        total_subtractive_ = 0; // Reset to base if underflow would occur
                    }
                    break;
            }
            
            modifiers_.erase(it);
            recalculateValues();
            
            if (getProportionalScaling() and old_max > 0) 
            {
                double ratio = static_cast<double>(current_) / old_max;
                current_ = static_cast<NumberType>(ratio * max_);
                
                // Todo : allow user to decide if becomes 0 or 1
                if (current_ == 0 and ratio > 0) 
                {
                    current_ = 1;
                }
                
                if (current_ > max_) 
                {
                    current_ = max_;
                }
            }
            
            return true;
        }
        return false;
    }

    NumberType current() 
    {
        return current_;
    }

    NumberType value() 
    {
        return current_;
    }

    NumberType max() 
    {
        return max_;
    }

    NumberType baseMax() 
    {
        return base_max_;
    }

    NumberType getTotalMultiplier() const { return total_multiplier_; }
    NumberType getTotalDivider() const { return total_divider_; }
    NumberType getTotalAdditive() const { return total_additive_; }
    NumberType getTotalSubtractive() const { return total_subtractive_; }

    bool clearModifiers() 
    {
        if (modifiers_.empty()) 
        {
            return false;
        }
        
        double ratio = static_cast<double>(current_) / max_;
        
        modifiers_.clear();
        total_multiplier_ = 0;  // Reset to "no multipliers"
        total_divider_ = 0;     // Reset to "no dividers"
        total_additive_ = 0;
        total_subtractive_ = 0;
        
        recalculateValues();

        if (getProportionalScaling())
        {
            current_ = static_cast<NumberType>(ratio * max_);
            
            // Ensure current doesn't exceed max
            if (current_ > max_) 
            {
                current_ = max_;
            }
        }
        
        return true;
    }

    // This will always add points but returns false when your number is higher than a limit
    bool add(NumberType points) 
    {
        [[unlikely]]
        if (points > std::numeric_limits<NumberType>::max() - current_) 
        {
            current_ = max_;
            return false; // hit hard max
        }
        
        NumberType new_value = current_ + points;
        if (new_value > max_) 
        {
            current_ = max_;
            return false;  // hit builtin max
        } 
        else [[likely]]
        {
            current_ = new_value;
            return true;
        }
        // should be unreachable
        // should mark as such and log
        return false;
    }

    // This will always remove points but returns false when you hit can't remove all the points passed
    bool remove(NumberType points) 
    {
        if (points > current_) 
        {
            current_ = 0;
            return false;
        } 
        else [[likely]]
        {
            current_ -= points;
            return true;
        }
        // should be unreachable
        // should mark as such and log
        return false;
    }

    // Convenience method for chaining modifiers
    PointStat& modify(value_type modifier) 
    {
        addModifier(modifier);
        return *this;
    }

    bool getProportionalScaling() const { return proportional_scaling_; }

private:

    // Recalculate max and current values based on accumulated modifiers
    void recalculateValues() 
    {
        // Start with base max
        NumberType working_max = base_max_;
        
        // Apply modifiers in order: Add, Subtract, Multiply, Divide
        // This order ensures we don't lose precision with integer math
        
        // Add
        if (total_additive_ > 0) 
        {
            if (working_max <= std::numeric_limits<NumberType>::max() - total_additive_) 
            {
                working_max += total_additive_;
            } 
            else 
            {
                // Overflow protection - cap at max value
                working_max = std::numeric_limits<NumberType>::max();
            }
        }
        
        // Subtract
        if (total_subtractive_ > 0) 
        {
            if (working_max > total_subtractive_) 
            {
                working_max -= total_subtractive_;
            }
             else 
            {
                working_max = 1; // Minimum value to maintain type safety
            }
        }
        
        // Multiply
        if (total_multiplier_ > 0) 
        {
            if (working_max <= std::numeric_limits<NumberType>::max() / total_multiplier_) 
            {
                working_max *= total_multiplier_;
            } 
            else 
            {
                working_max = std::numeric_limits<NumberType>::max();
            }
        }
        // Divide
        if (total_divider_ > 0) 
        {
            NumberType temp_result = working_max / total_divider_;
            if (temp_result > 0) 
            {
                working_max = temp_result;
            } 
            else 
            {
                working_max = 1;
            }
        }
        
        max_ = working_max;
        
        // Ensure current doesn't exceed new max
        if (current_ > max_) 
        {
            current_ = max_;
        }
    }

    std::vector<value_type> modifiers_;
    NumberType current_;
    NumberType base_max_;
    NumberType max_;
    
    // Accumulated modifier totals
    NumberType total_multiplier_;
    NumberType total_divider_;
    NumberType total_additive_;
    NumberType total_subtractive_;
    bool proportional_scaling_;
};

/*int main()
{
    PointStat<uint16_t> stat(100, 100);  // current=100, max=100

    // Add multiply by 2
    stat.addModifier(Modifier<uint16_t>(Modifier<uint16_t>::Type::Multiply, 2));
    // total_multiplier_ = 0 + 2 = 2

    // Add multiply by 3  
    stat.addModifier(Modifier<uint16_t>(Modifier<uint16_t>::Type::Multiply, 3));
    // total_multiplier_ = 2 + 3 = 5

    // Add 50 points
    stat.addModifier(Modifier<uint16_t>(Modifier<uint16_t>::Type::Add, 50));
    // max = (100 + 50) * 5 = 750

    std::cout << "Max value is : " << stat.max() << " \n";
    std::cout << "Total multiplier: " << stat.getTotalMultiplier() << "\n";
    std::cout << "Total additive: " << stat.getTotalAdditive() << "\n";
    
    return 0;
}*/