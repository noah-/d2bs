/** \page shook ScreenHook documentation
 * \tableofcontents
 * \section opacity Explanation of opacity parameter.
 * There is vague evidence that the range of legal values for the opacity
 * parameter is 0-7. Through testing the following information has been gleaned.
 *
 * Order of opacity:
 *
 * 3 - Completely transparent
 *
 * 0 - Very transparent
 *
 * 1
 *
 * 6 - Color seems to be ignored, treated as black
 *
 * 2 - Very opaque
 *
 * 4 - Color seems to be ignored, treated black, completely opaque
 *
 * 5, 7 - Completely opaque, with color
 *
 */

/** The type of a click handler.
 *
 * \ingroup handlers
 *
 * \param button The button clicked.
 *
 * \param x The x coordinate clicked.
 *
 * \param y The y coordinate clicked.
 */
typedef ClickHandler (bool*)(int button, int x, int y);

/** The type of a hover handler.
 *
 * \ingroup handlers
 *
 * \param x The x coordinate hovered.
 *
 * \param y The y coordinate hovered.
 */
typedef HoverHandler (void*)(int x, int y);
