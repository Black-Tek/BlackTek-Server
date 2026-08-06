// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "player.h"
#include "talkaction.h"

TalkActions::TalkActions()
	: scriptInterface("TalkAction Interface")
{
	scriptInterface.initState();
}

TalkActions::~TalkActions()
{
	clear(false);
}

void TalkActions::clear(bool fromLua)
{
	for (auto it = talkActions.begin(); it != talkActions.end(); ) {
		if (fromLua == it->second.fromLua) {
			it = talkActions.erase(it);
		} else {
			++it;
		}
	}

	reInitState(fromLua);
}

bool TalkActions::reload()
{
    loaded = false;
    clear(false);
    loaded = true;
    return true;
}

LuaScriptInterface& TalkActions::getScriptInterface()
{
	return scriptInterface;
}

bool TalkActions::registerLuaEvent(TalkAction* event)
{
	TalkAction_ptr talkAction{ event };
	std::vector<std::string> words = talkAction->getWordsMap();

	for (size_t i = 0; i < words.size(); i++) {
		if (i == words.size() - 1) {
			talkActions.emplace(words[i], std::move(*talkAction));
		} else {
			talkActions.emplace(words[i], *talkAction);
		}
	}

	return true;
}

TalkActionResult_t TalkActions::playerSaySpell(const PlayerPtr& player, SpeakClasses type, const std::string& words) const
{
	size_t wordsLength = words.length();
	for (auto it = talkActions.begin(); it != talkActions.end(); ) {
		const std::string& talkactionWords = it->first;
		if (!caseInsensitiveStartsWith(words, talkactionWords)) {
			++it;
			continue;
		}

		std::string param;
		if (wordsLength != talkactionWords.size()) {
			param = words.substr(talkactionWords.size());
			if (param.front() != ' ') {
				++it;
				continue;
			}
			trim_left(param, ' ');

			std::string separator = it->second.getSeparator();
			if (separator != " ") {
				if (!param.empty()) {
					if (param != separator) {
						++it;
						continue;
					} else {
						param.erase(param.begin());
					}
				}
			}
		}

		if (it->second.fromLua) {
			if (it->second.getNeedAccess() && !player->getGroup()->access) {
				return TALKACTION_CONTINUE;
			}

			if (player->getAccountType() < it->second.getRequiredAccountType()) {
				return TALKACTION_CONTINUE;
			}
		}

		if (it->second.executeSay(player, talkactionWords, param, type)) {
			return TALKACTION_CONTINUE;
		} else {
			return TALKACTION_BREAK;
		}
	}
	return TALKACTION_CONTINUE;
}

bool TalkAction::executeSay(const PlayerPtr& player, const std::string& words, const std::string& param, SpeakClasses type) const
{
	//onSay(player, words, param, type)
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - TalkAction::executeSay] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);

	LuaScriptInterface::pushSharedPtr(L, player);
	LuaScriptInterface::setMetatable(L, -1, "Player");

	LuaScriptInterface::pushString(L, words);
	LuaScriptInterface::pushString(L, param);
	lua_pushinteger(L, type);

	return scriptInterface->callFunction(4);
}
