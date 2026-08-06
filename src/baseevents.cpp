// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "baseevents.h"

void BaseEvents::reInitState(bool fromLua)
{
	if (!fromLua) {
		getScriptInterface().reInitState();
	}
}

Event::Event(LuaScriptInterface* interface) : scriptInterface(interface) {}

bool Event::loadCallback()
{
	if (!scriptInterface || scriptId != 0) {
		std::cout << "Failure: [Event::loadCallback] scriptInterface == nullptr. scriptid = " << scriptId << std::endl;
		return false;
	}

	int32_t id = scriptInterface->getEvent();
	if (id == -1) {
		std::cout << "[Warning - Event::loadCallback] Event " << getScriptEventName() << " not found. " << std::endl;
		return false;
	}

	scripted = true;
	scriptId = id;
	return true;
}

bool CallBack::loadCallBack(LuaScriptInterface* interface, const std::string& name)
{
	if (!interface) {
		std::cout << "Failure: [CallBack::loadCallBack] scriptInterface == nullptr" << std::endl;
		return false;
	}

	scriptInterface = interface;

	int32_t id = scriptInterface->getEvent(name);
	if (id == -1) {
		std::cout << "[Warning - CallBack::loadCallBack] Event " << name << " not found." << std::endl;
		return false;
	}

	scriptId = id;
	loaded = true;
	return true;
}
