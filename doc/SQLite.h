/** Class for dealing with an SQLite database.
 */
class SQLite
{
public:
	/** Create a database object which can be used to open SQLite databases and
	 * execute queries. Opens the :memory: path immediately.
	 */
	SQLite();

	/** Create a database object which can be used to open SQLite databases and
	 * execute queries. Opens the database immediately.
	 *
	 * \param path The path to the database to open.
	 */
	SQLite(String path);

	/** Create a database object which can be used to open SQLite databases and
	 * execute queries.
	 *
	 * \param path The path to the database to open.
	 *
	 * \param autoOpen Whether to open the database immediately or not.
	 */
	SQLite(String path, bool autoOpen);

	/** Execute a query on the database.
	 *
	 * \param queryString The query string to execute.
	 *
	 * \return true
	 */
	bool execute(String queryString);

	/** Do a sqlite3_prepare_v2 on the database with sql, then call the
	 * appropriate sqlite3_bind_ functions on the remaining parameters.
	 *
	 * \param sql The query to pass to sqlite3_prepare v2.
	 *
	 * \todo Determine what this function does (better documenation).
	 *
	 * \return The resulting DBStatement.
	 */
	DBStatement query(String sql, ...);

	/** Open the database.
	 *
	 * \return true
	 */
	bool open();

	/** Close the database.
	 *
	 * \return true
	 */
	bool close();

	/** Get the path used to open the database.
	 */
	String path;

	/** Get all the statements that have been queried.
	 */
	DBStatement[] statements;

	/** Whether or not the database is open.
	 */
	bool isOpen;

	/** Get the row id of the last inserted row.
	 */
	int lastRowId;

	/** Get the number of rows modified/inserted/deleted by the last statement.
	 */
	double changes;
};
