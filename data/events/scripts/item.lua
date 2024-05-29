function Item:onImbue(imbuement, firstCreation)
	local onImbue = EventCallback.onImbue
	if onImbue and firstCreation then
		onImbue(self, imbuement, firstCreation)
	end
	return true
end

function Item:onRemoveImbue(imbueType, decayRemoved)
	local onRemoveImbue = EventCallback.onRemoveImbue
	if EventCallback.onRemoveImbue then
		onRemoveImbue(self, imbueType, decayRemoved)
	end
end