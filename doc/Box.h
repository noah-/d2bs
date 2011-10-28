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
	 * Uses defaults x = 0, y = 0, x2 = 0, y2 = 0, color = 0, opacity = 0,
	 * align = 0, automap = false, click = null, hover = null.
	 */
	Box();

	/** Create a Box hook with the given parameters.
	 *
	 * Uses defaults y = 0, x2 = 0, y2 = 0, color = 0, opacity = 0, align = 0,
	 * automap = false, click = null, hover = null.
	 *
	 * \param x The x coordinate (left) of the Box.
	 */
	Box(int x);

	/** Create a Box hook with the given parameters.
	 *
	 * Uses defaults x2 = 0, y2 = 0, color = 0, opacity = 0, align = 0,
	 * automap = false, click = null, hover = null.
	 *
	 * \param x The x coordinate (left) of the Box.
	 *
	 * \param y The y coordinate (top) of the Box.
	 */
	Box(int x, int y);

	/** Create a Box hook with the given parameters.
	 *
	 * Uses defaults y2 = 0, color = 0, opacity = 0, align = 0, automap = false,
	 * click = null, hover = null.
	 *
	 * \param x The x coordinate (left) of the Box.
	 *
	 * \param y The y coordinate (top) of the Box.
	 *
	 * \param x2 The xsize (width) of the Box.
	 */
	Box(int x, int y, int x2);

	/** Create a Box hook with the given parameters.
	 *
	 * Uses defaults color = 0, opacity = 0, align = 0, automap = false,
	 * click = null, hover = null.
	 *
	 * \param x The x coordinate (left) of the Box.
	 *
	 * \param y The y coordinate (top) of the Box.
	 *
	 * \param x2 The xsize (width) of the Box.
	 *
	 * \param y2 The ysize (height) of the Box.
	 */
	Box(int x, int y, int x2, int y2);

	/** Create a Box hook with the given parameters.
	 *
	 * Uses defaults opacity = 0, align = 0, automap = false, click = null,
	 * hover = null.
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
	 */
	Box(int x, int y, int x2, int y2, int color);

	/** Create a Box hook with the given parameters.
	 *
	 * Uses defaults align = 0, automap = false, click = null, hover = null.
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
	 * \param opacity How much of the controls underneath the Box should show
	 * through.
	 */
	Box(int x, int y, int x2, int y2, int color, int opacity);

	/** Create a Box hook with the given parameters.
	 *
	 * Uses defaults automap = false, click = null, hover = null.
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
	 * \param opacity How much of the controls underneath the Box should show
	 * through.
	 *
	 * \param align The horizontal alignment.
	 *
	 * 0 - Left
	 *
	 * 1 - Right
	 *
	 * 2 - Center
	 */
	Box(int x, int y, int x2, int y2, int color, int opacity, int align);

	/** Create a Box hook with the given parameters.
	 *
	 * Uses defaults click = null, hover = null.
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
	 * \param opacity How much of the controls underneath the Box should show
	 * through.
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
	 */
	Box(int x, int y, int x2, int y2, int color, int opacity, int align,
		bool automap);

	/** Create a Box hook with the given parameters.
	 *
	 * Uses default hover = null.
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
	 * \param opacity How much of the controls underneath the Box should show
	 * through.
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
	 */
	Box(int x, int y, int x2, int y2, int color, int opacity, int align,
		bool automap, ClickHandler click);

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
	 * \param opacity How much of the controls underneath the Box should show
	 * through.
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
	Box(int x, int y, int x2, int y2, int color, int opacity, int align,
		bool automap, ClickHandler click, HoverHandler hover);

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
