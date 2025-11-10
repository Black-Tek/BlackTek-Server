levelCheckConfig = {
	normal = 1,   					-- aid 1000-2000 | lvl 0-1000 | +1 aid = +1 level
	normalLevelOffset = 0,   		-- level check starts at this level

	extended = 10, 					-- aid 2001-3000 | lvl 1010-10000 | +1 aid = +10 levels
	extendedLevelOffset = 1010, 	-- extended level check starts at this level
}

actionIds = {
	sandHole = 100, 				-- hidden sand hole
	pickHole = 105, 				-- hidden mud hole

	levelCheck = 1000,            	-- start of normal level check
	levelCheckLast = 2000,         	-- end of normal level check

	levelCheckExtended = 2001,    	-- start of extended level check
	levelCheckExtendedLast = 3000, 	-- end of extended level check

	citizenship = 30020,			-- citizenship teleport
	citizenshipLast = 30050,		-- citizenship teleport last
}
