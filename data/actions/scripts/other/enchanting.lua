local items = {
	equipment = {
		[2147] = { -- small ruby
			[Combat.DamageType.Fire] = {id = 2343, targetId = 2147} -- helmet of the ancients (enchanted)
		},
		[2383] = { -- spike sword
			[Combat.DamageType.Fire] = {id = 7744}, [Combat.DamageType.Ice] = {id = 7763},
			[Combat.DamageType.Earth] = {id = 7854}, [Combat.DamageType.Energy] = {id = 7869}
		},
		[2391] = { -- war hammer
			[Combat.DamageType.Fire] = {id = 7758}, [Combat.DamageType.Ice] = {id = 7777},
			[Combat.DamageType.Earth] = {id = 7868}, [Combat.DamageType.Energy] = {id = 7883}
		},
		[2423] = { -- clerical mace
			[Combat.DamageType.Fire] = {id = 7754}, [Combat.DamageType.Ice] = {id = 7773},
			[Combat.DamageType.Earth] = {id = 7864}, [Combat.DamageType.Energy] = {id = 7879}
		},
		[2429] = { -- barbarian axe
			[Combat.DamageType.Fire] = {id = 7749}, [Combat.DamageType.Ice] = {id = 7768},
			[Combat.DamageType.Earth] = {id = 7859}, [Combat.DamageType.Energy] = {id = 7874}
		},
		[2430] = { -- knight axe
			[Combat.DamageType.Fire] = {id = 7750}, [Combat.DamageType.Ice] = {id = 7769},
			[Combat.DamageType.Earth] = {id = 7860}, [Combat.DamageType.Energy] = {id = 7875}
		},
		[2445] = { -- crystal mace
			[Combat.DamageType.Fire] = {id = 7755}, [Combat.DamageType.Ice] = {id = 7774},
			[Combat.DamageType.Earth] = {id = 7865}, [Combat.DamageType.Energy] = {id = 7880}
		},
		[2454] = { -- war axe
			[Combat.DamageType.Fire] = {id = 7753}, [Combat.DamageType.Ice] = {id = 7772},
			[Combat.DamageType.Earth] = {id = 7863}, [Combat.DamageType.Energy] = {id = 7878}
		},
		[7380] = { -- headchopper
			[Combat.DamageType.Fire] = {id = 7752}, [Combat.DamageType.Ice] = {id = 7771},
			[Combat.DamageType.Earth] = {id = 7862}, [Combat.DamageType.Energy] = {id = 7877}
		},
		[7383] = { -- relic sword
			[Combat.DamageType.Fire] = {id = 7745}, [Combat.DamageType.Ice] = {id = 7764},
			[Combat.DamageType.Earth] = {id = 7855}, [Combat.DamageType.Energy] = {id = 7870}
		},
		[7384] = { -- mystic blade
			[Combat.DamageType.Fire] = {id = 7746}, [Combat.DamageType.Ice] = {id = 7765},
			[Combat.DamageType.Earth] = {id = 7856}, [Combat.DamageType.Energy] = {id = 7871}
		},
		[7389] = { -- heroic axe
			[Combat.DamageType.Fire] = {id = 7751}, [Combat.DamageType.Ice] = {id = 7770},
			[Combat.DamageType.Earth] = {id = 7861}, [Combat.DamageType.Energy] = {id = 7876}
		},
		[7392] = { -- orcish maul
			[Combat.DamageType.Fire] = {id = 7757}, [Combat.DamageType.Ice] = {id = 7776},
			[Combat.DamageType.Earth] = {id = 7867}, [Combat.DamageType.Energy] = {id = 7882}
		},
		[7402] = { -- dragon slayer
			[Combat.DamageType.Fire] = {id = 7748}, [Combat.DamageType.Ice] = {id = 7767},
			[Combat.DamageType.Earth] = {id = 7858}, [Combat.DamageType.Energy] = {id = 7873}
		},
		[7406] = { -- blacksteel sword
			[Combat.DamageType.Fire] = {id = 7747}, [Combat.DamageType.Ice] = {id = 7766},
			[Combat.DamageType.Earth] = {id = 7857}, [Combat.DamageType.Energy] = {id = 7872}
		},
		[7415] = { -- cranial basher
			[Combat.DamageType.Fire] = {id = 7756}, [Combat.DamageType.Ice] = {id = 7775},
			[Combat.DamageType.Earth] = {id = 7866}, [Combat.DamageType.Energy] = {id = 7881}
		},
		[8905] = { -- rainbow shield
			[Combat.DamageType.Fire] = {id = 8906}, [Combat.DamageType.Ice] = {id = 8907},
			[Combat.DamageType.Earth] = {id = 8909}, [Combat.DamageType.Energy] = {id = 8908}
		},
		[9949] = { -- dracoyle statue
			[Combat.DamageType.Earth] = {id = 9948} -- dracoyle statue (enchanted)
		},
		[9954] = { -- dracoyle statue
			[Combat.DamageType.Earth] = {id = 9953} -- dracoyle statue (enchanted)
		},
		[10022] = { -- worn firewalker boots
			[Combat.DamageType.Fire] = {id = 9933, say = {text = "Take the boots off first."}},
			slot = {type = CONST_SLOT_FEET, check = true}
		},
		[24716] = { -- werewolf amulet
			[Combat.DamageType.None] = {
				id = 24717,
				effects = {failure = CONST_ME_POFF, success = CONST_ME_THUNDER},
				message = {text = "The amulet cannot be enchanted while worn."}
			},
			slot = {type = CONST_SLOT_NECKLACE, check = true}
		},
		[24718] = { -- werewolf helmet
			[Combat.DamageType.None] = {
				id = {
					[SKILL_CLUB] = {id = 24783},
					[SKILL_SWORD] = {id = 24783},
					[SKILL_AXE] = {id = 24783},
					[SKILL_DISTANCE] = {id = 24783},
					[SKILL_MAGLEVEL] = {id = 24783}
				},
				effects = {failure = CONST_ME_POFF, success = CONST_ME_THUNDER},
				message = {text = "The helmet cannot be enchanted while worn."},
				usesStorage = true
			},
			slot = {type = CONST_SLOT_HEAD, check = true}
		},
		charges = 1000, effect = CONST_ME_MAGIC_RED
	},

	valuables = {
		[2146] = {id = 7759, shrine = {7508, 7509, 7510, 7511}}, -- small sapphire
		[2147] = {id = 7760, shrine = {7504, 7505, 7506, 7507}}, -- small ruby
		[2149] = {id = 7761, shrine = {7516, 7517, 7518, 7519}}, -- small emerald
		[2150] = {id = 7762, shrine = {7512, 7513, 7514, 7515}}, -- small amethyst
		soul = 2, mana = 300, effect = CONST_ME_HOLYDAMAGE
	},

	[2342] = {combatType = Combat.DamageType.Fire, targetId = 2147}, -- helmet of the ancients
	[7759] = {combatType = Combat.DamageType.Ice}, -- small enchanted sapphire
	[7760] = {combatType = Combat.DamageType.Fire}, -- small enchanted ruby
	[7761] = {combatType = Combat.DamageType.Earth}, -- small enchanted emerald
	[7762] = {combatType = Combat.DamageType.Energy}, -- small enchanted amethyst
	[24739] = {combatType = Combat.DamageType.None} -- moonlight crystals
}

function onUse(player, item, fromPosition, target, toPosition, isHotkey)
	if not target or not target:isItem() then
		return false
	end

	local itemId, targetId = item:getId(), target:getId()
	local targetType = items.valuables[itemId] or items.equipment[items[itemId].targetId or targetId]
	if not targetType then
		return false
	end

	if targetType.shrine then
		if not table.contains(targetType.shrine, targetId) then
			player:sendCancelMessage(RETURNVALUE_NOTPOSSIBLE)
			return true
		end

		if player:getMana() < items.valuables.mana then
			player:sendCancelMessage(RETURNVALUE_NOTENOUGHMANA)
			return true
		end

		if player:getSoul() < items.valuables.soul then
			player:sendCancelMessage(RETURNVALUE_NOTENOUGHSOUL)
			return true
		end
		player:addSoul(-items.valuables.soul)
		player:addMana(-items.valuables.mana)
		player:addManaSpent(items.valuables.mana)
		player:addItem(targetType.id)
		player:getPosition():sendMagicEffect(items.valuables.effect)
		item:remove(1)
	else
		local targetItem = targetType[items[itemId].combatType]
		if not targetItem or targetItem.targetId and targetItem.targetId ~= targetId then
			return false
		end

		local isInSlot = targetType.slot and targetType.slot.check and target:getType():usesSlot(targetType.slot.type) and Player(target:getParent())
		if isInSlot then
			if targetItem.say then
				player:say(targetItem.say.text, TALKTYPE_MONSTER_SAY)
				return true
			elseif targetItem.message then
				player:sendTextMessage(MESSAGE_EVENT_ADVANCE, targetItem.message.text)
			else
				return false
			end
		else
			if targetItem.targetId then
				item:transform(targetItem.id)
				item:decay()
				target:remove(1)
			else
				if targetItem.usesStorage then
					local vocationId = player:getVocation():getDemotion():getId()
					local storage = storages[itemId] and storages[itemId][targetId] and storages[itemId][targetId][vocationId]
					if not storage then
						return false
					end

					local storageValue = player:getStorageValue(storage.key)
					if storageValue == -1 then
						return false
					end

					local transform = targetItem.id and targetItem.id[storageValue]
					if not transform then
						return false
					end
					target:transform(transform.id)
				else
					target:transform(targetItem.id)
				end

				if target:hasAttribute(ITEM_ATTRIBUTE_DURATION) then
					target:decay()
				end

				if target:hasAttribute(ITEM_ATTRIBUTE_CHARGES) then
					target:setAttribute(ITEM_ATTRIBUTE_CHARGES, items.equipment.charges)
				end
				item:remove(1)
			end
		end
		player:getPosition():sendMagicEffect(targetItem.effects and (isInSlot and targetItem.effects.failure or targetItem.effects.success) or items.equipment.effect)
	end
	return true
end
