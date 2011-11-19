/** \defgroup globalFunctions Global functions
 */

/** Get a unit by type.
 *
 * \ingroup globalFunctions
 *
 * \param type The type of the unit. Set to -1 to get any unit. 100 gets the
 * cursor item, 101 gets the selected unit.
 *
 * \return The first unit found that matches the description.
 */
Unit getUnit(int type);

/** Get a unit by type and name.
 *
 * \ingroup globalFunctions
 *
 * \param type The type of the unit. Set to -1 to get any unit. 100 gets the
 * cursor item, 101 gets the selected unit.
 *
 * \param name The name of the unit to look for.
 *
 * \return The first unit found that matches the description.
 */
Unit getUnit(int type, char* name);

/** Get a unit by type and class id.
 *
 * \ingroup globalFunctions
 *
 * \param type The type of the unit. Set to -1 to get any unit. 100 gets the
 * cursor item, 101 gets the selected unit.
 *
 * \param classId The class id of the unit.
 *
 * \return The first unit found that matches the description.
 */
Unit getUnit(int type, uint32_t classId);

/** Get a unit by type, name and mode.
 *
 * \ingroup globalFunctions
 *
 * \param type The type of the unit. Set to -1 to get any unit. 100 gets the
 * cursor item, 101 gets the selected unit.
 *
 * \param name The name of the unit to look for.
 *
 * \param mode Either the mode being searched for, or a bitmask with bit 29 set
 * and bits 0-28 corresponding to modes of units being searched for.
 *
 * \return The first unit found that matches the description.
 */
Unit getUnit(int type, char* name, uint32_t mode);

/** Get a unit by type, classId and mode.
 *
 * \ingroup globalFunctions
 *
 * \param type The type of the unit. Set to -1 to get any unit. 100 gets the
 * cursor item, 101 gets the selected unit.
 *
 * \param classId The class id of the unit.
 *
 * \param mode Either the mode being searched for, or a bitmask with bit 29 set
 * and bits 0-28 corresponding to modes of units being searched for.
 *
 * \return The first unit found that matches the description.
 */
Unit getUnit(int type, uint32_t classId, uint32_t mode);

/** Get a unit by type, name, mode and nUnitId.
 *
 * \ingroup globalFunctions
 *
 * \param type The type of the unit. Set to -1 to get any unit. 100 gets the
 * cursor item, 101 gets the selected unit.
 *
 * \param name The name of the unit to look for.
 *
 * \param mode Either the mode being searched for, or a bitmask with bit 29 set
 * and bits 0-28 corresponding to modes of units being searched for.
 *
 * \param nUnitId The unique id of the unit.
 *
 * \return The first unit found that matches the description.
 */
Unit getUnit(int type, char* name, uint32_t mode, uint32_t nUnitId);

/** Get a unit by type, classId, mode and nUnitId.
 *
 * \ingroup globalFunctions
 *
 * \param type The type of the unit. Set to -1 to get any unit. 100 gets the
 * cursor item, 101 gets the selected unit.
 *
 * \param classId The class id of the unit.
 *
 * \param mode Either the mode being searched for, or a bitmask with bit 29 set
 * and bits 0-28 corresponding to modes of units being searched for.
 *
 * \param nUnitId The unique id of the unit.
 *
 * \return The first unit found that matches the description.
 */
Unit getUnit(int type, uint32_t classId, uint32_t mode, uint32_t nUnitId);

/** Creates a path walking from the source to the destination.
 * Returns an array of points that form a path from the source to the
 * destination. Takes Area ID, source and destination points as parameters.
 *
 * \ingroup globalFunctions
 *
 * \param Area The Area ID to path through.
 *
 * \param srcX The source X coordinate.
 *
 * \param srcY The source Y coordinate.
 *
 * \param dstX The destination X coordinate.
 *
 * \param dstY The destination Y coordinate.
 *
 * \return The path as an array of objects with x and y properties.
 */
object[] getPath(uint32_t Area, uint32_t srcX, uint32_t srcY, uint32_t dstX,
	int dstY);

/** Creates a path (walking or teleporting) from the source to the destination.
 * Returns an array of points that form a path from the source to the
 * destination. Takes Area ID, source and destination points, and walking or
 * teleporting as parameters.
 *
 * \ingroup globalFunctions
 *
 * \param Area The Area ID to path through.
 *
 * \param srcX The source X coordinate.
 *
 * \param srcY The source Y coordinate.
 *
 * \param dstX The destination X coordinate.
 *
 * \param dstY The destination Y coordinate.
 *
 * \param reductionType The type of reduction.
 * 	0 - walking
 * 	1 - teleport
 * 	2 - none
 *
 * \return The path as an array of objects with x and y properties.
 */
object[] getPath(uint32_t Area, uint32_t srcX, uint32_t srcY, uint32_t dstX,
	int dstY, uint32_t reductionType);

/** Creates a path (walking or teleporting) from the source to the destination.
 * Returns an array of points that form a path from the source to the
 * destination. Takes Area ID(s), source and destination points, walking or
 * teleporting and range as parameters.
 *
 * \ingroup globalFunctions
 *
 * \param Area The Area ID to path through.
 *
 * \param srcX The source X coordinate.
 *
 * \param srcY The source Y coordinate.
 *
 * \param dstX The destination X coordinate.
 *
 * \param dstY The destination Y coordinate.
 *
 * \param reductionType The type of reduction.
 * 	0 - walking
 * 	1 - teleport
 * 	2 - none
 *
 * \param Radius The distance between each point.
 *
 * \return The path as an array of objects with x and y properties.
 */
object[] getPath(uint32_t Area, uint32_t srcX, uint32_t srcY, uint32_t dstX,
	int dstY, uint32_t reductionType, uint32_t Radius);

/** Creates a path (walking or teleporting) from the source to the destination.
 * Returns an array of points that form a path from the source to the
 * destination. Takes Area ID, source and destination points, walking or
 * teleporting, range and reduction or not as parameters.
 *
 * \ingroup globalFunctions
 *
 * \param Area The Area ID to path through.
 *
 * \param srcX The source X coordinate.
 *
 * \param srcY The source Y coordinate.
 *
 * \param dstX The destination X coordinate.
 *
 * \param dstY The destination Y coordinate.
 *
 * \param reductionType The type of reduction.
 * 	0 - walking
 * 	1 - teleport
 * 	2 - none
 * 	3 - user specified
 *
 * \param Radius The distance between each point.
 *
 * \param reject Called on start and end to determine if point is valid.
 *
 * \param reduce Function to determine whether to keep point. Returns true if
 * 	point should be kept, false if point should be eliminated.
 *
 * \param mutate Called on the start/end if it isn't/is rejected respectively.
 * 	This is done before path generation.
 *
 * \return The path as an array of objects with x and y properties.
 */
object[] getPath(uint32_t Area, uint32_t srcX, uint32_t srcY, uint32_t dstX,
	int dstY, uint32_t reductionType, uint32_t Radius,
	bool (*reject)(int32_t x, int32_t y),
	bool (*reduce)(object curPt, int i, object[] pts),
	int32_t[] (*mutate)(int32_t x, int32_t y));

/** Get the collision flags at a given point in a given area.
 *
 * \ingroup globalFunctions
 *
 * \param nLevelId The level id of the area in question.
 *
 * \param nX The x coordinate of the point in question.
 *
 * \param nY The y coordinate of the point in question.
 *
 * \return The collision flags. For details see:
 * http://forums.d2botnet.org/viewtopic.php?f=18&t=1212 .
 */
unsigned short getCollision(uint32_t nLevelId, int32_t nX, int32_t nY);

/** Get the health points of the controlled unit's merc.
 *
 * \ingroup globalFunctions
 *
 * \return The health points.
 */
int getMercHP();

/** Get the cursor type from p_D2CLIENT_RegularCursorType.
 *
 * \ingroup globalFunctions
 *
 * \todo Determine what the return value means.
 *
 * \return The regular cursor type.
 */
int getCursorType();

/** Get the cursor type from p_D2CLIENT_RegularCursorType if nType != 1, from
 * p_D2CLIENT_ShopCursorType if nType == 1.
 *
 * \ingroup globalFunctions
 *
 * \param nType What type of cursor to get. 1 - ShopCursorType, other -
 * RegularCursorType.
 *
 * \todo Determine what the return value means.
 *
 * \return The cursor type.
 */
int getCursorType(int nType);

/** Get skill ID by name.
 *
 * \ingroup globalFunctions
 *
 * \param skillName The skill name.
 *
 * \return The skill ID.
 */
int getSkillByName(String skillName);

/** Get skill name by ID.
 *
 * \ingroup globalFunctions
 *
 * \param skillId The skill ID.
 *
 * \return The skill name.
 */
String getSkillById(int skillId);

/** Get the String in the current locale that corresponds to the given id.
 *
 * \ingroup globalFunctions
 *
 * \param localeId The id of the String.
 *
 * \return The String in the current locale.
 */
String getLocaleString(uint16_t localeId);

/** Get the width and height of the given text in the given font.
 *
 * \ingroup globalFunctions
 *
 * \param string The string to get the size of.
 *
 * \param font The font to use.
 *
 * \return The width and height of the text. 0 - width, 1 - height.
 */
int[] getTextSize(String string, int font);

/** Get the width and height of the given text in the given font.
 *
 * \ingroup globalFunctions
 *
 * \param string The string to get the size of.
 *
 * \param font The font to use.
 *
 * \param asObject False to return as an array.
 *
 * \return The width and height of the text. 0 - width, 1 - height.
 */
int[] getTextWidthHeight(String string, int font, bool asObject);

/** Get the width and height of the given text in the given font.
 *
 * \ingroup globalFunctions
 *
 * \param string The string to get the size of.
 *
 * \param font The font to use.
 *
 * \param asObject True to return as an object.
 *
 * \return The width and height of the text. Has properties .width and .height.
 */
Object getTextWidthHeight(String string, int font, bool asObject);

/** Get the priority of the current thread.
 *
 * \ingroup globalFunctions
 *
 * \return The priority.
 */
int getThreadPriority();

/** Get whether or not a UI flag is set.
 *
 * \ingroup globalFunctions
 *
 * \todo Get a reference for the UI IDs.
 *
 * \param nUIId The ID of the UI in question.
 *
 * \return Whether or not the flag is set.
 */
bool getUIFlag(int nUIId);

/** Get the TradeFlag or RecentTradeId.
 *
 * \ingroup globalFunctions
 *
 * \todo Determine what this means.
 *
 * \param nMode What to return. 0 - TradeFlag, 2 - RecentTradeId.
 *
 * \return The TradeFlag or RecentTradeId.
 */
int getTradeInfo(int nMode);

/** Get the RecentTradeName. Currently broken, returns null.
 *
 * \ingroup globalFunctions
 *
 * \todo Determine what these are.
 *
 * \param nMode What to return. 1 - RecentTradeName.
 *
 * \return The RecentTradeName.
 */
String getTradeInfo(int nMode);

/** Get whether the controlled unit has the given waypoint.
 *
 * \ingroup globalFunctions
 *
 * \param nWaypointId The area id of the waypoint in question.
 *
 * \return Whether or not the controlled unit has the waypoint.
 */
bool getWaypoint(int nWaypointId);

/** Get the current or first context.
 *
 * \ingroup globalFunctions
 *
 * \param currentScript Whether to get the current script (true) or the first
 * (false).
 *
 * \return A D2BSScript object for the request script.
 */
D2BSScript getScript(bool currentScript);

/** Get a script by thread id.
 *
 * \ingroup globalFunctions
 *
 * \param threadId The thread id of the script to get.
 *
 * \return A D2BSScript object for the script from the given thread.
 */
D2BSScript getScript(int threadId);

/** Get a script by filename.
 *
 * \ingroup globalFunctions
 *
 * \param name The filename of the script to find.
 *
 * \return A D2BSScript object for the script.
 */
D2BSScript getScript(String name);

/** Get the first script.
 *
 * \ingroup globalFunctions
 *
 * \return A D2BSScript representing the first script in the runtime.
 */
D2BSScript getScript();

/** Get the first room in area given by level id.
 *
 * \ingroup globalFunctions
 *
 * \param levelId The id of the area to get the room from, or 0 to get the room
 * the controlled unit is in.
 *
 * \return The first room in the area.
 */
Room getRoom(uint32_t levelId);

/** Get the room that the given point is in from the given level id.
 *
 * \ingroup globalFunctions
 *
 * \param levelId The level id to look in.
 *
 * \param x The x coordinate to find a room for.
 *
 * \param y The y coordinate to find a room for.
 *
 * \return The room at the given point.
 */
Room getRoom(uint32_t levelId, int x, int y);

/** Get the room that the given point is in.
 *
 * \ingroup globalFunctions
 *
 * \param x The x coordinate to find a room for.
 *
 * \param y The y coordinate to find a room for.
 *
 * \return The room at the given point.
 */
Room getRoom(int x, int y);

/** Get the first room in the current area.
 *
 * \ingroup globalFunctions
 *
 * \return The first room in the current area.
 */
Room getRoom();

/** Get the first party.
 *
 * \ingroup globalFunctions
 *
 * \return The first party.
 */
Party getParty();

/** Get the first PresetUnit in the given level.
 *
 * \ingroup globalFunctions
 *
 * \param levelId The area id of the level to look for a PresetUnit in.
 *
 * \return The first PresetUnit found.
 */
PresetUnit getPresetUnit(uint32_t levelId);

/** Get the first PresetUnit of the given type.
 *
 * \ingroup globalFunctions
 *
 * \param levelId The area id of the level to look for a PresetUnit in.
 *
 * \param nType The type of the PresetUnit. See getUnit for type codes.
 *
 * \return The first PresetUnit found of type nType.
 */
PresetUnit getPresetUnit(uint32_t levelId, int nType);

/** Get the first PresetUnit of the given type and class id.
 *
 * \ingroup globalFunctions
 *
 * \param levelId The area id of the level to look for a PresetUnit in.
 *
 * \param nType The type of the PresetUnit. See getUnit for type codes.
 *
 * \param nClassId The class id to find. See:
 * http://forums.d2botnet.org/viewtopic.php?f=18&t=986 and
 * http://forums.d2botnet.org/viewtopic.php?f=18&t=985 .
 *
 * \return The first PresetUnit found of type nType and class ID nClassId.
 */
PresetUnit getPresetUnit(uint32_t levelId, int nType, int nClassId);

/** Get an array of all the PresetUnit s in the given level.
 *
 * \ingroup globalFunctions
 *
 * \param levelId The area id of the level to look for PresetUnit s in.
 *
 * \return An array of all the PresetUnit s on the given level.
 */
PresetUnit[] getPresetUnits(uint32_t levelId);

/** Get an array of all the PresetUnit s of the given type.
 *
 * \ingroup globalFunctions
 *
 * \param levelId The area id of the level to look for PresetUnit s in.
 *
 * \param nType The type of the PresetUnit. See getUnit for type codes.
 *
 * \return An array of all PresetUnit s found of type nType.
 */
PresetUnit[] getPresetUnits(uint32_t levelId, int nType);

/** Get an array of PresetUnit s of the given type and class id.
 *
 * \ingroup globalFunctions
 *
 * \param levelId The area id of the level to look for PresetUnit s in.
 *
 * \param nType The type of the PresetUnit. See getUnit for type codes.
 *
 * \param nClassId The class id to find. See:
 * http://forums.d2botnet.org/viewtopic.php?f=18&t=986 and
 * http://forums.d2botnet.org/viewtopic.php?f=18&t=985 .
 *
 * \return An array of PresetUnit s found of type nType and class ID nClassId.
 */
PresetUnit[] getPresetUnits(uint32_t levelId, int nType, int nClassId);

/** Get the Area where the controlled unit currently resides.
 *
 * \ingroup globalFunctions
 *
 * \return An Area object for the current object.
 */
Area getArea();

/** Get the Area for area ID nArea.
 *
 * \ingroup globalFunctions
 *
 * \param nArea The area ID.
 *
 * \return An Area object.
 */
Area getArea(int32_t nArea);

/** Get the base stat from the given table with the given class ID and stat
 * name.
 *
 * \ingroup globalFunctions
 *
 * \param szTableName The name of the table to look in.
 *
 * \param nClassId The class ID of the ...
 *
 * \param szStatName The stat name.
 *
 * \todo Determine what these tables are.
 *
 * \return The stat.
 */
String getBaseStat(String szTableName, int32_t nClassId, String szStatName);

/** Get the base stat from the given table with the given class ID and stat
 * name.
 *
 * \ingroup globalFunctions
 *
 * \param nBaseStat The stat to look up.
 *
 * \param nClassId The class ID of the ...
 *
 * \param szStatName The stat name.
 *
 * \todo Determine what these tables are.
 *
 * \return The stat.
 */
String getBaseStat(int32_t nBaseStat, int32_t nClassId, String szStatName);

/** Get the base stat from the given table with the given class ID and stat
 * ID.
 *
 * \ingroup globalFunctions
 *
 * \param szTableName The name of the table to look in.
 *
 * \param nClassId The class ID of the ...
 *
 * \param nStat The stat ID.
 *
 * \todo Determine what these tables are.
 *
 * \return The stat.
 */
String getBaseStat(String szTableName, int32_t nClassId, int32_t nStat);

/** Get the base stat from the given table with the given class ID and stat
 * ID.
 *
 * \ingroup globalFunctions
 *
 * \param nBaseStat The stat to look up.
 *
 * \param nClassId The class ID of the ...
 *
 * \param nStat The stat ID.
 *
 * \todo Determine what these tables are.
 *
 * \return The stat.
 */
String getBaseStat(int32_t nBaseStat, int32_t nClassId, int32_t nStat);

/** Get the base stat from the given table with the given class ID and stat
 * name.
 *
 * \ingroup globalFunctions
 *
 * \param szTableName The name of the table to look in.
 *
 * \param nClassId The class ID of the ...
 *
 * \param szStatName The stat name.
 *
 * \todo Determine what these tables are.
 *
 * \return The stat.
 */
int getBaseStat(String szTableName, int32_t nClassId, String szStatName);

/** Get the base stat from the given table with the given class ID and stat
 * name.
 *
 * \ingroup globalFunctions
 *
 * \param nBaseStat The stat to look up.
 *
 * \param nClassId The class ID of the ...
 *
 * \param szStatName The stat name.
 *
 * \todo Determine what these tables are.
 *
 * \return The stat.
 */
int getBaseStat(int32_t nBaseStat, int32_t nClassId, String szStatName);

/** Get the base stat from the given table with the given class ID and stat
 * ID.
 *
 * \ingroup globalFunctions
 *
 * \param szTableName The name of the table to look in.
 *
 * \param nClassId The class ID of the ...
 *
 * \param nStat The stat ID.
 *
 * \todo Determine what these tables are.
 *
 * \return The stat.
 */
int getBaseStat(String szTableName, int32_t nClassId, int32_t nStat);

/** Get the base stat from the given table with the given class ID and stat
 * ID.
 *
 * \ingroup globalFunctions
 *
 * \param nBaseStat The stat to look up.
 *
 * \param nClassId The class ID of the ...
 *
 * \param nStat The stat ID.
 *
 * \todo Determine what these tables are.
 *
 * \return The stat.
 */
int getBaseStat(int32_t nBaseStat, int32_t nClassId, int32_t nStat);

/** Get the base stat from the given table with the given class ID and stat
 * name.
 *
 * \ingroup globalFunctions
 *
 * \param szTableName The name of the table to look in.
 *
 * \param nClassId The class ID of the ...
 *
 * \param szStatName The stat name.
 *
 * \todo Determine what these tables are.
 *
 * \return The stat.
 */
bool getBaseStat(String szTableName, int32_t nClassId, String szStatName);

/** Get the base stat from the given table with the given class ID and stat
 * name.
 *
 * \ingroup globalFunctions
 *
 * \param nBaseStat The stat to look up.
 *
 * \param nClassId The class ID of the ...
 *
 * \param szStatName The stat name.
 *
 * \todo Determine what these tables are.
 *
 * \return The stat.
 */
bool getBaseStat(int32_t nBaseStat, int32_t nClassId, String szStatName);

/** Get the base stat from the given table with the given class ID and stat
 * ID.
 *
 * \ingroup globalFunctions
 *
 * \param szTableName The name of the table to look in.
 *
 * \param nClassId The class ID of the ...
 *
 * \param nStat The stat ID.
 *
 * \todo Determine what these tables are.
 *
 * \return The stat.
 */
bool getBaseStat(String szTableName, int32_t nClassId, int32_t nStat);

/** Get the base stat from the given table with the given class ID and stat
 * ID.
 *
 * \ingroup globalFunctions
 *
 * \param nBaseStat The stat to look up.
 *
 * \param nClassId The class ID of the ...
 *
 * \param nStat The stat ID.
 *
 * \todo Determine what these tables are.
 *
 * \return The stat.
 */
bool getBaseStat(int32_t nBaseStat, int32_t nClassId, int32_t nStat);

/** Get the base stat from the given table with the given class ID and stat
 * name.
 *
 * \ingroup globalFunctions
 *
 * \param szTableName The name of the table to look in.
 *
 * \param nClassId The class ID of the ...
 *
 * \param szStatName The stat name.
 *
 * \todo Determine what these tables are.
 *
 * \return The stat.
 */
double getBaseStat(String szTableName, int32_t nClassId, String szStatName);

/** Get the base stat from the given table with the given class ID and stat
 * name.
 *
 * \ingroup globalFunctions
 *
 * \param nBaseStat The stat to look up.
 *
 * \param nClassId The class ID of the ...
 *
 * \param szStatName The stat name.
 *
 * \todo Determine what these tables are.
 *
 * \return The stat.
 */
double getBaseStat(int32_t nBaseStat, int32_t nClassId, String szStatName);

/** Get the base stat from the given table with the given class ID and stat
 * ID.
 *
 * \ingroup globalFunctions
 *
 * \param szTableName The name of the table to look in.
 *
 * \param nClassId The class ID of the ...
 *
 * \param nStat The stat ID.
 *
 * \todo Determine what these tables are.
 *
 * \return The stat.
 */
double getBaseStat(String szTableName, int32_t nClassId, int32_t nStat);

/** Get the base stat from the given table with the given class ID and stat
 * ID.
 *
 * \ingroup globalFunctions
 *
 * \param nBaseStat The stat to look up.
 *
 * \param nClassId The class ID of the ...
 *
 * \param nStat The stat ID.
 *
 * \todo Determine what these tables are.
 *
 * \return The stat.
 */
double getBaseStat(int32_t nBaseStat, int32_t nClassId, int32_t nStat);

/** Get the control specified by type.
 *
 * \ingroup globalFunctions
 *
 * \param nType The type of control (button, text box, etc.)
 *
 * \todo Get a reference for nType
 *
 * \return A Control object representing the Control found.
 */
Control getControl(int32_t nType);

/** Get the control specified by type, x location.
 *
 * \ingroup globalFunctions
 *
 * \param nType The type of control (button, text box, etc.)
 *
 * \param nX The x coordinate of the control of the control to find.
 *
 * \todo Get a reference for nType
 *
 * \return A Control object representing the Control found.
 */
Control getControl(int32_t nType, int32_t nX);

/** Get the control specified by type and location.
 *
 * \ingroup globalFunctions
 *
 * \param nType The type of control (button, text box, etc.)
 *
 * \param nX The x coordinate of the control of the control to find.
 *
 * \param nY The y coordinate of the control of the control to find.
 *
 * \todo Get a reference for nType
 *
 * \return A Control object representing the Control found.
 */
Control getControl(int32_t nType, int32_t nX, int32_t nY);

/** Get the control specified by type, location and width.
 *
 * \ingroup globalFunctions
 *
 * \param nType The type of control (button, text box, etc.)
 *
 * \param nX The x coordinate of the control of the control to find.
 *
 * \param nY The y coordinate of the control of the control to find.
 *
 * \param nXSize The width of the control.
 *
 * \todo Get a reference for nType
 *
 * \return A Control object representing the Control found.
 */
Control getControl(int32_t nType, int32_t nX, int32_t nY, int32_t nXSize);

/** Get the control specified by type, location and size.
 *
 * \ingroup globalFunctions
 *
 * \param nType The type of control (button, text box, etc.)
 *
 * \param nX The x coordinate of the control of the control to find.
 *
 * \param nY The y coordinate of the control of the control to find.
 *
 * \param nXSize The width of the control.
 *
 * \param nYSize The height of the control.
 *
 * \todo Get a reference for nType
 *
 * \return A Control object representing the Control found.
 */
Control getControl(int32_t nType, int32_t nX, int32_t nY, int32_t nXSize,
	int32_t nYSize);

/** Get the relation between two units.
 *
 * \ingroup globalFunctions
 *
 * \param nFirstUnitId The unit id for the unit making the relationship.
 *
 * \param nSecondUnitId The unit id for the unit receiving the relationship.
 *
 * \param nFlag The relationship.
 *
 * \todo Determine what the flags are.
 *
 * \todo Should this return bool instead of int? (i.e. change the d2bs code).
 *
 * \return Whether or not the relationship is of the type specified.
 */
bool getPlayerFlag(int nFirstUnitId, int nSecondUnitId, int nFlag);

/** Get the tick count. Returns GetTickCount().
 *
 * \ingroup globalFunctions
 *
 * \return The tick count.
 */
int getTickCount();

/** Get the Unit that the controlled Unit is currently interacting with.
 *
 * \ingroup globalFunctions
 *
 * \return The Unit currently being interacted with.
 */
Unit getInteractedNPC();

/** Print the String representation of the arguments to the console.
 *
 * \ingroup globalFunctions
 */
void print(...);

/** Delay for nDelay milliseconds. This is done by suspending the context,
 * sleeping, then resuming the context for short (<50 ms) delays. For longer
 * delays the context is suspended, 50ms are slept away, the context is resumed,
 * sequence repeats until time slept is greater then nDelay ms.
 *
 * \ingroup globalFunctions
 *
 * \param nDelay Milliseconds to sleep.
 */
void delay(uint32_t nDelay);

/** Load file and create a thread with it's main function. File taken from
 * scriptDir\\file. All remaining parameters are passed to the script.
 *
 * \ingroup globalFunctions
 *
 * \param file The filename of the script.
 *
 * \return If successful returns the new script object representing the thread
 * of the thread. Otherwise it returns null.
 */
D2BSScript load(String file, ...);

/** Determine whether a file has been included yet or not. File taken from
 * scriptDir\\libs\\file
 *
 * \ingroup globalFunctions
 *
 * \param file Filename of the file that might have been included.
 *
 * \return Whether or not the file has been included.
 */
bool isIncluded(String file);

/** Include file from the scriptDir\\libs directory.
 *
 * \ingroup globalFunctions
 *
 * \param file Filename of the file to include.
 *
 * \return Whether the inclusion was successful.
 */
bool include(String file);

/** Stop all scripts.
 *
 * \ingroup globalFunctions
 */
void stop();

/** Conditionally stop current script.
 *
 * \ingroup globalFunctions
 *
 * \param stop 1 if current script should be stopped.
 */
void stop(int stop);

/** Conditionnaly stop current script.
 *
 * \ingroup globalFunctions
 *
 * \param stop True if current script should be stopped.
 */
void stop(bool stop);

/** Return a random number between low and high. Uses C rand() when out of game,
 * and D2GAME_D2Rand with the game seed while in game.
 *
 * \ingroup globalFunctions
 *
 * \param low The lowest possible integer to return.
 *
 * \param high The highest possible integer to return.
 *
 * \return A random integer in the range [low,high].
 */
int rand(int32_t low, int32_t high);

/** Send data to another window by means of the WM_COPYDATA message. Uses
 * FindWindow to get the HWND and SendMessage to send the message.
 *
 * \ingroup globalFunctions
 *
 * \param windowClassName The class name of the receiving window.
 *
 * \param windowName The window name of the receiving window.
 *
 * \param nModeId Data to be sent as the dwData member of the copy data.
 *
 * \param data Data to be send as the lpData member of the copy data.
 *
 * \return The result of SendMessage.
 */
int sendCopyData(String windowClassName, String windowName, int32_t nModeId,
	String data);

/** Send data to a DDE server by means of DdeClientTransaction.
 *
 * \ingroup globalFunctions
 *
 * \param mode The mode of transaction to use.
 *
 * 0 - Do command given by pszItem with an XTYP_REQUEST and get the resulting
 * data.
 *
 * 1 - Do command given by pszItem with an XTYP_POKE sending ""
 *
 * 2 - Do command with an XTYP_EXECUTE sending ""
 *
 * \param pszDDEServer The service name of the DDE server to connect to.
 *
 * \param pszTopic The topic with which to start the connection.
 *
 * \param pszItem Data to be sent.
 *
 * \return Result data if mode was 0.
 */
String sendDDE(int32_t mode, String pszDDEServer, String pszTopic,
	String pszItem);

/** Get whether or not a key is pressed. Returns the value of GetAsyncKeyState.
 *
 * \ingroup globalFunctions
 *
 * \param vKey The virtual key code for the key in question. See:
 * http://msdn.microsoft.com/en-us/library/dd375731%28v=VS.85%29.aspx
 *
 * \return Whether not the key is pressed.
 */
bool keystate(int vKey);

/** Add an event handler to the list of listeners for the given event.
 *
 * \ingroup globalFunctions
 *
 * \param event The name of the event. Can be:
 *
 * melife (uint32_t dwLife)
 *
 * memana (uint32_t dwMana)
 *
 * keyup (uint32_t key)
 *
 * keydown (uint32_t key)
 *
 * mouseclick (uint32_t button, bool bUp, uint32_t x, uint32_t y)
 *
 * mousemove (uint32_t x, uint32_t y)
 *
 * scriptmsg (...)
 *
 * golddrop (uint32_t gId, uint32_t Mode)
 *
 * chatmsg (String lpszNick, String lpszMsg)
 *
 * whispermsg (String lpszNick, String lpszMsg)
 *
 * copydata (int32_t dwMode, String lpszMsg)
 *
 * gamemsg (String lpszMsg)
 *
 * itemaction (uint32_t gId, uint32_t mode, String code, Bool global)
 *
 * gameevent (BYTE mode, DWORD param1, DWORD param2, String name1, String name2)
 *
 * From http://www.blizzhackers.cc/viewtopic.php?t=392307 Dark_Mage-
 *
 * mode:
 *
 * 0x00 - "%Name1(%Name2) dropped due to time out."
 * 0x01 - "%Name1(%Name2) dropped due to errors."
 * 0x02 - "%Name1(%Name2) joined our world. Diablo's minions grow stronger."
 * 0x03 - "%Name1(%Name2) left our world. Diablo's minions weaken."
 * 0x04 - "%Name1 is not in the game."
 * 0x05 - "%Name1 is not logged in."
 * 
 * 0x06 - "%Name1 was Slain by %Name2"
 *    BYTE Param2 = Unit Type of Slayer (0x00 = Player, 0x01 = NPC)
 *        if Type = Player, %Name2 = Slayer Character Name & DWORD Param1 = Slayer Character Type
 *        if Type = NPC, DWORD Param1 = Monster Id Code from MPQ (points to string for %Name2)
 *        if Type = NPC & Monster is Unique, %Name2 = Unique Monster Id
 * 
 * 0x07 - Player Relations (Bottom Left Text)
 *    DWORD Param1 = Player Id
 *    Player Id = Pointer to Character Name
 *    BYTE Param2 = Action Taken
 *       Actions:
 *       0x01 - "%Player permits you to loot his corpse."
 *       0x02 - "%Player permits you to loot her corpse."
 *       0x03 - "%Player has declared hostility towards you."
 *       0x04 - "%Player is no longer hostile towards you."
 *       0x05 - "%Player invites you to ally against the forces of evil."
 *       0x06 - "%Player has canceled party invite."
 *       0x07 - "%Player has joined your party to fight the forces of evil."
 *       0x08 - "You are now allied with %Player."
 *       0x09 - "%Player has left your party."
 *       0x0a - "%Player no longer allows you to access his corpse."
 *       0x0b - "%Player no longer allows you to access her corpse."
 * 
 * 0x08 - "%Name1 is busy."
 * 0x09 - "You must wait a short time to trade with that player."
 * 
 * 0x0a - "%Name1 has items in his box."
 *    if %Name1 = 0x00, "You have items in your box."
 * 
 * 0x0b - <Unknown>
 * 0x0c - <Unknown>
 * 0x0d - "%Name1 is not listening to you."
 * 0x0e - Received on 'Not enough mana' speech.
 * 0x0f - "Realm going down in %Param1 minutes."
 * 0x10 - "You must wait a short time to declare hostility with that player."
 * 0x11 - "%Param1 Stones of Jordan Sold to Merchants"
 * 0x12 - "Diablo Walks the Earth"
 *
 * \param eventHandler A function with the signature listed above that handles
 * the event.
 */
void addEventListener(String event, Function eventHandler);

/** Removes a previously added event from the listener list.
 *
 * \ingroup globalFunctions
 *
 * \param event The event name (the one used to add the event).
 *
 * \param eventHandler The handler to remove.
 */
void removeEventListener(String event, Function eventHandler);

/** Clear the event handler list for given event.
 *
 * \ingroup globalFunctions
 *
 * \param event The name of the event to clear the handler list for.
 */
void clearEvent(String event);

/** Clear all event handlers (from all events).
 *
 * \ingroup globalFunctions
 */
void clearAllEvents();

/** Get whether or not the JSOPTION_STRICT flag is set.
 *
 * \ingroup globalFunctions
 *
 * \return Whether or not the strict flag is set.
 */
bool js_strict();

/** Set or clear the JSOPTION_STRICT flag.
 *
 * \ingroup globalFunctions
 *
 * \param setStrictFlag The new JSOPTION_STRICT state.
 */
void js_strict(bool setStrictFlag);

/** Get the D2BS version string.
 *
 * \ingroup globalFunctions
 *
 * \return The D2BS version string.
 */
String version();

/** Print the D2BS version string.
 *
 * \ingroup globalFunctions
 */
void version(int a);

/** Broadcast a message to all other scripts. D2BS calls each scripts scriptmsg
 * event listeners with the messages passed in.
 *
 * \ingroup globalFunctions
 */
void scriptBroadcast(...);

/** Get the sqlite version string (from sqlite3_version).
 *
 * \ingroup globalFunctions
 *
 * \return The sqlite version string.
 */
String sqlite_version();

/** Return the sqlite memory usage.
 *
 * \ingroup globalFunctions
 */
double sqlite_memusage();

/** Get the Folder object for the directory scriptPath\\name
 *
 * \ingroup globalFunctions
 *
 * \param name The name of the Folder to open.
 *
 * \return A Folder object representing the Folder name.
 */
Folder dopen(String name);

/** Log the String representation of the arguments.
 *
 * \ingroup globalFunctions
 */
void debugLog(...);

/** Show the console.
 *
 * \ingroup globalFunctions
 */
void showConsole();

/** Hide the console.
 *
 * \ingroup globalFunctions
 */
void hideConsole();

/** Loads the default (either from config or copydata or DDE) profile from
 * d2bs.ini and gets to the lobby (for battle.net chars) or into a game (for
 * single player characters).
 *
 * \ingroup globalFunctions
 *
 * \throw String May throw one of the following:
 *
 * "invalid character name" - If it fails to select the character.
 *
 * "Failed to click the Single button?" - If it fails to click the single player
 * button.
 *
 * "Failed to click the 'Battle.net' button?" - If it fails to click the
 * battle.net button.
 *
 * "Failed to click the 'Other Multiplayer' button?" - If it fails to click the
 * Other Multiplayer button.
 *
 * "Failed to click the 'Open Battle.net' button?" - If it fails to click the
 * Open battle.net button.
 *
 * "Failed to click the 'TCP/IP' button?" - If it fails to click the TCP/IP
 * button.
 *
 * "Failed to click the 'Host Game' button?" - If it fails to click the Host
 * Game (TCP/IP) button.
 *
 * "Failed to click the 'Join Game' button?" - If it fails to click the Join
 * Game (TCP/IP) button.
 *
 * "Failed to click the OK button" - If it fails to click the OK button (TCP/IP
 * enter IP address).
 *
 * "Failed to find the 'Host IP Address' text-edit box." - If it can't find the
 * host IP address (TCP/IP) text box.
 *
 * "Could not get the IP address from the profile in the d2bs.ini file." - If
 * the user failed to specify the IP address in the d2bs.ini file.
 *
 * "Failed to click the exit button?" - If it is login and fails to click the
 * exit button.
 *
 * "Failed to set the 'Username' text-edit box.' - If it can't find the username
 * control.
 *
 * "Failed to set the 'Password' text-edit box." - If it can't find the password
 * control.
 *
 * "Failed to click the 'Log in' button?" - If it can't click the login button.
 *
 * "Failed to click the 'Normal Difficulty' button?" - If it can't click the
 * normal button in single player game creation.
 *
 * "Failed to click the 'Nightmare Difficulty' button?" - If it can't click the
 * nightmare button in single player game creation.
 *
 * "Failed to click the 'Hell Difficulty' button?" - If it can't click the hell
 * button in single player game creation.
 *
 * "Invalid single player difficulty level specified!" - If the difficulty level
 * is invalid.
 *
 * "Unable to connect" - If the unable to connect screen is shown.
 *
 * "CD-Key in use" - If the cdkey in use screen is shown.
 *
 * "Bad account or password" - If the username/password wrong screen is shown.
 *
 * "Realm Down" - If the realm down message is shown.
 *
 * "Unhandled login location" - If the game is in an unsupported location.
 */
void login();

/** Loads the profile from d2bs.ini and gets to the lobby (for battle.net chars)
 * or into a game (for single player characters).
 *
 * \ingroup globalFunctions
 *
 * \param profile The profile to load.
 *
 * \throw String May throw one of the following:
 *
 * "invalid character name" - If it fails to select the character.
 *
 * "Failed to click the Single button?" - If it fails to click the single player
 * button.
 *
 * "Failed to click the 'Battle.net' button?" - If it fails to click the
 * battle.net button.
 *
 * "Failed to click the 'Other Multiplayer' button?" - If it fails to click the
 * Other Multiplayer button.
 *
 * "Failed to click the 'Open Battle.net' button?" - If it fails to click the
 * Open battle.net button.
 *
 * "Failed to click the exit button?" - If it is login and fails to click the
 * exit button.
 *
 * "Failed to set the 'Username' text-edit box.' - If it can't find the username
 * control.
 *
 * "Failed to set the 'Password' text-edit box." - If it can't find the password
 * control.
 *
 * "Failed to click the 'Log in' button?" - If it can't click the login button.
 *
 * "Failed to click the 'Normal Difficulty' button?" - If it can't click the
 * normal button in single player game creation.
 *
 * "Failed to click the 'Nightmare Difficulty' button?" - If it can't click the
 * nightmare button in single player game creation.
 *
 * "Failed to click the 'Hell Difficulty' button?" - If it can't click the hell
 * button in single player game creation.
 *
 * "Invalid single player difficulty level specified!" - If the difficulty level
 * is invalid.
 *
 * "Unable to connect" - If the unable to connect screen is shown.
 *
 * "CD-Key in use" - If the cdkey in use screen is shown.
 *
 * "Bad account or password" - If the username/password wrong screen is shown.
 *
 * "Realm Down" - If the realm down message is shown.
 *
 * "Unhandled login location" - If the game is in an unsupported location.
 */
void login(String profile);

/** Select the character from the given profile. Keeps all details except the
 * character name from old profile.
 *
 * \ingroup globalFunctions
 *
 * \param profile The profile to get the character name from.
 *
 * \return Whether or not character swap was successful.
 */
bool selectCharacter(String profile);

/** Create a game with the given name.
 *
 * \ingroup globalFunctions
 *
 * \param name The game name
 */
void createGame(String name);

/** Create a game with the given name and password.
 *
 * \ingroup globalFunctions
 *
 * \param name The game name
 *
 * \param pass The game password
 */
void createGame(String name, String pass);

/** Create a game with the given name and password in the given difficulty.
 *
 * \ingroup globalFunctions
 *
 * \param name The game name.
 *
 * \param pass The game password.
 *
 * \param diff The game difficulty. 0 - normal, 1 - nightmare, 2 - hell, 3 - 
 * hardest difficulty available.
 */
void createGame(String name, String pass, int32_t diff);

/** Join a game with the given name.
 *
 * \ingroup globalFunctions
 *
 * \param name The game name.
 */
void joinGame(String name);

/** Join a game with the given name and password.
 *
 * \ingroup globalFunctions
 *
 * \param name The game name.
 *
 * \param pass The game password.
 */
void joinGame(String name, String pass);

/** Create a profile with the given profile name, mode, gateway, username,
 * password and character name.
 *
 * \ingroup globalFunctions
 *
 * \param profile The profile name.
 *
 * \param mode The mode (single player/battle.net/etc).
 *
 * \param gateway The realm gateway.
 *
 * \param username Account username.
 *
 * \param password Account password.
 *
 * \param charname Character name (case sensitive).
 */
void addProfile(String profile, String mode, String gateway, String username,
	String password, String charname);

/** Create a profile with the given profile name, mode, gateway, username,
 * password, character name and single player difficulty.
 *
 * \ingroup globalFunctions
 *
 * \param profile The profile name.
 *
 * \param mode The mode (single player/battle.net/etc).
 *
 * \param gateway The realm gateway.
 *
 * \param username Account username.
 *
 * \param password Account password.
 *
 * \param charname Character name (case sensitive).
 *
 * \param spDifficulty The difficulty to create games for a single player
 * character.
 */
void addProfile(String profile, String mode, String gateway, String username,
	String password, String charname, int spDifficulty);

/** Get the current OOG location.
 *
 * \ingroup globalFunctions
 *
 * \todo Create a reference for the return values.
 *
 * \return The current OOG location.
 */
int getLocation();

/** Load an mpq file.
 *
 * \ingroup globalFunctions
 *
 * \param path The mpq file to load
 */
void loadMpq(String path);

/** Submit the item on the cursor to the open screen (like the add sockets
 * screen).
 *
 * \ingroup globalFunctions
 *
 * \return Whether or not it was successful.
 */
bool submitItem();

/** Get the mouse coordinates (in screen space) and return as an array.
 *
 * \ingroup globalFunctions
 *
 * \return The mouse coordinates in an array. 0 - x, 1 - y.
 */
int[] getMouseCoords();

/** Get the mouse coordinates (in screen or map space) and return as an array.
 *
 * \ingroup globalFunctions
 *
 * \param nFlag Whether to return in map space (true) or screen space (false).
 *
 * \return The mouse coordinates in an array. 0 - x, 1 - y.
 */
int[] getMouseCoords(bool nFlag);

/** Get the mouse coordinates (in screen or map space) and return as an array.
 *
 * \ingroup globalFunctions
 *
 * \param nFlag Whether to return in map space (true) or screen space (false).
 *
 * \param nReturn Whether to return as an array or as an Object, false for array
 * format.
 *
 * \return The mouse coordinates in an array. 0 - x, 1 - y.
 */
int[] getMouseCoords(bool nFlag, int nReturn);

/** Get the mouse coordinates (in screen or map space) and return as an Object.
 *
 * \ingroup globalFunctions
 *
 * \param nFlag Whether to return in map space (true) or screen space (false).
 *
 * \param nReturn Whether to return as an array or as an Object, true for Object
 * format.
 *
 * \return The mouse coordinates in an array. Has properties .x and .y.
 */
Object getMouseCoords(bool nFlag, int nReturn);

/** Copy the cached data associated with given unit. This is things like type
 * and unit id.
 *
 * \ingroup globalFunctions
 *
 * \todo Figure out why anyone would want to copy a unit.
 *
 * \return The new copy of the unit.
 */
Unit copyUnit(Unit other);

/** Click on the map with the given click type and possibly shift at the given
 * unit.
 *
 * \ingroup globalFunctions
 *
 * \param nClickType The click type to do.
 *
 * \param nShift Whether or not to be holding down shift. 0 - Don't hold shift,
 * 1 - hold shift.
 *
 * \param toClick The unit to click on.
 *
 * \todo Get a reference for nClickType.
 *
 * \return Whether or not the click was successful.
 */
bool clickMap(uint16_t nClickType, uint16_t nShift, Unit toClick);

/** Click on the map with the given click type and possibly shift at the given
 * unit.
 *
 * \ingroup globalFunctions
 *
 * \param nClickType The click type to do.
 *
 * \param nShift Whether or not to be holding down shift.
 *
 * \param toClick The unit to click on.
 *
 * \todo Get a reference for nClickType.
 *
 * \return Whether or not the click was successful.
 */
bool clickMap(uint16_t nClickType, bool nShift, Unit toClick);

/** Click on the map with the given click type and possibly shift at the given
 * point.
 *
 * \ingroup globalFunctions
 *
 * \param nClickType The click type to do.
 *
 * \param nShift Whether or not to be holding down shift. 0 - Don't hold shift,
 * 1 - hold shift.
 *
 * \param nX The x coordinate to click at.
 *
 * \param nY The y coordinate to click at.
 *
 * \todo Get a reference for nClickType.
 *
 * \return Whether or not the click was successful.
 */
bool clickMap(uint16_t nClickType, uint16_t nShift, uint16_t nX, uint16_t nY);

/** Click on the map with the given click type and possibly shift at the given
 * point.
 *
 * \ingroup globalFunctions
 *
 * \param nClickType The click type to do.
 *
 * \param nShift Whether or not to be holding down shift.
 *
 * \param nX The x coordinate to click at.
 *
 * \param nY The y coordinate to click at.
 *
 * \todo Get a reference for nClickType.
 *
 * \return Whether or not the click was successful.
 */
bool clickMap(uint16_t nClickType, bool nShift, uint16_t nX, uint16_t nY);

/** Get or do something to do with accepting trade.
 *
 * \ingroup globalFunctions
 *
 * \param action What action to perform.
 *
 * 1 - Get whether trade we've accepted trade already or not.
 *
 * 2 - Get the trade flag ...
 * 
 * 3 - Get whether the check is red or not.
 *
 * \return The flag specified by action.
 */
bool acceptTrade(int action);

/** Accept a trade if not already accepted, cancel a trade if already accepted.
 *
 * \ingroup globalFunctions
 *
 * \todo Verify that I understand this correctly.
 *
 * \return Whether or not trade was accepted.
 */
bool acceptTrade();

/** Make the windows default beep sound.
 *
 * \ingroup globalFunctions
 */
void beep();

/** Beep with beep ID nBeepId.
 *
 * \ingroup globalFunctions
 *
 * \param nBeepId ID of the beep to make. See:
 * http://msdn.microsoft.com/en-us/library/ms680356%28VS.85%29.aspx .
 */
void beep(init nBeepId);

/** Click an item.
 *
 * \ingroup globalFunctions
 *
 * \param item The item to click on.
 */
void clickItem(Unit item);

/** Click on a body location with a certain click type.
 *
 * \ingroup globalFunctions
 *
 * \todo Determine values for nClickType
 *
 * \param nClickType The click type. 4 for merc locations.
 *
 * \param nBodyLoc The body location.
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
void clickItem(int nClickType, int nBodyLoc);

/** Click on a given item with the given click type.
 *
 * \ingroup globalFunctions
 *
 * \todo Determine values for nClickType
 *
 * \param nClickType The click type. 4 for merc locations.
 *
 * \param item The item to click.
 *
 * \param nClickType
 */
void clickItem(int nClickType, Unit item);

/** Click an item location with the given click type.
 *
 * \param nClickType The click type. 4 for merc locations.
 *
 * \param nX The x coordinate of the location to click.
 *
 * \param nY The y coordinate of the location to click.
 *
 * \param nLoc The location of the item.
 *
 * 0 - Inventory
 *
 * 2 - Player trade
 *
 * 3 - Cube
 *
 * 4 - Stash
 *
 * 5 - Belt
 */
void clickItem(int nButton, nX, nY, nLoc);

/** Get the euclidean distance from me to a.
 *
 * \ingroup globalFunctions
 *
 * \param a The second Unit.
 *
 * \return The euclidean distance from a.
 */
double getDistance(Unit a);

/** Get the euclidean distance from a to b.
 *
 * \ingroup globalFunctions
 *
 * \param a The first Unit.
 *
 * \param b The second Unit.
 *
 * \return The euclidean distance from a to b.
 */
double getDistance(Unit a, Unit b);

/** Get the euclidean distance from a to b.
 *
 * \ingroup globalFunctions
 *
 * \param a The first Unit.
 *
 * \param bx The x coordinate of point b.
 *
 * \param by The y coordinate of point b.
 *
 * \return The euclidean distance from a to b.
 */
double getDistance(Unit a, int32_t bx, int32_t by);

/** Get the euclidean distance from me to a.
 *
 * \ingroup globalFunctions
 *
 * \param ax The x coordinate of point a.
 *
 * \param ay The y coordinate of point a.
 *
 * \return The euclidean distance from me to a.
 */
double getDistance(int32_t ax, int32_t ay);

/** Get the euclidean distance from a to b.
 *
 * \ingroup globalFunctions
 *
 * \param ax The x coordinate of point a.
 *
 * \param ay The y coordinate of point a.
 *
 * \param b The second Unit.
 *
 * \return The euclidean distance from a to b.
 */
double getDistance(int32_t ax, int32_t ay, Unit b);

/** Get the euclidean distance from a to b.
 *
 * \ingroup globalFunctions
 *
 * \param ax The x coordinate of point a.
 *
 * \param ay The y coordinate of point a.
 *
 * \param bx The x coordinate of point b.
 *
 * \param by The y coordinate of point b.
 *
 * \return The euclidean distance from a to b.
 */
double getDistance(int32_t ax, int32_t ay, int32_t bx, int32_t by);

/** Drop some gold.
 *
 * \ingroup globalFunctions
 *
 * \param nGold The amount of gold to drop.
 */
void gold(int nGold);

/** Do something with some gold.
 *
 * \ingroup globalFunctions
 *
 * \todo Determine what the values for nMode are.
 *
 * \param nGold The amount of gold to move around.
 *
 * \param nMode What to do with the gold.
 */
void gold(int nGold, int nMode);

/** Check if two units collide.
 *
 * \ingroup globalFunctions
 *
 * \param a Unit a.
 *
 * \param b Unit b.
 *
 * \param nBitMask Bit mask to check collision of a and b with.
 */
int checkCollision(Unit a, Unit b, int nBitMask);

/** Play d2 sound by id.
 *
 * \ingroup globalFunctions
 *
 * \return true
 */
bool playSound(int nSoundId);

/** Quit the game.
 *
 * \ingroup globalFunctions
 */
void quit();

/** Quit Diablo II. Allows the core to shutdown, than terminates the process.
 *
 * \ingroup globalFunctions
 */
void quitGame();

/** Say the string equivalent of the each of the arguments.
 *
 * \ingroup globalFunctions
 *
 * \return true if at least one parameter was sent in.
 */
bool say(...);

/** Click one of the buttons in the party screen for the given player.
 *
 * \ingroup globalFunctions
 *
 * \param player The player who's button you wish to click.
 *
 * \param nMode Which button to click.
 *
 * 0 - Allow loot corpse
 *
 * 1 - Hostile player
 *
 * 2 - Join party
 *
 * 3 - Leave party (doesn't matter which party is passed in)
 */
bool clickParty(Party player, int nMode);

/** Switch the weapons.
 *
 * \ingroup globalFunctions
 *
 * \return true
 */
bool weaponSwitch();

/** Get which weapon switch is being used.
 *
 * \ingroup globalFunctions
 *
 * \param dummy Any int other than 0.
 *
 * \return Which weapon switch is active.
 */
int weaponSwitch(int32_t dummy);

/** Switch the weapons.
 *
 * \ingroup globalFunctions
 *
 * \param dummy 0 to use this mode.
 *
 * \return true
 */
bool weaponSwitch(int32_t dummy);

/** Hit the transmute button.
 *
 * \ingroup globalFunctions
 */
void transmute();

/** Use a stat point.
 *
 * BE CAREFUL! This function directly sends packets without checks. If you
 * call this function and do not have the points, or specify an invalid stat,
 * you might get flagged/banned.
 *
 * \ingroup globalFunctions
 *
 * \todo Come up with a reference for the statType values
 *
 * \param statType The type of stat to add the point to
 */
void useStatPoint(uint16_t statType);

/** Use a stat point.
 *
 * BE CAREFUL! This function directly sends packets without checks. If you
 * call this function and do not have the points, or specify an invalid stat,
 * you might get flagged/banned.
 *
 * \ingroup globalFunctions
 *
 * \todo Come up with a reference for the statType values
 *
 * \param statType The type of stat to add the points to
 *
 * \param count The number of points to add
 */
void useStatPoint(uint16_t statType, uint32_t count);

/** Use a skill point
 *
 * BE CAREFUL! This function directly sends packets without checks. If you
 * call this function and do not have the points, or specify an invalid skill,
 * you might get flagged/banned.
 *
 * \ingroup globalFunctions
 *
 * \todo Come up with a reference for the skill values
 *
 * \param skill The skill to add the point to
 */
void useSkillPoint(uint16_t skill);

/** Use a skill point
 *
 * BE CAREFUL! This function directly sends packets without checks. If you
 * call this function and do not have the points, or specify an invalid skill,
 * you might get flagged/banned.
 *
 * \ingroup globalFunctions
 *
 * \todo Come up with a reference for the skill values
 *
 * \param skill The skill to add the points to
 *
 * \param count The number points to spend
 */
void useSkillPoint(uint16_t skill, uint32_t count);

/** Take a screenshot
 *
 * Performs the action that pressing print screen (by default) would do
 *
 * \ingroup globalFunctions
 */
void takeScreenshot();

/** Convert a point from screen coordinates to automap coordinates.
 *
 * \ingroup globalFunctions
 *
 * \param arg An object with x and y parameters specifying the point.
 *
 * \return An object with x and y, containing the automap coordinate.
 */
Object screenToAutomap(Object arg);

/** Convert a point from screen coordinates to automap coordinates.
 *
 * \ingroup globalFunctions
 *
 * \param ix The x coordinate.
 *
 * \param iy The y coordinate.
 *
 * \return An object with x and y, containing the automap coordinate.
 */
Object screenToAutomap(int ix, int iy);

/** Convert a point from automap coordinates to screen coordinates.
 *
 * \ingroup globalFunctions
 *
 * \param arg An object with x and y parameters specifying the point.
 *
 * \return An object with x and y, containing the screen coordinate.
 */
Object automapToScreen(Object arg);

/** Convert a point from automap coordinates to screen coordinates.
 *
 * \ingroup globalFunctions
 *
 * \param ix The x coordinate.
 *
 * \param iy The y coordinate.
 *
 * \return An object with x and y, containing the screen coordinate.
 */
Object automapToScreen(int ix, int iy);

/** Takes the md5 hash.
 *
 * \ingroup globalFunctions
 *
 * \param str The string to hash.
 *
 * \return The md5 hash of the string.
 */
String md5(String str);

/** Takes the sha1 hash.
 *
 * \ingroup globalFunctions
 *
 * \param str The string to hash.
 *
 * \return The sha1 hash of the string.
 */
String sha1(String str);

/** Takes the sha256 hash.
 *
 * \ingroup globalFunctions
 *
 * \param str The string to hash.
 *
 * \return The sha256 hash of the string.
 */
String sha256(String str);

/** Takes the sha384 hash.
 *
 * \ingroup globalFunctions
 *
 * \param str The string to hash.
 *
 * \return The sha384 hash of the string.
 */
String sha384(String str);

/** Takes the sha512 hash.
 *
 * \ingroup globalFunctions
 *
 * \param str The string to hash.
 *
 * \return The sha512 hash of the string.
 */
String sha512(String str);

/** Take the md5 hash of a file.
 *
 * \ingroup globalFunctions
 *
 * \param file Filename relative to script path.
 *
 * \return The md5 hash of the file.
 */
String md5_file(String file);

/** Take the sha1 hash of a file.
 *
 * \ingroup globalFunctions
 *
 * \param file Filename relative to script path.
 *
 * \return The sha1 hash of the file.
 */
String sha1_file(String file);

/** Take the sha256 hash of a file.
 *
 * \ingroup globalFunctions
 *
 * \param file Filename relative to script path.
 *
 * \return The sha256 hash of the file.
 */
String sha256_file(String file);

/** Take the sha384 hash of a file.
 *
 * \ingroup globalFunctions
 *
 * \param file Filename relative to script path.
 *
 * \return The sha384 hash of the file.
 */
String sha384_file(String file);

/** Take the sha512 hash of a file.
 *
 * \ingroup globalFunctions
 *
 * \param file Filename relative to script path.
 *
 * \return The sha512 hash of the file.
 */
String sha512_file(String file);
