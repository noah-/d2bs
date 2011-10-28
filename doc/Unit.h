/** This class represents a unit (monster, wp, npc, character, etc) in the
 * game.
 */
class Unit
{
public:
	/** Get the next unit that matches the originally given name or class id and
	 * originally specified mode.
	 *
	 * \return Whether another unit was found.
	 */
	bool getNext();

	/** Get the next unit that matches the given name and originally specified
	 * mode.
	 *
	 * \param szName The name to look for.
	 *
	 * \return Whether another unit was found.
	 */
	bool getNext(String szName);

	/** Get the next unit that matches the given class id and originally 
	 * specified mode.
	 *
	 * \param dwClassId The class id to look for.
	 *
	 * \return Whether another unit was found.
	 */
	bool getNext(uint32_t dwClassId);

	/** Get the next unit that matches the given name and mode.
	 *
	 * \param szName The name to look for.
	 *
	 * \param dwMode Either the mode being searched for, or a bitmask with bit
	 * 29 set and bits 0-28 corresponding to modes of units being searched for.
	 *
	 * \return Whether another unit was found.
	 */
	bool getNext(String szName, uint32_t dwMode);

	/** Get the next unit that matches the given class id and mode.
	 *
	 * \param dwClassId The class id to look for.
	 *
	 * \param dwMode Either the mode being searched for, or a bitmask with bit
	 * 29 set and bits 0-28 corresponding to modes of units being searched for.
	 *
	 * \return Whether another unit was found.
	 */
	bool getNext(uint32_t dwClassId, uint32_t dwMode);

	/** Cancel whatever's going on.
	 *
	 * If there's scrolling text clear it.
	 * If you're interacting to an NPC, stop.
	 * If there's an item on the cursor, drop it.
	 * Otherwise close any other interaction that might be going on.
	 */
	void cancel();

	/** Close some form of interaction.
	 *
	 * \todo Clarify what each of the calls do.
	 *
	 * \param type Type of interaction to cancel
	 *
	 * 0 - Call D2CLIENT_CloseInteract
	 *
	 * 1 - Call D2CLIENT_CloseNPCInteract
	 */
	void cancel(int type);

	/** Try to repair.
	 *
	 * Need to be able to find unit that you're trying to repair with. That
	 * means the unit needs to still be in the server unit hash table.
	 *
	 * \return true if successful.
	 */
	bool repair();

	/** Use an NPC menu.
	 *
	 * Need to be able to find unit that you're trying to repair with. That
	 * means the unit needs to still be in the server unit hash table.
	 *
	 * \param menuItem Index of the menu item to click.
	 * 
	 * \return true if menu was found.
	 */
	bool useMenu(int menuItem);

	/** Interact with the unit.
	 *
	 * If the unit is an item in inventory pick it up. Otherwise click it on
	 * the map.
	 */
	bool interact();

	/** Interact with a waypoint.
	 *
	 * If the unit is an object, assume it's a waypoint, and take it.
	 *
	 * \param destId The area id of the destination to select.
	 */
	bool interact(int destId);

	/** Get the first item from inventory.
	 *
	 * \return The first item from inventory.
	 */
	Unit getItem();

	/** Get an item from inventory by name.
	 *
	 * \param name The name of the item to look for.
	 *
	 * \return The first item found that matches the description.
	 */
	Unit getItem(String name);

	/** Get an item from inventory by classId.
	 *
	 * \param classId The class id of the unit.
	 *
	 * \return The first item found that matches the description.
	 */
	Unit getItem(uint32_t classId);

	/** Get an item from inventory by name and mode.
	 *
	 * \param name The name of the unit to look for.
	 *
	 * \param mode Either the mode being searched for, or a bitmask with bit
	 * 29 set and bits 0-28 corresponding to modes of units being searched
	 * for.
	 *
	 * \return The first item found that matches the description.
	 */
	Unit getItem(String name, uint32_t mode);

	/** Get an item from inventory by classId and mode.
	 *
	 * \param classId The class id of the unit.
	 *
	 * \param mode Either the mode being searched for, or a bitmask with bit
	 * 29 set and bits 0-28 corresponding to modes of units being searched
	 * for.
	 *
	 * \return The first item found that matches the description.
	 */
	Unit getItem(uint32_t classId, uint32_t mode);

	/** Get an item from inventory by name, mode and nUnitId.
	 *
	 * \param name The name of the unit to look for.
	 *
	 * \param mode Either the mode being searched for, or a bitmask with bit
	 * 29 set and bits 0-28 corresponding to modes of units being searched
	 * for.
	 *
	 * \param nUnitId The unique id of the unit.
	 *
	 * \return The first item found that matches the description.
	 */
	Unit getItem(String name, uint32_t mode, uint32_t nUnitId);

	/** Get an item from inventory by classId, mode and nUnitId.
	 *
	 * \param classId The class id of the unit.
	 *
	 * \param mode Either the mode being searched for, or a bitmask with bit
	 * 29 set and bits 0-28 corresponding to modes of units being searched
	 * for.
	 *
	 * \param nUnitId The unique id of the unit.
	 *
	 * \return The first item found that matches the description.
	 */
	Unit getItem(uint32_t classId, uint32_t mode, uint32_t nUnitId);

	/** Get all items from inventory.
	 *
	 * \return An array of all the items from inventory.
	 */
	Unit[] getItems();

	/** Get whether this player has a merc.
	 *
	 * The unit being operated on should be a player.
	 *
	 * \param dummy Used to signify boolean result instead of object. Should
	 * be true.
	 *
	 * \return true if the player has a live merc.
	 */
	bool getMerc(bool dummy);

	/** Get a player's merc.
	 *
	 * The unit being operated on should be a player.
	 *
	 * \return The merc if there is one.
	 */
	Unit getMerc();

	/** Get skill name from hand.
	 *
	 * \param hand Hand to get the skill name off.
	 *
	 * 0 - Right hand
	 *
	 * 1 - Left hand
	 *
	 * \return Skill name.
	 */
	String getSkill(int hand);

	/** Get skill id from hand.
	 *
	 * \param hand Hand to get the skill id off.
	 *
	 * 2 - Right hand
	 *
	 * 3 - Left hand
	 *
	 * \return Skill id.
	 */
	int getSkill(int hand);

	/** Get all skills.
	 *
	 * Gets all skill ids, along with corresponding base and total skill
	 * levels.
	 *
	 * \param dummy Should be 4.
	 *
	 * \return An array of arrays of integers. Inside the inner array the
	 * 0th index is the skill id, the 1st index is the base skill level and
	 * the 2nd index is the total skill level. Only the first 256 skills are
	 * read.
	 */
	int[][] getSkill(int dummy);

	/** Calls D2COMMON_GetSkillLevel and returns the result.
	 *
	 * \todo Figure out what D2COMMON_GetSkillLevel does.
	 *
	 * \returns Whatever D2COMMON_GetSkillLevel returns.
	 */
	int getSkill(int nSkillId, int nExt);

	/** Gets the parent of a unit.
	 *
	 * This function is used for monster and item units.
	 *
	 * \return The parent unit.
	 */
	Unit getParent();

	/** Gets the parent of a unit.
	 *
	 * This function is used for object units.
	 *
	 * \return The parent's name.
	 */
	String getParent();

	/** Puts the string equivalent of message over the unit.
	 */
	void overhead(Object message);

	/** Returns the item flags.
	 *
	 * \returns Item flags:
	 *
	 * 0x00000001 - Equipped
	 *
	 * 0x00000008 - In socket
	 *
	 * 0x00000010 - Identified
	 *
	 * 0x00000040 - Weapon/shield is in the active weapon switch
	 *
	 * 0x00000080 - Weapon/shield is in the inactive weapon switch
	 *
	 * 0x00000100 - Item is broken
	 *
	 * 0x00000400 - Full rejuv
	 *
	 * 0x00000800 - Socketed
	 *
	 * 0x00002000 - In the trade or gamble screen
	 *
	 * 0x00004000 - Not in a socket
	 *
	 * 0x00010000 - Is an ear
	 *
	 * 0x00020000 - A starting item (worth only 1 gold)
	 *
	 * 0x00200000 - Rune, or potion, or mephisto's soulstone
	 *
	 * 0x00400000 - Ethereal
	 *
	 * 0x00800000 - Is an item
	 *
	 * 0x01000000 - Personalized
	 *
	 * 0x04000000 - Runeword
	 *
	 * Source: http://subversion.assembla.com/svn/d2bs/scripts/YAMB/libs/YAMB/common/YAM-Common.dbl r1086
	 */
	int getFlags();

	/** Same as getFlags, but returns a boolean.
	 *
	 * Returns true if any of the flags given match the item flags.
	 *
	 * \ref getFlags
	 *
	 * \param flags Flags to check.
	 *
	 * \return true if any of the flags are set in the item flags.
	 */
	bool getFlag(int flags);

	/** Get a stat by stat id.
	 *
	 * Used for everything except stats 6-11.
	 *
	 * \param nStat The stat type.
	 * See http://forums.d2botnet.org/viewtopic.php?f=18&t=989
	 *
	 * \return The stat value.
	 */
	double getStat(int nStat);

	/** Get a stat by stat id and sub index.
	 *
	 * Used for stats 6-11.
	 *
	 * \param nStat The stat type.
	 * See http://forums.d2botnet.org/viewtopic.php?f=18&t=989
	 *
	 * \return The stat value.
	 */
	int getStat(int nStat);

	/** Get a stat by stat id and sub index.
	 *
	 * Used for stat 13.
	 *
	 * \param nStat The stat type.
	 * See http://forums.d2botnet.org/viewtopic.php?f=18&t=989
	 *
	 * \param nSubIndex A subindex to a certain type of stat. For instance a
	 * specific skill for the +1 to skill tab stat.
	 *
	 * \return The stat value.
	 */
	double getStat(int nStat, int nSubIndex);

	/** Get a stat by stat id and sub index.
	 *
	 * Used for everything except stat 13.
	 *
	 * \param nStat The stat type.
	 * See http://forums.d2botnet.org/viewtopic.php?f=18&t=989
	 *
	 * \param nSubIndex A subindex to a certain type of stat. For instance a
	 * specific skill for the +1 to skill tab stat.
	 *
	 * \return The stat value.
	 */
	int getStat(int nStat, int nSubIndex);

	/** Get an array of all the stats of the item.
	 *
	 * \param nStat Set to -1.
	 *
	 * \return An array of the first 64 stats. The indices of the inner array
	 * are: 0 - nStat, 1 - nSubIndex, 2 - nValue.
	 */
	int[][] getStat(int nStat);

	/** Get an array of all the stats of the item.
	 *
	 * \param nStat Set to -1.
	 *
	 * \param nSubIndex Ignored.
	 *
	 * \return An array of the first 64 stats. The indices of the inner array
	 * are: 0 - nStat, 1 - nSubIndex, 2 - nValue.
	 */
	int[][] getStat(int nStat, int nSubIndex);

	/** Return whether or not the unit has a given state.
	 *
	 * \param nState The state id.
	 *
	 * \return Whether or not the unit has the state.
	 */
	bool getState(int nState);

	/** Get the price of the item at npc 148, with "buysell" of 0, in the
	 * current difficult.
	 *
	 * \todo Determine if this is the buy or sell price. "buysell" is 0.
	 *
	 * \return Some sort of price.
	 */
	int getPrice();

	/** Get the price of the item at a given npc, with "buysell" of 0, in the
	 * current difficulty.
	 *
	 * \todo Determine the meaning of "buysell".
	 *
	 * \param npc The npc to determine the price at.
	 *
	 * \return The price requested.
	 */
	int getPrice(Unit npc);

	/** Get the price of the item at a given npc (by id), with "buysell" of 0,
	 * in the current difficulty.
	 *
	 * \todo Determine the meaning of "buysell".
	 *
	 * \param npcId The id of the npc to determine the price at.
	 *
	 * \return The price requested.
	 */
	int getPrice(int npcId);

	/** Get the price of the item at a given npc, with choice of buying or
	 * selling, in the current difficulty.
	 *
	 * \todo Determine the meaning of "buysell".
	 *
	 * \param npc The npc to determine the price at.
	 *
	 * \param buysell Unknown
	 *
	 * \return The price requested.
	 */
	int getPrice(Unit npc, int buysell);

	/** Get the price of the item at a given npc (by id), which choice of buying
	 * or selling, in the current difficulty.
	 *
	 * \todo Determine the meaning of "buysell".
	 *
	 * \param npcId The id of the npc to determine the price at.
	 *
	 * \param buysell Unknown
	 *
	 * \return The price requested.
	 */
	int getPrice(int npcId, int buysell);

	/** Get the price of the item at a given npc, with choice of buying or
	 * selling, in a given difficulty.
	 *
	 * \todo Determine the meaning of "buysell".
	 *
	 * \param npc The npc to determine the price at.
	 *
	 * \param buysell Unknown
	 *
	 * \param difficulty The difficulty of interest: 0 - normal, 1 - nightmare,
	 * 2 - hell
	 *
	 * \return The price requested.
	 */
	int getPrice(Unit npc, int buysell, int difficulty);

	/** Get the price of the item at a given npc (by id), with choice of buying
	 * or selling, in a given difficulty.
	 *
	 * \todo Determine the meaning of "buysell".
	 *
	 * \param npcId The id of the npc to determine the price at.
	 *
	 * \param buysell Unknown
	 *
	 * \param difficulty The difficulty of interest: 0 - normal, 1 - nightmare,
	 * 2 - hell
	 *
	 * \return The price requested.
	 */
	int getPrice(int npcId, int buysell, int difficulty);

	/** Determine whether or not a unit has a given enchant.
	 *
	 * \param nEnchant Enchantment id.
	 *
	 * \return Whether or not the unit has the enchantment.
	 */
	bool getEnchant(int nEnchant);

	/** Shop with a given npc, either buying or selling.
	 *
	 * \param dwMode What to do with the item. 1 - Sell, 2 - Buy, 6 - ?
	 *
	 * \return Whether or not the shop went through.
	 */
	bool shop(int dwMode);

	/** Set the skill on the given hand to be skill with name skillName.
	 *
	 * Waits up to one second for the skill to be set.
	 *
	 * \todo Fix argc < 1, should be argc < 2
	 *
	 * \param skillName Name of the skill to put up.
	 *
	 * \param nHand Hand to put the skill on. non-zero left, 0 - right.
	 *
	 * \return Whether operation was successful.
	 */
	bool setSkill(String skillName, int nHand);

	/** Set the skill on the given hand to be skill with id nSkillId.
	 *
	 * Waits up to one second for the skill to be set.
	 *
	 * \todo Fix argc < 1, should be argc < 2
	 *
	 * \param nSkillId Id of the skill to put up.
	 *
	 * \param nHand Hand to put the skill on. non-zero left, 0 - right.
	 *
	 * \return Whether operation was successful.
	 */
	bool setSkill(int nSkillId, int nHand);

	/** Move to the given location.
	 *
	 * \param x The x location.
	 * 
	 * \param y The y location.
	 */
	void move(int x, int y);

	/** Move to this unit.
	 */
	void move();

	/** Get the quest flag for a quest specified by act and quest number.
	 *
	 * \param nAct The act of the quest.
	 *
	 * \param nQuest The quest number.
	 *
	 * \return The quest flag for the specified quest.
	 */
	int getQuest(int nAct, int nQuest);

	/** Get the number of minions of a certain type.
	 *
	 * \param nType The type of the minions.
	 *
	 * \return The number of minions of the specified type.
	 */
	int getMinionCount(int nType);

	/** Get price to repair this unit at the current npc or npc 0x9A if not
	 * currently interacting.
	 *
	 * \return The price to repair the given unit.
	 */
	int getRepairCost();

	/** Get price to repair this unit at the current npc given by nNpcClassId.
	 *
	 * \param nNpcClassId The class id of the npc to get the price for repair
	 * at.
	 *
	 * \return The price to repair the given unit.
	 */
	int getRepairCost(int nNpcClassId);

	/** Get the cost to do something (buy, sell, repair) with the given item.
	 *
	 * \param nMode What to do: 0 - buy, 1 - sell, 2 - repair.
	 *
	 * \return The price.
	 */
	int getItemCost(int nMode);

	/** Get the cost to do something (buy, sell, repair) with the given item, at
	 * the given npc.
	 *
	 * \param nMode What to do: 0 - buy, 1 - sell, 2 - repair.
	 *
	 * \param nNpcClassId The class id of the npc to check the price with.
	 *
	 * \return The price.
	 */
	int getItemCost(int nMode, int nNpcClassId);

	/** Get the cost to do something (buy, sell, repair) with the given item, at
	 * the given npc, in the given difficulty.
	 *
	 * \param nMode What to do: 0 - buy, 1 - sell, 2 - repair.
	 *
	 * \param nNpcClassId The class id of the npc to check the price with.
	 *
	 * \param nDifficulty The difficulty to check the price in.
	 *
	 * \return The price.
	 */
	int getItemCost(int nMode, int nNpcClassId, int nDifficulty);

	/** The type of the unit.
	 *
	 * 0 - Player
	 *
	 * 1 - NPC
	 *
	 * 2 - Object
	 *
	 * 3 - Missile
	 *
	 * 4 - Item
	 *
	 * 5 - Warp
	 * 
	 * Source: botNET
	 */
	int type;

	/** The class id of the object.
	 *
	 * See: http://forums.d2botnet.org/viewtopic.php?f=18&t=1002
	 *
	 * http://forums.d2botnet.org/viewtopic.php?f=18&t=986 and
	 *
	 * http://forums.d2botnet.org/viewtopic.php?f=18&t=985
	 */
	int classId;

	/** The mode of the unit.
	 *
	 * See: http://forums.d2botnet.org/viewtopic.php?f=18&t=988
	 */
	int mode;

	/** The name of the unit.
	 */
	String name;

	/** The act where the unit is currently located.
	 */
	int act;

	/** The unit unique id. Referred to simply as the unit id. Used along with
	 * the unit type to uniquely identify the unit.
	 */
	int gid;

	/** The x location of the unit.
	 */
	int x;

	/** The y location of the unit.
	 */
	int y;

	/** The id of the area (level) the unit is located in.
	 *
	 * \todo Get a reference for the area IDs.
	 */
	int area;

	/** The unit's current health.
	 */
	int hp;

	/** The unit's maximum health.
	 */
	int hpmax;

	/** The unit's current mana.
	 */
	int mp;

	/** The unit's maximum mana.
	 */
	int mpmax;

	/** The unit's current stamina.
	 */
	int stamina;

	/** The unit's maximum stamina.
	 */
	int staminamax;

	/** The character level. The level that determines stat points, skill
	 * levels, etc.
	 */
	int charlvl;

	/** The number of items in inventory.
	 */
	int itemcount;

	/** The unit id of the owner of the unit.
	 */
	int owner;

	/** The type of owner.
	 *
	 * \ref type
	 */
	int ownertype;

	/** The special type of the unit. Represents things like unique, minion,
	 * boss, etc.
	 *
	 * 1 - "Normal" Boss
	 * 2 - Champion
	 * 4 - Boss
	 * 8 - Minion
	 */
	int spectype;

	/** The direction of the unit.
	 *
	 * \todo Figure out what the direction is.
	 */
	int direction;

	/** The unique id of a boss. Taken from pUnit->pMonsterData->wUniqueNo. Not
	 * the same thing as the gid (unit id).
	 */
	int uniqueid;

	/** The three digit code for an item.
	 *
	 * See: http://forums.d2botnet.org/viewtopic.php?f=18&t=991
	 */
	String code;

	/** The magic prefix of the item.
	 */
	String prefix;

	/** The magic suffix of the item.
	 */
	String suffix;

	/** The id code for the magic prefix.
	 */
	int prefixnum;

	/** The id code for the magic suffix.
	 */
	int suffixnum;

	/** The full name of an item.
	 */
	String fname;

	/** The item quality.
	 *
	 * 1 - Low quality
	 *
	 * 2 - Normal
	 *
	 * 3 - Superior
	 *
	 * 4 - Magic
	 *
	 * 5 - Set
	 *
	 * 6 - Rare
	 *
	 * 7 - Unique
	 *
	 * 8 - Crafted
	 *
	 * Source: botNET
	 */
	int quality;

	/** No clue.
	 *
	 * \todo Get a clue.
	 */
	int node;

	/** The location of the item (ground, inventory, stash, etc.).
	 *
	 * 0 - Ground
	 *
	 * 1 - Equipped
	 *
	 * 2 - Belt
	 *
	 * 3 - Inventory
	 *
	 * 4 - Store
	 *
	 * 5 - Trade
	 *
	 * 6 - Cube
	 *
	 * 7 - Stash
	 *
	 * Source: d2botnet code
	 */
	int location;

	/** The x size of the item.
	 */
	int sizex;

	/** The y size of the item.
	 */
	int sizey;

	/** The type of the item.
	 *
	 * \todo Put together a reference for item type.
	 */
	int itemType;

	/** The description of the item.
	 */
	String description;

	/** The equipped location of the item.
	 *
	 * 0 - Not equipped
	 *
	 * 1 - Helmet
	 *
	 * 2 - Amulet
	 *
	 * 3 - Armor
	 *
	 * 4 - Right hand slot 1
	 *
	 * 5 - Left hand slot 1
	 *
	 * 6 - Right ring
	 *
	 * 7 - Left ring
	 *
	 * 8 - Belt
	 *
	 * 9 - Boots
	 *
	 * 10 - Gloves
	 *
	 * 11 - Right hand slot 2
	 *
	 * 12 - Left hand slot 2
	 */
	int bodylocation;

	/** Item level. Used for things like item stat rolling.
	 */
	int ilvl;

	/** Level requirement (character level required to use the item).
	 */
	int ilvlreq;

	/** Whether the controlled character is in the always run mode or not.
	 *
	 * 0 - Walk unless directed to run
	 * 1 - Always run
	 */
	int runwalk;

	/** Set the weapon switch.
	 */
	int weaponswitch;

	/** The object type.
	 *
	 * \todo Figure out what this means.
	 */
	int objtype;

	/** Whether or not the chest is locked.
	 */
	int islocked;
};
