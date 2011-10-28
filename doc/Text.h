#include "ScreenHooks.h"

/** A Text screen hook
 *
 * \todo Verify all the documentation in this class.
 */
class Text
{
public:
	/** Create a Text hook with the given parameters.
	 *
	 * Uses defaults szText = "", x = 0, y = 0, color = 0, font = 0, align = 0,
	 * automap = false, click = null, hover = null.
	 */
	Text();

	/** Create a Text hook with the given parameters.
	 *
	 * Uses defaults x = 0, y = 0, color = 0, font = 0, align = 0,
	 * automap = false, click = null, hover = null.
	 *
	 * \param szText The string that the Text hook displays.
	 */
	Text(String szText);

	/** Create a Text hook with the given parameters.
	 *
	 * Uses defaults y = 0, color = 0, font = 0, align = 0, automap = false,
	 * click = null, hover = null.
	 *
	 * \param szText The string that the Text hook displays.
	 *
	 * \param x The x coordinate (left) of the Text.
	 */
	Text(String szText, int x);

	/** Create a Text hook with the given parameters.
	 *
	 * Uses defaults color = 0, font = 0, align = 0, automap = false,
	 * click = null, hover = null.
	 *
	 * \param szText The string that the Text hook displays.
	 *
	 * \param x The x coordinate (left) of the Text.
	 *
	 * \param y The y coordinate (top) of the Text.
	 */
	Text(String szText, int x, int y);

	/** Create a Text hook with the given parameters.
	 *
	 * Uses defaults font = 0, align = 0, automap = false, click = null,
	 * hover = null.
	 *
	 * \param szText The string that the Text hook displays.
	 *
	 * \param x The x coordinate (left) of the Text.
	 *
	 * \param y The y coordinate (top) of the Text.
	 *
	 * \param color The color of the Text.
	 *
	 * See: http://forums.d2botnet.org/viewtopic.php?f=18&t=1000
	 */
	Text(String szText, int x, int y, int color);

	/** Create a Text hook with the given parameters.
	 *
	 * Uses defaults align = 0, automap = false, click = null, hover = null.
	 *
	 * \param szText The string that the Text hook displays.
	 *
	 * \param x The x coordinate (left) of the Text.
	 *
	 * \param y The y coordinate (top) of the Text.
	 *
	 * \param color The color of the Text.
	 *
	 * See: http://forums.d2botnet.org/viewtopic.php?f=18&t=1000
	 *
	 * \param font Something that determines the font.
	 */
	Text(String szText, int x, int y, int color, int font);

	/** Create a Text hook with the given parameters.
	 *
	 * Uses defaults automap = false, click = null, hover = null.
	 *
	 * \param szText The string that the Text hook displays.
	 *
	 * \param x The x coordinate (left) of the Text.
	 *
	 * \param y The y coordinate (top) of the Text.
	 *
	 * \param color The color of the Text.
	 *
	 * See: http://forums.d2botnet.org/viewtopic.php?f=18&t=1000
	 *
	 * \param font Something that determines the font.
	 *
	 * \param align The horizontal alignment.
	 *
	 * 0 - Left
	 *
	 * 1 - Right
	 *
	 * 2 - Center
	 */
	Text(String szText, int x, int y, int color, int font, int align);

	/** Create a Text hook with the given parameters.
	 *
	 * Uses defaults click = null, hover = null.
	 *
	 * \param szText The string that the Text hook displays.
	 *
	 * \param x The x coordinate (left) of the Text.
	 *
	 * \param y The y coordinate (top) of the Text.
	 *
	 * \param color The color of the Text.
	 *
	 * See: http://forums.d2botnet.org/viewtopic.php?f=18&t=1000
	 *
	 * \param font Something that determines the font.
	 *
	 * \param align The horizontal alignment.
	 *
	 * 0 - Left
	 *
	 * 1 - Right
	 *
	 * 2 - Center
	 *
	 * \param automap Whether the Text is in automap coordinate space (true) or
	 * screen coordinate space (false).
	 */
	Text(String szText, int x, int y, int color, int font, int align,
		bool automap);

	/** Create a Text hook with the given parameters.
	 *
	 * Uses default hover = null.
	 *
	 * \param szText The string that the Text hook displays.
	 *
	 * \param x The x coordinate (left) of the Text.
	 *
	 * \param y The y coordinate (top) of the Text.
	 *
	 * \param color The color of the Text.
	 *
	 * See: http://forums.d2botnet.org/viewtopic.php?f=18&t=1000
	 *
	 * \param font Something that determines the font.
	 *
	 * \param align The horizontal alignment.
	 *
	 * 0 - Left
	 *
	 * 1 - Right
	 *
	 * 2 - Center
	 *
	 * \param automap Whether the Text is in automap coordinate space (true) or
	 * screen coordinate space (false).
	 *
	 * \param click The click handler that gets called when the Text is
	 * clicked.
	 */
	Text(String szText, int x, int y, int color, int font, int align,
		bool automap, ClickHandler click);

	/** Create a Text hook with the given parameters.
	 *
	 * \param szText The string that the Text hook displays.
	 *
	 * \param x The x coordinate (left) of the Text.
	 *
	 * \param y The y coordinate (top) of the Text.
	 *
	 * \param color The color of the Text.
	 *
	 * See: http://forums.d2botnet.org/viewtopic.php?f=18&t=1000
	 *
	 * \param font Something that determines the font.
	 *
	 * \param align The horizontal alignment.
	 *
	 * 0 - Left
	 *
	 * 1 - Right
	 *
	 * 2 - Center
	 *
	 * \param automap Whether the Text is in automap coordinate space (true) or
	 * screen coordinate space (false).
	 *
	 * \param click The click handler that gets called when the Text is
	 * clicked.
	 *
	 * \param hover The hover handler that gets called when the Text gets
	 * hovered over.
	 */
	Text(String szText, int x, int y, int color, int font, int align,
		bool automap, ClickHandler click, HoverHandler hover);

	/** Remove the Text hook from the screen and destroy the corresponding
	 * object.
	 */
	void remove();

	/** The x coordinate (left) of the Text.
	 */
	int x;

	/** The y coordinate (top) of the Text.
	 */
	int y;

	/** Whether or not the Text is visible.
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

	/** The z-order of the Text (what it covers up and is covered by).
	 */
	int zorder;

	/** The click handler that gets called when the Text is clicked.
	 */
	ClickHandler click;

	/** The hover handler that gets called when the Text gets hovered over.
	 */
	HoverHandler hover;

	/** The color of the Text.
	 *
	 * See: http://forums.d2botnet.org/viewtopic.php?f=18&t=1000
	 */
	int color;

	/** Something that determines the font.
	 */
	int font;

	/** The string that the Text hook displays.
	 */
	String text;
};
