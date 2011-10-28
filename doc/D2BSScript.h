/** Represents a script (a thread), created by either loading a file, or typing
 * something in the console.
 */
class D2BSScript
{
public:
	/** Get the next script.
	 *
	 * \return Whether there was another script or not.
	 */
	bool getNext();

	/** Pause the script.
	 */
	void pause();

	/** Resume the script.
	 */
	void resume();

	/** Stop the script.
	 */
	void stop();

	/** Send some values to the script. Has the effect of calling the any
	 * listeners to scriptmsg with the parameters provided.
	 */
	void send(...);

	/** The relative filename of the script. If the script is from the console
	 * the filename will be reported as "Command Line".
	 */
	String name;

	/** The type of script.
	 *
	 * 0 - InGame
	 *
	 * 1 - OutOfGame or Command
	 */
	int type;

	/** Whether or not the script is running.
	 */
	bool running;

	/** The thread id.
	 */
	int threadid;
};
