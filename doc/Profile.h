/** This class represents the details needed to get a player to a game or lobby.
 *
 * This class hides the password. It provides the login() method to use the
 * password.
 */
class Profile
{
public:
	/** Create a Profile for the currently set profile.
 	 */
	Profile();

	/** Create a Profile for the named profile.
 	 *
	 * \param profileName Name of the profile to load.
 	 */
	Profile(String profileName);

	/** Create a Profile with the given information.
 	 *
	 * \param pt The type of profile, this constructor handles singlePlayer and
	 *		tcpIpHost types.
	 *
	 * \param charName Name of the character.
	 *
	 * \param diff Difficulty, 0 - normal, 1 - nightmare, 2 - hell.
 	 */
	Profile(ProfileType pt, String charName, int8_t diff);

	/** Create a Profile with the given information.
 	 *
	 * \param pt The type of profile, this constructor handles battleNet and
	 * 		openBattleNet types.
	 *
	 * \param account Account name.
	 *
	 * \param pass Password.
	 *
	 * \param charName Name of the character.
	 *
	 * \param gateway Search string for the gateway (any substring of the
	 *		desired gateway, will take first gateway that matches).
 	 */
	Profile(ProfileType pt, String account, String pass, String charName,
				String gateway);

	/** Create a Profile with the given information.
 	 *
	 * \param pt The type of profile, this constructor handles singlePlayer and
	 *		tcpIpHost types.
	 *
	 * \param charName Name of the character.
	 *
	 * \param ip IP address to connect to.
 	 */
	Profile(ProfileType pt, String charName, String ip);

	/** Log the profile in.
 	 */
	void login();

	/** The type of profile, see ProfileType.
 	 */
	ProfileType type;

	union
	{
		/** The IP of the game to connect to (TCP/IP join).
 		 */
		String ip;
		/** The username to use to connect to battle.net.
 		 */
		String username;
	}

	/** The battle.net gateway to connect to.
 	 */
	String gateway;

	/** The character to select.
 	 */
	String character;

	/** The difficulty to use: 0 - normal, 1 - nightmare, 2 - hell.
 	 */
	int8_t difficulty;

	/** The number of milliseconds to wait while logging in.
 	 */
	uint32_t maxLoginTime;

	/** The number of milliseconds to wait for the characters to load.
 	 */
	uint32_t maxCharacterSelectTime;
};
