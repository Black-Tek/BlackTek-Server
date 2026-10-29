-- Keep IDs unique across script reloads, so replies to old windows cannot
-- accidentally confirm a newly created operation.
ModalWindowNextId = ModalWindowNextId or 10000

if not __ModalWindow then
	__ModalWindow = ModalWindow
end
-- Old callbacks belong to the previous script generation.
for playerId, windows in pairs(ModalWindows or {}) do
	local player = Player(playerId)
	if player and player.forgetModalWindow then
		for id in pairs(windows) do player:forgetModalWindow(id) end
	end
end
ModalWindows = {}

ModalWindow = {}
ModalWindow.__index = ModalWindow

function ModalWindow.new(self, ...)
	if type(self) ~= "table" then
		return __ModalWindow(self, ...)
	end

	self.modalWindowId = 0
	self.buttons = {}
	self.choices = {}
	self.using = 0
	return setmetatable(self, ModalWindow)
end

setmetatable(ModalWindow, {
	__call = function (self, ...)
		return ModalWindow.new(...)
	end
})

function ModalWindow:setTitle(title)
	self.title = tostring(title)
	return true
end

function ModalWindow:setMessage(message)
	self.message = tostring(message)
	return true
end

function ModalWindow:addButton(name, callback)
	if type(name) ~= "string" then
		io.write("ModalWindow:addButton: name must be a string.")
		name = tostring(name)
	end

	if self.buttons[name] then
		io.write("ModalWindow: Button with name '" .. name .. "' already exists.")
		return false
	end

	local id = #self.buttons + 1
	local button = { id = id, name = name, callback = callback }
	self.buttons[id] = button
	self.buttons[name] = button
	return true
end

function ModalWindow:removeButton(name)
	if type(name) ~= "string" then
		io.write("ModalWindow:removeButton: name must be a string.")
		name = tostring(name)
	end

	local button = self.buttons[name]
	if not button then
		io.write("ModalWindow: Button with name '" .. name .. "' does not exist.")
		return false
	end

	self.buttons[button.id] = nil
	self.buttons[name] = nil
	return true
end

function ModalWindow:callButton(name, player, button, choice)
	if type(name) ~= "string" then
		io.write("ModalWindow:callButton: name must be a string.")
		name = tostring(name)
	end

	local button = self.buttons[name]
	if not button then
		io.write("ModalWindow: Button with name '" .. name .. "' does not exist.")
		return false
	end

	if not button.callback then
		io.write("ModalWindow: Button with name '" .. name .. "' has no callback.")
		return false
	end
	return button.callback(player, button, choice)
end

function ModalWindow:clearButtons()
	self.buttons = {}
	return true
end

function ModalWindow:setDefaultEnterButton(buttonId)
	self.defaultEnterButton = buttonId
	return true
end

function ModalWindow:setDefaultEscapeButton(buttonId)
	self.defaultEscapeButton = buttonId
	return true
end

function ModalWindow:setDefaultCallback(callback)
	self.defaultCallback = callback
	return true
end

function ModalWindow:addChoice(text, callback)
	if type(text) ~= "string" then
		io.write("ModalWindow:addChoice: text must be a string.")
		text = tostring(text)
	end

	local id = #self.choices + 1
	local choice = { id = id, text = text, callback = callback }
	self.choices[id] = choice
	self.choices[text] = choice
	return true
end

function ModalWindow:removeChoice(text)
	if type(text) ~= "string" then
		io.write("ModalWindow:removeChoice: text must be a string.")
		text = tostring(text)
	end

	local choice = self.choices[text]
	if not choice then
		io.write("ModalWindow: Choice with text '" .. text .. "' does not exist.")
		return false
	end

	self.choices[choice.id] = nil
	self.choices[text] = nil
	return true
end

function ModalWindow:callChoice(text, player, button, choice)
	if type(text) ~= "string" then
		io.write("ModalWindow:callChoice: text must be a string.")
		text = tostring(text)
	end

	local choice = self.choices[text]
	if not choice then
		io.write("ModalWindow: Choice with text '" .. text .. "' does not exist.")
		return false
	end

	if not choice.callback then
		io.write("ModalWindow: Choice with text '" .. text .. "' has no callback.")
		return false
	end
	return choice.callback(player, button, choice)
end

function ModalWindow:clearChoices()
	self.choices = {}
	return true
end

function ModalWindow:clear()
	self.choices = {}
	self.buttons = {}
	return true
end

function ModalWindow:setPriority(priority)
	self.priority = priority
	return true
end

function ModalWindow:setId()
	if self.modalWindowId ~= 0 then
		return self.modalWindowId
	end

	self.modalWindowId = ModalWindowNextId
	ModalWindowNextId = ModalWindowNextId + 1
	return self.modalWindowId
end

function ModalWindow:create()
	local modalWindow = __ModalWindow(self:setId(), self.title, self.message)
	modalWindow:setPriority(self.priority and true or false)

	for id = 1, #self.buttons do
		local name = self.buttons[id].name
		modalWindow:addButton(id, name)
		if id == self.defaultEnterButton or name == self.defaultEnterButton then
			modalWindow:setDefaultEnterButton(id)
		end
		if id == self.defaultEscapeButton or name == self.defaultEscapeButton then
			modalWindow:setDefaultEscapeButton(id)
		end
	end

	for id = 1, #self.choices do
		modalWindow:addChoice(id, self.choices[id].text)
	end
	return modalWindow
end

function ModalWindow:sendToPlayer(player)
	local modalWindow = self:create()
	local playerId = player:getId()
	if not modalWindow:sendToPlayer(player) then
		return false
	end
	ModalWindows[playerId] = ModalWindows[playerId] or {}
	if not ModalWindows[playerId][self.modalWindowId] then
		self.using = self.using + 1
	end
	ModalWindows[playerId][self.modalWindowId] = self
	self.expiresAt = os.time() + (self.timeout or 900)
	player:registerEvent("modalWindowHelper")
	return true
end

function ModalWindow.clearPlayer(playerId, modalWindowId)
	local windows = ModalWindows[playerId]
	if not windows then
		return
	end
	local player = Player(playerId)
	for id, window in pairs(windows) do
		if not modalWindowId or id == modalWindowId then
			if player and player.forgetModalWindow then player:forgetModalWindow(id) end
			window.using = math.max(0, window.using - 1)
			windows[id] = nil
		end
	end
	if not next(windows) then
		ModalWindows[playerId] = nil
	end
end

local creatureEvent = CreatureEvent("modalWindowHelper")

function creatureEvent.onModalWindow(player, modalWindowId, buttonId, choiceId)
	local playerId = player:getId()
	local modalWindows = ModalWindows[playerId]
	if not modalWindows then
		return true
	end

	local modalWindow = modalWindows[modalWindowId]
	if not modalWindow then
		return true
	end

	-- Remove the entry before user code runs, including when the callback errors
	-- or creates another window. A reply can consume an operation only once.
	ModalWindow.clearPlayer(playerId, modalWindowId)
	if modalWindow.expiresAt <= os.time() then
		return true
	end

	local button = modalWindow.buttons[buttonId]
	if not button then
		return true
	end
	local choice = modalWindow.choices[choiceId] or {}
	local callback = button.callback or choice.callback or modalWindow.defaultCallback
	if callback then
		local ok, err = pcall(callback, player, button, choice)
		if not ok then
			print("[ModalWindow] Callback failed: " .. tostring(err))
		end
	end
	return true
end

creatureEvent:register()

local logout = CreatureEvent("modalWindowLogout")
function logout.onLogout(player)
	ModalWindow.clearPlayer(player:getId())
	return true
end
logout:register()

-- Forced kicks bypass onLogout in this engine. Sweep offline players and
-- expired windows as well, so their callbacks cannot retain native objects.
local cleanup = GlobalEvent("modalWindowCleanup")
function cleanup.onThink(interval)
	local now = os.time()
	for playerId, windows in pairs(ModalWindows) do
		if not Player(playerId) then
			ModalWindow.clearPlayer(playerId)
		else
			for id, window in pairs(windows) do
				if window.expiresAt <= now then
					ModalWindow.clearPlayer(playerId, id)
				end
			end
		end
	end
	return true
end
cleanup:interval(60000)
cleanup:register()
