-- Run from the repository root: lua tests/data/events_modal_regression.lua
-- Tests the modal helper still present upstream. The retired shrine is not loaded.
local passed = 0
local function test(name, fn)
	local ok, err = pcall(fn)
	assert(ok, name .. ": " .. tostring(err))
	passed = passed + 1; print("PASS " .. name)
end
local players, events, globals, nativeWindows = {}, {}, {}, {}
local now = 1000
os.time = function() return now end
function Player(id) return players[id] end
function CreatureEvent(name)
	local e = {}; function e:register() events[name] = self end; return e
end
function GlobalEvent(name)
	local e = {}; function e:register() globals[name] = self end; function e:interval() end; return e
end
function ModalWindow(id, title, message)
	local w = {id = id, buttons = {}}
	function w:setPriority() end
	function w:addButton(id, name) self.buttons[id] = name end
	function w:addChoice() end
	function w:setDefaultEnterButton(id) self.enter = id end
	function w:setDefaultEscapeButton(id) self.escape = id end
	function w:sendToPlayer(p)
		if p.failSend then return false end
		nativeWindows[p.id] = self; p.pending[self.id] = true; return true
	end
	return w
end
local nextPlayer = 0
local function player()
	nextPlayer = nextPlayer + 1
	local p = {id = nextPlayer, pending = {}}
	function p:getId() return self.id end
	function p:registerEvent() end
	function p:forgetModalWindow(id) self.pending[id] = nil end
	players[p.id] = p; return p
end
dofile("data/scripts/creaturescripts/modal_window_helper.lua")
local function reply(p, button, id)
	id = id or nativeWindows[p.id].id
	if not p.pending[id] then return end
	p:forgetModalWindow(id)
	events.modalWindowHelper.onModalWindow(p, id, button, 0)
end
local function confirmation(p, callback)
	local w = ModalWindow {title = "Confirm", message = "Proceed?"}
	w:addButton("Yes", callback); w:addButton("No")
	w:setDefaultEnterButton("Yes"); w:setDefaultEscapeButton("No")
	w:sendToPlayer(p)
	return w
end

test("Escape cancels while Enter selects the actual Yes ID", function()
	local p, calls = player(), 0
	confirmation(p, function() calls = calls + 1 end)
	assert(nativeWindows[p.id].enter == 1 and nativeWindows[p.id].escape == 2)
	reply(p, nativeWindows[p.id].escape)
	assert(calls == 0 and not ModalWindows[p.id])
	confirmation(p, function() calls = calls + 1 end)
	reply(p, nativeWindows[p.id].enter)
	assert(calls == 1)
end)
test("replayed reply invokes a callback only once", function()
	local p, calls = player(), 0
	local w = confirmation(p, function() calls = calls + 1 end)
	reply(p, 1); reply(p, 1, w.modalWindowId)
	assert(calls == 1 and w.using == 0 and not ModalWindows[p.id])
end)
test("callback failure cannot retain its consumed window", function()
	local p = player()
	local w = confirmation(p, function() error("expected callback failure") end)
	reply(p, 1)
	assert(not ModalWindows[p.id] and w.using == 0)
end)
test("failed send does not register callbacks", function()
	local p = player(); p.failSend = true
	local w = ModalWindow {title = "test", message = "test"}; w:addButton("Yes")
	assert(not w:sendToPlayer(p) and not ModalWindows[p.id] and w.using == 0)
end)
test("logout releases callbacks capturing a player", function()
	local p = player(); local id = p.id
	local weak = setmetatable({p}, {__mode = "v"})
	do local captured = p; confirmation(p, function() return captured.id end) end
	events.modalWindowLogout.onLogout(p)
	players[id] = nil; p = nil
	collectgarbage("collect")
	assert(not ModalWindows[id] and not weak[1])
end)
test("forced disconnect sweep releases captured player", function()
	local p = player(); local id = p.id
	local weak = setmetatable({p}, {__mode = "v"})
	do local captured = p; confirmation(p, function() return captured.id end) end
	players[id] = nil; p = nil
	globals.modalWindowCleanup.onThink(60000)
	collectgarbage("collect")
	assert(not ModalWindows[id] and not weak[1])
end)
test("expired window cannot run its callback before sweep", function()
	local p, calls = player(), 0
	confirmation(p, function() calls = calls + 1 end)
	now = now + 901; reply(p, 1)
	assert(calls == 0 and not ModalWindows[p.id])
end)
test("sweep expires unanswered windows of online players", function()
	local p = player(); confirmation(p, function() end)
	now = now + 901; globals.modalWindowCleanup.onThink(60000)
	assert(not ModalWindows[p.id])
	assert(not next(p.pending))
end)
test("reloading scripts uses fresh IDs and releases old callbacks", function()
	local p = player(); local old = confirmation(p, function() end)
	dofile("data/scripts/creaturescripts/modal_window_helper.lua")
	local new = confirmation(p, function() end)
	assert(new.modalWindowId > old.modalWindowId and not ModalWindows[p.id][old.modalWindowId])
	assert(not p.pending[old.modalWindowId] and p.pending[new.modalWindowId])
end)
test("repeated cancellation releases obsolete native pending IDs", function()
	local p = player()
	for n = 1, 100 do
		local w = confirmation(p, function() end)
		ModalWindow.clearPlayer(p.id, w.modalWindowId)
	end
	assert(not next(p.pending) and not ModalWindows[p.id])
end)
test("a shared window is independently consumed for each player", function()
	local a, b, calls = player(), player(), 0
	local w = confirmation(a, function() calls = calls + 1 end)
	w:sendToPlayer(b)
	reply(a, 1); assert(w.using == 1 and not ModalWindows[a.id] and ModalWindows[b.id])
	reply(b, 1); assert(w.using == 0 and not ModalWindows[b.id] and calls == 2)
end)
print(string.format("%d events modal regression tests passed", passed))
