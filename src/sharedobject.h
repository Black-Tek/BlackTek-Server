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

#ifndef BT_SHARED_OBJECT_H
#define BT_SHARED_OBJECT_H

#include <memory>
#include "declarations.h"

class SharedObject;

template <typename T>
concept GameObject = std::is_base_of_v<SharedObject, T> and std::is_class_v<T>;

class SharedObject : public std::enable_shared_from_this<SharedObject>
{
public:
    virtual ~SharedObject() = default;

    SharedObject& operator=(const SharedObject&) = delete;
	
	template <GameObject T, class... Args>
	static std::shared_ptr<T> Instantiate_Shared(Args... args) {
    	//std::cout << "## Debug Info : " << typeid(T).name() << " created with Instantiate_Shared()." << "\n";
    	return std::make_shared<T>(std::forward<Args>(args)...);
    }

	template <class T>
	std::shared_ptr<T> static_shared_this() {
    	//std::cout << "## Debug Info : " << typeid(T).name() << " created with static_shared_this()." << "\n";
    	//std::cout << " # " << typeid(T).name() << " has " << (shared_from_this().use_count() - 1) << " instances." << "\n";
		return std::static_pointer_cast<T>(shared_from_this());
	}

	template <class T>
	std::shared_ptr<const T> static_shared_this() const {
    	//std::cout << "## Debug Info : " << typeid(T).name() << " created with static_shared_this()." << "\n";
    	//std::cout << " # " << typeid(T).name() << " has " << (shared_from_this().use_count() - 1) << " instances." << "\n";
		return std::static_pointer_cast<const T>(shared_from_this());
	}

	template <class T>
	std::shared_ptr<T> dynamic_shared_this() {
    	//std::cout << "## Debug Info : " << typeid(T).name() << " created with dynamic_shared_this()." << "\n";
    	//std::cout << " # " << typeid(T).name() << " has " << (shared_from_this().use_count() - 1) << " instances." << "\n";
		return std::dynamic_pointer_cast<T>(shared_from_this());
	}

	template <class T>
	std::shared_ptr<const T> dynamic_shared_this() const {
    	//std::cout << "## Debug Info : " << typeid(T).name() << " created with dynamic_shared_this()." << "\n";
    	//std::cout << " # " << typeid(T).name() << " has " << (shared_from_this().use_count() - 1) << " instances." << "\n";
		return std::dynamic_pointer_cast<const T>(shared_from_this());
	}
};

#endif
