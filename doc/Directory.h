/** Represents a folder.
 */
class Folder
{
public:
	/** The path name to the directory.
	 */
	String name;

	/** Create a new directory.
	 *
	 * \param name The name of the directory to create under the current
	 * directory.
	 *
	 * \return The new Folder created.
	 */
	Folder create(String name);

	/** Remove the directory. Must be empty to work properly.
	 *
	 * \return true.
	 */
	bool remove();

	/** Get the filename for each of the files in the directory.
	 *
	 * \return An array of all the filenames.
	 */
	String[] getFiles();

	/** Get the name for each of the sub-directories in the current directory.
	 *
	 * \return An array of all the folder names.
	 */
	String[] getFolders();
};
