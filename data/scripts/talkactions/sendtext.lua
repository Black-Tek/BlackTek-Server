local talk = TalkAction("/sendtext")

function talk.onSay(player, words, param)
    if not player:getGroup():getAccess() then
        return true
    end
    player:sendTextMessage(MESSAGE_EXPERIENCE, "Experience display test.", player:getPosition(), 100, TEXTCOLOR_WHITE_EXP)
    return false
end

talk:separator(" ")
talk:register()
