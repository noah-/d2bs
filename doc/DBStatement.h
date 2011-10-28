/** A class describing ...
 *
 * \todo Get a better description (and understanding).
 */
class DBStatement
{
public:
	/** An object with property names from the column names and property values
	 * coming from the data.
	 *
	 * \return The object with all the data.
	 */
	Object getObject();

	/** Get the column count.
	 *
	 * \return The column count.
	 */
	int getColumnCount();

	/** Get the value from the column specified by index.
	 *
	 * \param colIndex The index of the column to get data from.
	 *
	 * \return The data.
	 */
	double getColumnValue(int colIndex);

	/** Get the value from the column specified by index.
	 *
	 * \param colIndex The index of the column to get data from.
	 *
	 * \return The data.
	 */
	String getColumnValue(int colIndex);

	/** Get the name of the column at the given index.
	 *
	 * \param colIndex The index of the column to get data from.
	 *
	 * \return The column name.
	 */
	getColumnName(int colIndex);

	/** Call sqlite3_step and close_db_stmt on the statement.
	 *
	 * \todo Interpret what this means.
	 *
	 * \return Whether the result is SQLITE_DONE.
	 */
	bool go();

	/** Call sqlite3_step on the statement.
	 *
	 * \todo Interpret what this means and how's it's different from go().
	 *
	 * \return Whether the result is SQLITE_ROW.
	 */
	bool next();

	/** Skip a certain number of rows in the results.
	 *
	 * \param rows The number of rows to skip.
	 *
	 * \return The number of rows actually skipped.
	 */
	int skip(int rows);

	/** Call sqlite3_reset.
	 *
	 * \todo Interpret what this does.
	 *
	 * \return true.
	 */
	bool reset();

	/** Close the statement.
	 *
	 * \return true.
	 */
	bool close();

	/** Bind the given data to the given column.
	 *
	 * \param colnum The column number to bind to.
	 *
	 * \param val The data to bind.
	 *
	 * \return true.
	 */
	bool bind(int colnum, double val);

	/** Bind the given data to the given column.
	 *
	 * \param colnum The column number to bind to.
	 *
	 * \param val The data to bind.
	 *
	 * \return true.
	 */
	bool bind(int colnum, int val);

	/** Bind the given data to the given column.
	 *
	 * \param colnum The column number to bind to.
	 *
	 * \param val The data to bind.
	 *
	 * \return true.
	 */
	bool bind(int colnum, String val);

	/** Bind the given data to the given column.
	 *
	 * \param colnum The column number to bind to.
	 *
	 * \param val The data to bind.
	 *
	 * \return true.
	 */
	bool bind(int colnum, bool val);

	/** The statement string.
	 */
	String sql;

	/** Whether or not there is more data to be gotten.
	 */
	bool ready;
};
