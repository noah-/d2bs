/** Represents a "Room" in the game. A Room is a rectangular piece of the map
 * data. It has pointers to many lists, such as a list of PresetUnit s.
 */
class Room
{
public:
	/** Move to the next Room.
	 *
	 * \return Whether there was another Room to move to or not.
	 */
	bool getNext();

	/** Reveal the Room on the map.
	 *
	 * \return Whether or not the Room was successfully revealed.
	 */
	bool reveal();

	/** Get the PresetUnit s from the current Room.
	 *
	 * \return An array of all the PresetUnit s from the current Room.
	 */
	PresetUnit[] getPresetUnits();

	/** Get the PresetUnit s from the current Room.
	 *
	 * \param nType The type of Unit to get. See Unit::type.
	 *
	 * \return An array of all the PresetUnit s from the current Room.
	 */
	PresetUnit[] getPresetUnits(int nType);

	/** Get the PresetUnit s from the current Room.
	 *
	 * \param nType The type of Unit to get. See Unit::type.
	 *
	 * \param nClass The class id of the Unit (s) to find. See Unit::classId.
	 *
	 * \return An array of all the PresetUnit s from the current Room.
	 */
	PresetUnit[] getPresetUnits(int nType, int nClass);

	/** Get the collision data for the Room.
	 *
	 * \todo Verify that the array does actually have reverse of usual
	 * dimensions.
	 *
	 * \return An array with the collision data. The first dimension is the y
	 * coordinate, the second is the x coordinate.
	 */
	int[][] getCollision();

	/** Get the "nearby" Rooms from pRoom2Near member.
	 *
	 * \return An array of the nearby Rooms.
	 */
	Room[] getNearby();

	/** Get some property of the Room.
	 *
	 * \param nStat Which property to get:
	 *
	 * 0 - xStart (room 1)
	 *
	 * 1 - yStart (room 1)
	 *
	 * 2 - xSize (room 1)
	 *
	 * 3 - ySize (room 1)
	 *
	 * 4 - posX (room 2)
	 *
	 * 5 - posY (room 2)
	 *
	 * 6 - sizeX (room 2)
	 *
	 * 7 - sizeY (room 2)
	 *
	 * 9 - posGameX (coll)
	 *
	 * 10 - posGameY (coll)
	 *
	 * 11 - sizeGameX (coll)
	 *
	 * 12 - sizeGameY (coll)
	 *
	 * 13 - posRoomX (coll)
	 *
	 * 14 - posGameY (coll)
	 *
	 * 15 - sizeRoomX (coll)
	 *
	 * 16 - sizeRoomY (coll)
	 *
	 * \return The stat value.
	 */
	int getStat(int nStat);

	/** Get the first Room of the level this Room is on.
	 *
	 * \return The first Room.
	 */
	Room getFirst();

	/** Determine whether or not the Unit is in the Room.
	 *
	 * \param unit The unit to check.
	 *
	 * \return Whether or not the Unit is in the Room.
	 */
	bool unitInRoom(Unit unit);

	/** Get the Room number.
	 *
	 * \todo Determine what a "room number" is.
	 */
	int number;

	/** Get the x coordinate of the Room.
	 */
	int x;

	/** Get the y coordinate of the Room.
	 */
	int y;

	/** Get the x size (width) of the Room.
	 */
	int xsize;

	/** Get the y size (height) of the Room.
	 */
	int ysize;

	/** No implementation.
	 */
	int subnumber;

	/** Get the Area ID for the area which the Room is in.
	 */
	int area;

	/** Get the Area ID for the area which the Room is in.
	 */
	int level;

	/** Get the correct tomb.
	 *
	 * \todo Determine if this is the Area ID of the correct tomb.
	 *
	 * \todo Determine what Room s this is valid on.
	 */
	int correcttomb;
};
