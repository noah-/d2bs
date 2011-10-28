/** A separate context to run scripts in.
 */
class Sandbox
{
public:
	/** Create a Sandbox. This creates a separate context to run things in.
	 */
	Sandbox();

	/** Evaluate the code passed in and return the result.
	 *
	 * \param code The code to run, literally JavaScript code to run.
	 *
	 * \return The result of the last expression statement.
	 */
	Object evaluate(String code);

	/** Include a file from ScriptPath\\libs\\file.
	 *
	 * \param file The filename relative to ScriptPath\\libs to include.
	 *
	 * \return The result of the last expression statement.
	 */
	Object include(String file);

	/** Determine whether a file has been included yet or not.
	 *
	 * \param file The filename relative to ScriptPath\\libs to include.
	 *
	 * \return Whether or not the file has been included.
	 */
	bool isInclude(String file);

	/** Clear the scope of the Sandbox's global object.
	 *
	 * \todo Clarify what this means (i.e. post-conditions).
	 */
	void clearScope();
};
