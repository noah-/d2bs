#include "ScreenHooks.h"

/** A Box screen hook
 *
 * \todo Explain (and understand) this better.
 * \todo Verify all the documentation in this class.
 */
class Box
{
public:
	/** Create a Box hook with the given parameters.
	 *
	 * \param x The x coordinate (left) of the Box.
	 *
	 * \param y The y coordinate (top) of the Box.
	 *
	 * \param x2 The xsize (width) of the Box.
	 *
	 * \param y2 The ysize (height) of the Box.
	 *
	 * \param color The color of the Box.
	 *
	 * See: http://forums.d2botnet.org/viewtopic.php?f=18&t=1000
	 *
	 * \param opacity See \ref opacity
	 *
	 * \param align The horizontal alignment.
	 *
	 * 0 - Left
	 *
	 * 1 - Right
	 *
	 * 2 - Center
	 *
	 * \param automap Whether the Box is in automap coordinate space (true) or
	 * screen coordinate space (false).
	 *
	 * \param click The click handler that gets called when the Box is
	 * clicked.
	 *
	 * \param hover The hover handler that gets called when the Box gets
	 * hovered over.
	 */
	Box(int x = 0, int y = 0, int x2 = 0, int y2 = 0, int color = 0,
		int opacity = 0, int align = 0, bool automap = false,
		ClickHandler click = null, HoverHandler hover = null);

	/** Remove the Box from the screen and destroy the corresponding object.
	 */
	void remove();

	/** The x coordinate (left) of the Box.
	 */
	int x;

	/** The y coordinate (top) of the Box.
	 */
	int y;

	/** The xsize (width) of the Box.
	 */
	int xsize;

	/** The ysize (height) of the Box.
	 */
	int ysize;

	/** Whether or not the Box is visible.
	 */
	bool visible;

	/** The horizontal alignment.
	 *
	 * 0 - Left
	 *
	 * 1 - Right
	 *
	 * 2 - Center
	 */
	int align;

	/** The z-order of the Box (what it covers up and is covered by).
	 */
	int zorder;

	/** The click handler that gets called when the Box is clicked.
	 */
	ClickHandler click;

	/** The hover handler that gets called when the Box gets hovered over.
	 */
	HoverHandler hover;

	/** The color of the Box.
	 *
	 * See: http://forums.d2botnet.org/viewtopic.php?f=18&t=1000
	 */
	int color;

	/** How much of the controls underneath the Box should show through.
	 */
	int opacity;
};
