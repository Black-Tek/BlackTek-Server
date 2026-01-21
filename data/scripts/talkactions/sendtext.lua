local talk = TalkAction("/sendtext")

function talk.onSay(player, words, param)

    local packet = NetworkMessage()
    local pos = Position(92, 123, 7)

    packet:addByte(0xB4)
    packet:addPosition(pos)
    packet:addU32(100)
    packet:addByte(TEXTCOLOR_WHITE_EXP)
    packet:addString("you gaining")
    packet:sendToPlayer(player)

end

talk:separator(" ")
talk:register()