function onStepOut(creature, item, position, fromPosition)
    local tile = Tile(position)
    if tile and tile:getCreatureCount() > 0 then
        return true
    end

    local newPosition = position
    local newTile = Tile(newPosition)

    if newTile and not newTile:hasFlag(TILESTATE_BLOCKSOLID) then
        doRelocate(position, newPosition)
    end

    local i, tileItem, tileCount = 1, true, tile:getThingCount()
    while tileItem and i < tileCount do
        tileItem = tile:getThing(i)
        if tileItem and tileItem:getUniqueId() ~= item.uid and tileItem:getType():isMovable() then
            tileItem:remove()
        else
            i = i + 1
        end
    end

    item:transform(item.itemid - 1)
    return true
end