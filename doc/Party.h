/** Represents the items in the list in the Party screen. Also called a
 * RosterUnit.
 */
class Party
{
public:
	/** The x coordinate of the location of the RosterUnit.
	 */
	int x;

	/** The y coordinate of the location of the RosterUnit.
	 */
	int y;

	/** The Area ID of the RosterUnit.
	 */
	int area;

	/** The unique id of the RosterUnit.
	 */
	double gid;

	/** The life of the RosterUnit.
	 *
	 * \todo Determine how one is to get the maximum life of the RosterUnit.
	 */
	int life;

	/** The Party flags of the RosterUnit.
	 *
	 * \todo Determine the values that this can take on.
	 */
	int partyflag;

	/** The Party ID.
	 *
	 * \todo Determine what a Party ID is.
	 */
	int partyid;

	/** The name of the RosterUnit.
	 */
	String name;

	/** The class ID of the RosterUnit. See Unit::classId.
	 */
	int classid;

	/** The character level of the RosterUnit.
	 *
	 * \todo Verify that this level is the character level.
	 */
	int level;

	/** Move to the next RosterUnit.
	 */
	void getNext();
};
