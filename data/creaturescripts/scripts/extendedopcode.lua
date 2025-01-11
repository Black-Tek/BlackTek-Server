local OPCODE_LANGUAGE = 1

function onExtendedOpcode(player, opcode, buffer)
    local points = 0
    local resultId = db.storeQuery("SELECT `ancestral_points` FROM `players` WHERE `id` = " .. player:getGuid())
    if resultId ~= false then
      points = result.getDataInt(resultId, "ancestral_points")
      result.free(resultId)
    end
    player:sendExtendedOpcode(89, points)
    if opcode == OPCODE_LANGUAGE then
        -- otclient language
        if buffer == 'en' or buffer == 'pt' then
            -- example, setting player language, because otclient is multi-language...
            -- player:setStorageValue(SOME_STORAGE_ID, SOME_VALUE)
        end
    else
        -- other opcodes can be ignored, and the server will just work fine...
    end
end
