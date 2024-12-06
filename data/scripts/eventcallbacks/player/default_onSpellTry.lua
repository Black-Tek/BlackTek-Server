local ec = EventCallback

ec.onSpellTry = function(self, spell, spellType)
	return true
end

ec:register()
