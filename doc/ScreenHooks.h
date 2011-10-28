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
