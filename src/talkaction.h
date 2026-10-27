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

#ifndef FS_TALKACTION_H
#define FS_TALKACTION_H

#include "luascript.h"
#include "baseevents.h"
#include "const.h"

class TalkAction;
using TalkAction_ptr = std::unique_ptr<TalkAction>;

enum TalkActionResult_t {
	TALKACTION_CONTINUE,
	TALKACTION_BREAK,
	TALKACTION_FAILED,
};

class TalkAction : public Event
{
	public:
		explicit TalkAction(LuaScriptInterface* interface) : Event(interface) {}

		const std::string& getWords() const {
			return words;
		}
		const std::vector<std::string>& getWordsMap() const {
			return wordsMap;
		}
		void setWords(std::string_view word) {
			words = word;
			wordsMap.emplace_back(word);
		}
		std::string getSeparator() const {
			return separator;
		}
		void setSeparator(const std::string& sep) {
			separator = sep;
		}

		//scripting
		bool executeSay(const PlayerPtr& player, const std::string& words, const std::string& param, SpeakClasses type) const;

		AccountType_t getRequiredAccountType() const {
			return requiredAccountType;
		}

		void setRequiredAccountType(AccountType_t reqAccType) {
			requiredAccountType = reqAccType;
		}

		bool getNeedAccess() const {
			return needAccess;
		}

		void setNeedAccess(bool b) {
			needAccess = b;
		}

	private:
		std::string_view getScriptEventName() const override { return "onSay"; }

		std::string words;
		std::vector<std::string> wordsMap;
		std::string separator = "\"";
		bool needAccess = false;
		AccountType_t requiredAccountType = ACCOUNT_TYPE_NORMAL;
};

class TalkActions final : public BaseEvents
{
	public:
		TalkActions();
		~TalkActions() override;

		// non-copyable
		TalkActions(const TalkActions&) = delete;
		TalkActions& operator=(const TalkActions&) = delete;

		TalkActionResult_t playerSaySpell(const PlayerPtr& player, SpeakClasses type, const std::string& words) const;

		bool registerLuaEvent(TalkAction* event);
		void clear(bool fromLua) override final;
		bool reload() override;

	private:
		LuaScriptInterface& getScriptInterface() override;

		std::map<std::string, TalkAction> talkActions;

		LuaScriptInterface scriptInterface;
};

#endif
