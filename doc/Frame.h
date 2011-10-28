#include "ScreenHooks.h"

/** This class represents a screen hook for a Frame.
 *
 * \todo Explain (and understand) this better.
 * \todo Verify all the documentation in this class.
 */
class Frame
{
public:
	/** Create a Frame hook with the given parameters.
	 *
	 * Uses defaults x = 0, y = 0, x2 = 0, y2 = 0, align = 0, automap = false,
	 * click = null, hover = null.
	 */
	Frame();

	/** Create a Frame hook with the given parameters.
	 *
	 * Uses defaults y = 0, x2 = 0, y2 = 0, align = 0, automap = false,
	 * click = null, hover = null.
	 *
	 * \param x The x coordinate (left) of the Frame.
	 */
	Frame(int x);

	/** Create a Frame hook with the given parameters.
	 *
	 * Uses defaults x2 = 0, y2 = 0, align = 0, automap = false, click = null,
	 * hover = null.
	 *
	 * \param x The x coordinate (left) of the Frame.
	 *
	 * \param y The y coordinate (top) of the Frame.
	 */
	Frame(int x, int y);

	/** Create a Frame hook with the given parameters.
	 *
	 * Uses defaults y2 = 0, align = 0, automap = false, click = null,
	 * hover = null.
	 *
	 * \param x The x coordinate (left) of the Frame.
	 *
	 * \param y The y coordinate (top) of the Frame.
	 *
	 * \param x2 The xsize (width) of the Frame.
	 */
	Frame(int x, int y, int x2);

	/** Create a Frame hook with the given parameters.
	 *
	 * Uses defaults align = 0, automap = false, click = null, hover = null.
	 *
	 * \param x The x coordinate (left) of the Frame.
	 *
	 * \param y The y coordinate (top) of the Frame.
	 *
	 * \param x2 The xsize (width) of the Frame.
	 *
	 * \param y2 The ysize (height) of the Frame.
	 */
	Frame(int x, int y, int x2, int y2);

	/** Create a Frame hook with the given parameters.
	 *
	 * Uses defaults automap = false, click = null, hover = null.
	 *
	 * \param x The x coordinate (left) of the Frame.
	 *
	 * \param y The y coordinate (top) of the Frame.
	 *
	 * \param x2 The xsize (width) of the Frame.
	 *
	 * \param y2 The ysize (height) of the Frame.
	 *
	 * \param align The horizontal alignment.
	 *
	 * 0 - Left
	 *
	 * 1 - Right
	 *
	 * 2 - Center
	 */
	Frame(int x, int y, int x2, int y2, int align);

	/** Create a Frame hook with the given parameters.
	 *
	 * Uses defaults automap = false, click = null, hover = null.
	 *
	 * \param x The x coordinate (left) of the Frame.
	 *
	 * \param y The y coordinate (top) of the Frame.
	 *
	 * \param x2 The xsize (width) of the Frame.
	 *
	 * \param y2 The ysize (height) of the Frame.
	 *
	 * \param align The horizontal alignment.
	 *
	 * 0 - Left
	 *
	 * 1 - Right
	 *
	 * 2 - Center
	 *
	 * \param automap Whether the Frame is in automap coordinate space (true) or
	 * screen coordinate space (false).
	 */
	Frame(int x, int y, int x2, int y2, int align, bool automap);

	/** Create a Frame hook with the given parameters.
	 *
	 * Uses default hover = null.
	 *
	 * \param x The x coordinate (left) of the Frame.
	 *
	 * \param y The y coordinate (top) of the Frame.
	 *
	 * \param x2 The xsize (width) of the Frame.
	 *
	 * \param y2 The ysize (height) of the Frame.
	 *
	 * \param align The horizontal alignment.
	 *
	 * 0 - Left
	 *
	 * 1 - Right
	 *
	 * 2 - Center
	 *
	 * \param automap Whether the Frame is in automap coordinate space (true) or
	 * screen coordinate space (false).
	 *
	 * \param click The click handler that gets called when the Frame is
	 * clicked.
	 */
	Frame(int x, int y, int x2, int y2, int align, bool automap,
		ClickHandler click);

	/** Create a Frame hook with the given parameters.
	 *
	 * \param x The x coordinate (left) of the Frame.
	 *
	 * \param y The y coordinate (top) of the Frame.
	 *
	 * \param x2 The xsize (width) of the Frame.
	 *
	 * \param y2 The ysize (height) of the Frame.
	 *
	 * \param align The horizontal alignment.
	 *
	 * 0 - Left
	 *
	 * 1 - Right
	 *
	 * 2 - Center
	 *
	 * \param automap Whether the Frame is in automap coordinate space (true) or
	 * screen coordinate space (false).
	 *
	 * \param click The click handler that gets called when the Frame is
	 * clicked.
	 *
	 * \param hover The hover handler that gets called when the Frame gets
	 * hovered over.
	 */
	Frame(int x, int y, int x2, int y2, int align, bool automap,
		ClickHandler click, HoverHandler hover);

	/** Remove the Frame from the screen and destroy the corresponding object.
	 */
	void remove();

	/** The x coordinate (left) of the Frame.
	 */
	int x;

	/** The y coordinate (top) of the Frame.
	 */
	int y;

	/** The xsize (width) of the Frame.
	 */
	int xsize;

	/** The ysize (height) of the Frame.
	 */
	int ysize;

	/** Whether or not the Frame is visible.
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

	/** The z-order of the Frame (what it covers up and is covered by).
	 */
	int zorder;

	/** The click handler that gets called when the Frame is clicked.
	 */
	ClickHandler click;

	/** The hover handler that gets called when the Frame gets hovered over.
	 */
	HoverHandler hover;
};
