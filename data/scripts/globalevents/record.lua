local new_record = GlobalEvent("global_new_record")

new_record.onRecord = function(current, old)
	addEvent(Game.broadcastMessage, 150, "New record: " .. current .. " players are logged in.", MESSAGE_STATUS_DEFAULT)
	return true
end

new_record:type("record")
new_record:register()