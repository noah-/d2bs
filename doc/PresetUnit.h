/** Represents a PresetUnit (a type of Unit that is available as soon as the
 * Area data is available).
 *
 * \todo Determine how location is determine based off roomx, roomy, x, and y.
 */
class PresetUnit
{
public:
	/** The type ID. See Unit::type.
	 */
	int type;

	/** The room X (?).
	 */
	int roomx;

	/** The room Y (?).
	 */
	int roomy;

	/** The X coordinate (?).
	 */
	int x;

	/** The Y coordinate (?).
	 */
	int y;

	/** The unique ID of the PresetUnit.
	 */
	int id;

	/** The Area ID of the Area the PresetUnit is in.
	 */
	int level;
};
