#include "JSMenu.h"
//#include "Control.h"
#include "JSControl.h"
#include "Constants.h"
#include "Helpers.h"
#include "D2BS.h"

JSAPI_FUNC(my_login)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	char file[_MAX_FNAME+MAX_PATH], *profile = NULL,
		 mode[256], username[48], password[256], gateway[256], charname[24],
		 difficulty[10], maxLoginTime[10], maxCharTime[10];
	
	int loginTime = 0, charTime = 0, SPdifficulty = 0;
	bool copiedProfile = false;

	if(!JSVAL_IS_STRING(argv[0]))
	{
		if(Vars.szProfile != NULL)
		{
			int size = strlen(Vars.szProfile)+1;
			profile = new char[size];
			strcpy_s(profile, size, Vars.szProfile);
			copiedProfile = true;
		}
		else
			THROW_ERROR(cx, "Invalid profile specified!");
	} else {
		profile = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
		strcpy_s(Vars.szProfile, 256, profile);
	}

	if(!profile) THROW_ERROR(cx, "Could not get profile!");
	if(!ProfileExists(profile)) THROW_ERROR(cx, "Profile does not exist!");

	sprintf_s(file, sizeof(file), "%sd2bs.ini", Vars.szPath);

	GetPrivateProfileString(profile, "mode", "single", mode, sizeof(mode), file);
	GetPrivateProfileString(profile, "character", "ERROR", charname, sizeof(charname), file);
	GetPrivateProfileString(profile, "spdifficulty", "0", difficulty, sizeof(difficulty), file);
	GetPrivateProfileString(profile, "username", "ERROR", username, sizeof(username), file);
	GetPrivateProfileString(profile, "password", "ERROR", password, sizeof(password), file);
	GetPrivateProfileString(profile, "gateway", "ERROR", gateway, sizeof(gateway), file);

	if(copiedProfile)
		delete[] profile;

	GetPrivateProfileString("settings", "MaxLoginTime", "5", maxLoginTime, sizeof(maxLoginTime), file);
	GetPrivateProfileString("settings", "MaxCharSelectTime", "5", maxCharTime, sizeof(maxCharTime), file);

	char* errorMsg = "";
	loginTime = abs(atoi(maxLoginTime) * 1000);
	charTime = abs(atoi(maxCharTime) * 1000);
	SPdifficulty = atoi(difficulty);
	Control* pControl = NULL;
	int location = 0;
	int timeout = 0;
	bool loginComplete = FALSE,	skippedToBnet = TRUE;
	Vars.bBlockKeys = Vars.bBlockMouse = TRUE;

	char ipaddress[16] = "";
	/*
		clickedOnce is needed because, when in OOG_OTHER_MULTIPLAYER
		the clickControl () is done twice and the second time it is
		failing because the button is not there anymore.
	*/
	int clickedOnce = false;

	while(!loginComplete)
	{
		location = OOG_GetLocation();
		switch(location)
		{
			case OOG_D2SPLASH:
				clickControl(*p_D2WIN_FirstControl);
				break;
			case OOG_CHAR_SELECT:
				if (!OOG_SelectCharacter(charname))
					 errorMsg = "Invalid character name";
				break;
			case OOG_MAIN_MENU:
				if (tolower(mode[0])== 's')
					if(!clickControl(findControl(6, (char *)NULL, -1, 264,324,272,35)))
						 errorMsg = "Failed to click the Single button?";
				if(tolower(mode[0])== 'b')
				{
					OOG_SelectGateway(gateway, 256);
					if(!clickControl(findControl(6, (char *)NULL, -1, 264, 366, 272, 35)))
						 errorMsg = "Failed to click the 'Battle.net' button?";
				}
				if(tolower(mode[0])== 'o')
				{
					if(!clickControl(findControl(6, (char *)NULL, -1, 264, 433, 272, 35)))
						errorMsg =  "Failed to click the 'Other Multiplayer' button?";
					else
						skippedToBnet = FALSE;
				}
				break;
			case OOG_LOGIN:
				if((tolower(mode[0])== 's') || ((tolower(mode[0]) ==  'o') && skippedToBnet))
				{
					if(!clickControl(findControl(6, "EXIT", -1,33,572,128,35)))
						errorMsg =  "Failed to click the exit button?";
					break;
				}
				pControl = findControl(1, (char *)NULL, -1, 322, 342, 162, 19);
				if(pControl)
					SetControlText(pControl, username);
				else
					errorMsg = "Failed to set the 'Username' text-edit box.";
				// Password text-edit box
				pControl = findControl(1, (char *)NULL, -1, 322, 396, 162, 19);
				if(pControl)
					SetControlText(pControl, password);
				else
					errorMsg = "Failed to set the 'Password' text-edit box.";

				pControl = findControl(6, (char *)NULL, -1, 264, 484, 272, 35);
				if(pControl)
					if(!clickControl(pControl))
						errorMsg ="Failed to click the 'Log in' button?";
				timeout++;
				break;
			case OOG_DIFFICULTY:
				switch(SPdifficulty)
				{
				case 0:
					// normal button
					if(!clickControl(findControl(6, (char *)NULL, -1, 264, 297, 272, 35)))
						errorMsg ="Failed to click the 'Normal Difficulty' button?";
					break;
				case 1:
					// nightmare button
					if(!clickControl(findControl(6, (char *)NULL, -1, 264, 340, 272, 35)))
						errorMsg =  "Failed to click the 'Nightmare Difficulty' button?";
					break;
				case 2:
					// hell button
					if(!clickControl(findControl(6, (char *)NULL, -1, 264, 383, 272, 35)))
						errorMsg =  "Failed to click the 'Hell Difficulty' button?";
					break;
				default:
					errorMsg =  "Invalid single player difficulty level specified!";
					break;
				}
			case OOG_OTHER_MULTIPLAYER:
				// Open Battle.net
				if (tolower(mode[18])== 'o')
					if(!clickControl(findControl(6, (char *)NULL, -1, 264, 310, 272, 35)))
						errorMsg = "Failed to click the 'Open Battle.net' button?";
				// TCP/IP Game
				if (tolower(mode[18])== 't')
					if(!clickControl(findControl(6, (char *)NULL, -1, 264,350,272,35)) && !clickedOnce)
						errorMsg = "Failed to click the 'TCP/IP Game' button?";
					else
						clickedOnce = true;

				break;
			case OOG_TCP_IP:
				if (tolower(mode[25])== 'h')
					if(!clickControl(findControl(6, (char *)NULL, -1,265,206,272,35)))
						errorMsg = "Failed to click the 'Host Game' button?";
				if (tolower(mode[25])== 'j')
					if(!clickControl(findControl(6, (char *)NULL, -1,265,264,272,35)))
						errorMsg = "Failed to click the 'Join Game' button?";
				break;
			case OOG_ENTER_IP_ADDRESS:
				strncpy_s (ipaddress, &mode[30], (size_t)16);

				if (_strcmpi(ipaddress, "")) {
					pControl = findControl(1, (char *)NULL, -1, 300, 268, -1, -1);
					if(pControl) {
						SetControlText(pControl, ipaddress);

						// Click the OK button
						if(!clickControl(findControl(6, (char *)NULL, -1, 421, 337, 96, 32))) {
							errorMsg = "Failed to click the OK button";
						}
					} else
						errorMsg = "Failed to find the 'Host IP Address' text-edit box.";
				} else
					errorMsg = "Could not get the IP address from the profile in the d2bs.ini file.";

				break;
			case OOG_MAIN_MENU_CONNECTING: 
			case OOG_CHARACTER_SELECT_PLEASE_WAIT:
			case OOG_PLEASE_WAIT: 
			case OOG_GATEWAY:
			case OOG_CHARACTER_SELECT_NO_CHARS: 
			case OOG_NONE:
				timeout++;
				break;
			case OOG_LOBBY: 
			case OOG_INLINE: 
			case OOG_CHAT: 
			case OOG_CREATE:
			case OOG_JOIN: 
			case OOG_LADDER: 
			case OOG_CHANNEL: 
			case OOG_GAME_EXIST: 
			case OOG_GAME_DOES_NOT_EXIST:	
				loginComplete=TRUE;
				break;
			case OOG_UNABLE_TO_CONNECT:
				errorMsg = "Unable to connect";
				break;
			case OOG_CDKEY_IN_USE:
				errorMsg = "CD-Key in use";
				break;
			case OOG_LOGIN_ERROR:
				errorMsg = "Bad account or password";
				break;
			case OOG_REALM_DOWN:
				errorMsg = "Realm Down";
				break;
			default:
				errorMsg = "Unhandled login location";
				break;				
		}

		if(_strcmpi(errorMsg, ""))
		{
			Vars.bBlockKeys = Vars.bBlockMouse = FALSE;
			THROW_ERROR(cx, errorMsg);						
			break;
		}

		if((timeout*100) > loginTime)
		{
			Vars.bBlockKeys = Vars.bBlockMouse = FALSE;
			THROW_ERROR(cx, "login time out");
			break;
		}

		if(ClientState() == ClientStateInGame)
			loginComplete = TRUE;
		
		Sleep(100);
	}
	Vars.bBlockKeys = Vars.bBlockMouse = FALSE;

	return JS_TRUE;
}

JSAPI_FUNC(my_selectChar)
{
	if(argc != 1 || !JSVAL_IS_STRING(argv[0]))
		THROW_ERROR(cx, "Invalid parameters specified to selectCharacter");

	char* profile = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	if(!ProfileExists(profile))
		THROW_ERROR(cx, "Invalid profile specified");
	char charname[24], file[_MAX_FNAME+MAX_PATH];
	sprintf_s(file, sizeof(file), "%sd2bs.ini", Vars.szPath);
	GetPrivateProfileString(profile, "character", "ERROR", charname, sizeof(charname), file);

	*rval = JSVAL_TO_BOOLEAN(OOG_SelectCharacter(charname));
	return JS_TRUE;
}

JSAPI_FUNC(my_createGame)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	char *name = NULL, *pass = NULL;
	int32 diff = 3;
	if(!JS_ConvertArguments(cx, argc, argv, "s/si", &name, &pass, &diff))
	{
		JS_ReportError(cx, "Invalid arguments specified to createGame");
		return JS_FALSE;
	}
	if(!pass)
		pass = "";

	if(strlen(name) > 15 || strlen(pass) > 15)
		THROW_ERROR(cx, "Invalid game name or password length");

	if(!OOG_CreateGame(name, pass, diff))
		THROW_ERROR(cx, "createGame failed");

	return JS_TRUE;
}

JSAPI_FUNC(my_joinGame)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	char *name = NULL, *pass = NULL;
	if(!JS_ConvertArguments(cx, argc, argv, "s/s", &name, &pass))
	{
		JS_ReportError(cx, "Invalid arguments specified to createGame");
		return JS_FALSE;
	}
	if(!pass)
		pass = "";

	if(strlen(name) > 15 || strlen(pass) > 15)
		THROW_ERROR(cx, "Invalid game name or password length");

	if(!OOG_JoinGame(name, pass))
		THROW_ERROR(cx, "joinGame failed");

	return JS_TRUE;
}

JSAPI_FUNC(my_addProfile)
{
	// validate the args...
	char *profile, *mode, *gateway, *username, *password, *charname;
	profile = mode = gateway = username = password = charname = NULL;
	int spdifficulty = 3;
	if(argc < 6 || argc > 7)
		THROW_ERROR(cx, "Invalid arguments passed to addProfile");

	char** args[] = {&profile, &mode, &gateway, &username, &password, &charname};
	for(uintN i = 0; i < 6; i++)
	{
		if(!JSVAL_IS_STRING(argv[i]))
		{
			THROW_ERROR(cx, "Invalid argument passed to addProfile");
		}
		else
			*args[i] = JS_GetStringBytes(JSVAL_TO_STRING(argv[i]));
	}

	for(int i = 0; i < 6; i++)
	{
		if(!(*args[i]))
			THROW_ERROR(cx, "Failed to convert string");
	}

	if(argc == 7)
		spdifficulty = JSVAL_TO_INT(argv[6]);

	if(spdifficulty > 3 || spdifficulty < 0)
		THROW_ERROR(cx, "Invalid argument passed to addProfile");

	char file[_MAX_FNAME+_MAX_PATH];

	sprintf_s(file, sizeof(file), "%sd2bs.ini", Vars.szPath);
	if(!ProfileExists(*args[0]))
	{
		char settings[600] = "";
		sprintf_s(settings, sizeof(settings),
					"mode=%s\tgateway=%s\tusername=%s\tpassword=%s\tcharacter=%s\tspdifficulty=%d\t",
					mode, gateway, username, password, charname, spdifficulty);

		StringReplace(settings, '\t', '\0', 600);
		WritePrivateProfileSection(*args[0], settings, file);
	}

	return JS_TRUE;
}

JSAPI_FUNC(my_getOOGLocation)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	*rval = INT_TO_JSVAL(OOG_GetLocation());

	return JS_TRUE;
}

JSAPI_FUNC(my_createCharacter)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	char* name = NULL;
	int32 type = -1;
	JSBool hc = JS_FALSE, ladder = JS_FALSE;
	if(!JS_ConvertArguments(cx, argc, argv, "si/bb", &name, &type, &hc, &ladder))
		return JS_FALSE;

	*rval = BOOLEAN_TO_JSVAL(!!OOG_CreateCharacter(name, type, !!hc, !!ladder));
	return JS_TRUE;
}