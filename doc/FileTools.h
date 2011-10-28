/** A collection of tools to do things like move files and determine whether
 * files exist.
 */
class FileTools
{
public:
	/** Remove (delete) a file.
	 *
	 * \param file The filename of the file to delete.
	 */
	static void remove(String file);

	/** Rename a file.
	 *
	 * \param orig The original filename.
	 *
	 * \param newName The new filename.
	 */
	static void rename(String orig, String newName);

	/** Copy a file.
	 *
	 * \param orig The original filename.
	 *
	 * \param newName The filename of the new file.
	 */
	static void copy(String orig, String newName);

	/** Determine if a file exists.
	 *
	 * \param file The filename of the file to check.
	 *
	 * \return Whether or not the file exists.
	 */
	static bool exists(String file);

	/** Read the contents of a file into a String.
	 *
	 * \param file The filename of the file to open.
	 *
	 * \return The contents of the file.
	 */
	static String readText(String file);

	/** Write the string representation (for standard files) or the binary
	 * representation (for binary files) of each of the arguments to the file.
	 *
	 * \param file The filename of the file to write to.
	 *
	 * \return Whether or not the operation was successful.
	 */
	bool writeText(String file, ...);

	/** Append the string representation (for standard files) or the binary
	 * representation (for binary files) of each of the arguments to the file.
	 *
	 * \param file The filename of the file to write to.
	 *
	 * \return Whether or not the operation was successful.
	 */
	bool appendText(String file, ...);
};
