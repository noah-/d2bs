#include "ScreenHooks.h"

/** A 2d line.
 *
 * \todo Verify all the documentation in this class.
 */
class Line
{
public:
	/** Create a Line hook with the given parameters.
	 *
	 * \param x The x coordinate (left) of the Line.
	 *
	 * \param y The y coordinate (top) of the Line.
	 *
	 * \param x2 The xsize (width) of the Line.
	 *
	 * \param y2 The ysize (height) of the Line.
	 *
	 * \param color The color of the Line.
	 *
	 * See: http://forums.d2botnet.org/viewtopic.php?f=18&t=1000
	 *
	 * \param automap Whether the Line is in automap coordinate space (true) or
	 * screen coordinate space (false).
	 *
	 * \param click The click handler that gets called when the Line is
	 * clicked.
	 *
	 * \param hover The hover handler that gets called when the Line gets
	 * hovered over.
	 */
	Line(int x = 0, int y = 0, int x2 = 0, int y2 = 0, int color = 0,
		bool automap = false, ClickHandler click = null,
		HoverHandler hover = null);

	/** Remove the Line from the screen and destroy the corresponding object.
	 */
	void remove();

	/** The first x coordinate of the Line.
	 */
	int x;

	/** The first y coordinate of the Line.
	 */
	int y;

	/** The second x coordinate of the Line.
	 */
	int x2;

	/** The second y coordinate of the Line.
	 */
	int y2;

	/** Whether or not the Line is visible.
	 */
	bool visible;

	/** The color of the Line.
	 *
	 * See: http://forums.d2botnet.org/viewtopic.php?f=18&t=1000
	 */
	int color;

	/** The z-order of the Line (what it covers up and is covered by).
	 */
	int zorder;

	/** The click handler that gets called when the Line is clicked.
	 */
	ClickHandler click;

	/** The hover handler that gets called when the Line gets hovered over.
	 */
	HoverHandler hover;
};
