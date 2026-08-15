// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_BASEEVENTS_H
#define FS_BASEEVENTS_H

#include "luascript.h"

class Event;
using Event_ptr = std::unique_ptr<Event>;

class Event
{
	public:
		explicit Event(LuaScriptInterface* interface);
		virtual ~Event() = default;

		bool loadCallback();

		bool isScripted() const {
			return scripted;
		}

		bool scripted = false;
		bool fromLua = false;

		int32_t getScriptId() const
		{
			return scriptId;
		}

	protected:
		virtual std::string_view getScriptEventName() const = 0;

		int32_t scriptId = 0;
		LuaScriptInterface* scriptInterface = nullptr;
};

class BaseEvents
{
	public:
 		constexpr BaseEvents() = default;
		virtual ~BaseEvents() = default;

		virtual bool reload() = 0;
		bool isLoaded() const {
			return loaded;
		}
		void reInitState(bool fromLua);

	protected:
		bool loaded = false;

	private:
		virtual LuaScriptInterface& getScriptInterface() = 0;
		virtual void clear(bool) = 0;

};

class CallBack
{
	public:
		CallBack() = default;

		bool loadCallBack(LuaScriptInterface* interface, const std::string& name);

	protected:
		int32_t scriptId = 0;
		LuaScriptInterface* scriptInterface = nullptr;

	private:
		bool loaded = false;
};

#endif
