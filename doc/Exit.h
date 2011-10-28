/** Represents a method of getting from one Area to another.
 */
class Exit
{
public:
	/** The x coordinate of the exit.
	 */
	int x;

	/** The y coordinate of the exit.
	 */
	int y;

	/** The Area ID of the target level.
	 */
	int target;

	/** The Exit type.
	 *
	 * 1 - A walk through Exit
	 *
	 * 2 - A stairs type Exit
	 */
	int type;

	/** The class ID of the Exit if it is a stairs type Exit, 0 otherwise.
	 */
	int tileid;

	/** The Area ID of the source side of the Exit.
	 */
	int level;
};
