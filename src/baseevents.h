/**
 * BlackTek Server Engine
 * https://github.com/Black-Tek/BlackTek-Server
 *
 * ============================================================================
 * COPYRIGHT & ATTRIBUTION NOTICE
 * ============================================================================
 *
 * Upstream Copyright:
 *   Copyright (C) The Forgotten Server Authors
 *   Governed by the GNU General Public License v2.0 (GPL-2.0)
 *
 * Modifications & BlackTek Core Additions:
 *   Copyright (C) 2024-2026 BlackTek (Author: Codinablack)
 *
 * ============================================================================
 * DUAL LICENSING & TRANSITION NOTICE
 * ============================================================================
 *
 * This file contains original code from The Forgotten Server alongside substantial
 * modifications and new architecture by BlackTek.
 *
 * To facilitate the project's long-term goal of transitioning the codebase to a
 * fully permissive MIT License:
 *
 * 1. All original TFS source lines remain governed by GPL-2.0.
 * 2. All contributions, modifications, and new additions authored by BlackTek
 *    are dual-licensed under both the MIT License and GPL-2.0.
 * 3. Downstream users distributing this combined codebase must comply with
 *    the GPL-2.0 requirements as long as original upstream TFS code remains.
 *
 * Once an individual module or file has been completely refactored or rewritten
 * such that no original TFS code remains, the file will be relicensed solely
 * under the MIT License.
 *
 * See licensing/original codebase/GNU-GPL (GPL-2.0) and licensing/License (MIT)
 * in the project root for the full texts.
 */

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
