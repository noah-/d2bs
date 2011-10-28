#include "ScreenHooks.h"

/** This class represents a screen hook for a Image.
 *
 * \todo Explain (and understand) this better.
 * \todo Verify all the documentation in this class.
 */
class Image
{
public:
	/** Create an Image hook with the given parameters.
	 *
	 * Uses defaults szText = "", x = 0, y = 0, color = 0, align = 0,
	 * automap = false, click = null, hover = null
	 */
	Image();

	/** Create an Image hook with the given parameters.
	 *
	 * Uses defaults x = 0, y = 0, color = 0, align = 0, automap = false,
	 * click = null, hover = null
	 *
	 * \param szText The filename to load the image from.
	 */
	Image(String szText);

	/** Create an Image hook with the given parameters.
	 *
	 * Uses defaults y = 0, color = 0, align = 0, automap = false, click = null,
	 * hover = null
	 *
	 * \param szText The filename to load the image from.
	 *
	 * \param x The x coordinate (left) of the Image.
	 */
	Image(String szText, int x);

	/** Create an Image hook with the given parameters.
	 *
	 * Uses defaults color = 0, align = 0, automap = false, click = null,
	 * hover = null
	 *
	 * \param szText The filename to load the image from.
	 *
	 * \param x The x coordinate (left) of the Image.
	 *
	 * \param y The y coordinate (top) of the Image.
	 */
	Image(String szText, int x, int y);

	/** Create an Image hook with the given parameters.
	 *
	 * Uses defaults align = 0, automap = false, click = null, hover = null
	 *
	 * \param szText The filename to load the image from.
	 *
	 * \param x The x coordinate (left) of the Image.
	 *
	 * \param y The y coordinate (top) of the Image.
	 *
	 * \param color The color of the Image.
	 *
	 * See: http://forums.d2botnet.org/viewtopic.php?f=18&t=1000
	 */
	Image(String szText, int x, int y, int color);

	/** Create an Image hook with the given parameters.
	 *
	 * Uses defaults automap = false, click = null, hover = null
	 *
	 * \param szText The filename to load the image from.
	 *
	 * \param x The x coordinate (left) of the Image.
	 *
	 * \param y The y coordinate (top) of the Image.
	 *
	 * \param color The color of the Image.
	 *
	 * See: http://forums.d2botnet.org/viewtopic.php?f=18&t=1000
	 *
	 * \param align The horizontal alignment.
	 *
	 * 0 - Left
	 *
	 * 1 - Right
	 *
	 * 2 - Center
	 */
	Image(String szText, int x, int y, int color, int align);

	/** Create an Image hook with the given parameters.
	 *
	 * Uses defaults click = null, hover = null
	 *
	 * \param szText The filename to load the image from.
	 *
	 * \param x The x coordinate (left) of the Image.
	 *
	 * \param y The y coordinate (top) of the Image.
	 *
	 * \param color The color of the Image.
	 *
	 * See: http://forums.d2botnet.org/viewtopic.php?f=18&t=1000
	 *
	 * \param align The horizontal alignment.
	 *
	 * 0 - Left
	 *
	 * 1 - Right
	 *
	 * 2 - Center
	 *
	 * \param automap Whether the Image is in automap coordinate space (true) or
	 * screen coordinate space (false).
	 */
	Image(String szText, int x, int y, int color, int align, bool automap);

	/** Create an Image hook with the given parameters.
	 *
	 * Uses default hover = null
	 *
	 * \param szText The filename to load the image from.
	 *
	 * \param x The x coordinate (left) of the Image.
	 *
	 * \param y The y coordinate (top) of the Image.
	 *
	 * \param color The color of the Image.
	 *
	 * See: http://forums.d2botnet.org/viewtopic.php?f=18&t=1000
	 *
	 * \param align The horizontal alignment.
	 *
	 * 0 - Left
	 *
	 * 1 - Right
	 *
	 * 2 - Center
	 *
	 * \param automap Whether the Image is in automap coordinate space (true) or
	 * screen coordinate space (false).
	 *
	 * \param click The click handler that gets called when the Image is
	 * clicked.
	 */
	Image(String szText, int x, int y, int color, int align, bool automap,
		ClickHandler click);

	/** Create an Image hook with the given parameters.
	 *
	 * \param szText The filename to load the image from.
	 *
	 * \param x The x coordinate (left) of the Image.
	 *
	 * \param y The y coordinate (top) of the Image.
	 *
	 * \param color The color of the Image.
	 *
	 * See: http://forums.d2botnet.org/viewtopic.php?f=18&t=1000
	 *
	 * \param align The horizontal alignment.
	 *
	 * 0 - Left
	 *
	 * 1 - Right
	 *
	 * 2 - Center
	 *
	 * \param automap Whether the Image is in automap coordinate space (true) or
	 * screen coordinate space (false).
	 *
	 * \param click The click handler that gets called when the Image is
	 * clicked.
	 *
	 * \param hover The hover handler that gets called when the Image gets
	 * hovered over.
	 */
	Image(String szText, int x, int y, int color, int align, bool automap,
		ClickHandler click, HoverHandler hover);

	/** Remove the Image from the screen and destroy the corresponding object.
	 */
	void remove();

	/** The x coordinate (left) of the Image.
	 */
	int x;

	/** The y coordinate (top) of the Image.
	 */
	int y;

	/** Whether or not the Image is visible.
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

	/** The z-order of the Image (what it covers up and is covered by).
	 */
	int zorder;

	/** The click handler that gets called when the Image is clicked.
	 */
	ClickHandler click;

	/** The hover handler that gets called when the Image gets hovered over.
	 */
	HoverHandler hover;

	/** Location of the file to load for display.
	 */
	String location;
};
