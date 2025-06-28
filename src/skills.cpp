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
#include <toml++/toml.hpp>
#include <iostream>
#include <array>
#include "skills.h"
#include <gtl/phmap.hpp>

namespace Components {
    namespace Skills {

        static gtl::node_hash_map<std::string, CustomSkill> skill_registry{};

        static bool load(std::string path)
        {
            for (const auto& file : std::filesystem::recursive_directory_iterator(path)) 
            {
                if (file.is_regular_file() and file.path().extension() == ".toml")
                {
                    try 
                    {
                        auto toml_file = toml::parse_file(file.path().string());

                        for (const auto& [index, entry] : toml_file) 
                        {

                            toml::table skill_data = *entry.as_table();
                            std::string name = skill_data["name"].value_or("unknown");
                            if (name == "unknown")
                            {
                                // log skipped skill for lack of name
                                continue;
                            }
                            
                            auto max_level = skill_data["max"].value_or(0);
                            
                            std::string_view formula_name = skill_data["formula"].value_or("none");
                            FormulaType formula = ParseFormula(formula_name);
                            skill_registry.try_emplace(name, formula, max_level);
                        }
                    }
                    catch (const toml::parse_error& err) 
                    {
                        std::cerr << "Error parsing file " << file.path() << ": " << err << "\n";
                    }
                }
            }
        }
    }
}
