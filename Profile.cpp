#include "D2BS.h"
#include "Profile.h"
#include "Control.h"

void Profile::init(const char* profileName)
{
	char file[_MAX_FNAME+MAX_PATH], difficulty[10], _maxLoginTime[10], _maxCharTime[10], mode[256];
	int tmp;

	if(profileName == NULL)
		throw "Can't open null profile name.";

	if(profileName[0] == '\0')
		throw "Can't open empty profile name.";

	sprintf_s(file, sizeof(file), "%sd2bs.ini", Vars.szPath);

	GetPrivateProfileString(profileName, "mode", "single", mode, sizeof(mode), file);
	GetPrivateProfileString(profileName, "character", "ERROR", charname, sizeof(charname), file);
	GetPrivateProfileString(profileName, "spdifficulty", "0", difficulty, sizeof(difficulty), file);
	GetPrivateProfileString(profileName, "username", "ERROR", username, sizeof(username), file);
	GetPrivateProfileString(profileName, "password", "ERROR", password, sizeof(password), file);
	GetPrivateProfileString(profileName, "gateway", "ERROR", gateway, sizeof(gateway), file);

	GetPrivateProfileString("settings", "MaxLoginTime", "5", _maxLoginTime, sizeof(maxLoginTime), file);
	GetPrivateProfileString("settings", "MaxCharSelectTime", "5", _maxCharTime, sizeof(maxCharTime), file);

	maxLoginTime = abs(atoi(_maxLoginTime) * 1000);
	maxCharTime = abs(atoi(_maxCharTime) * 1000);

	tmp = atoi(difficulty);

	if(tmp < 0 || tmp > 2)
		throw "Invalid difficulty.";

	diff = (char)tmp;

	type = PROFILETYPE_INVALID;

	switch(tolower(mode[0]))
	{
		case 's':
			type = PROFILETYPE_SINGLEPLAYER;
			break;
		case 'b':
			type = PROFILETYPE_BATTLENET;
			break;
		case 'o':
			switch(tolower(mode[18]))
			{
				case 'o':
					type = PROFILETYPE_OPEN_BATTLENET;
					break;
				case 't':
					switch(tolower(mode[25]))
					{
						case 'h':
							type = PROFILETYPE_TCPIP_HOST;
							break;
						case 'j':
							type = PROFILETYPE_TCPIP_JOIN;
							strncpy_s (ip, &mode[30], (size_t)16);
							break;
					}
					break;
			}
			break;
	}
}

bool Profile::ProfileExists(const char *profile)
{
	char file[_MAX_FNAME+_MAX_PATH], profiles[65535] = "";
	sprintf_s(file, sizeof(file), "%sd2bs.ini", Vars.szPath);

	int count = GetPrivateProfileString(NULL, NULL, NULL, profiles, 65535, file);
	if(count > 0)
	{
		int i = 0;
		while(i < count)
		{
			if(_strcmpi(profiles+i, profile) == 0)
				return true;

			i += strlen(profiles+i)+1;
		}
	}
	return false;
}

static BOOL isOtherMP(ProfileType pt)
{
	switch(pt)
	{
		case PROFILETYPE_OPEN_BATTLENET:
		case PROFILETYPE_TCPIP_HOST:
		case PROFILETYPE_TCPIP_JOIN:
			return true;
		default:
			return false;
	}
}

static BOOL isTcpIp(ProfileType pt)
{
	switch(pt)
	{
		case PROFILETYPE_TCPIP_HOST:
		case PROFILETYPE_TCPIP_JOIN:
			return true;
		default:
			return false;
	}
}

DWORD Profile::login(char** error)
{
	//Sleep(10000);
	bool loginComplete = FALSE,	skippedToBnet = TRUE;
	int location = 0;
	char* errorMsg = "";
	Control* pControl = NULL;
	unsigned int timeout = 0;

	/*
		clickedOnce is needed because, when in OOG_OTHER_MULTIPLAYER
		the clickControl () is done twice and the second time it is
		failing because the button is not there anymore.
	*/
	int clickedOnce = false;

	Vars.bBlockKeys = Vars.bBlockMouse = TRUE;

	while(!loginComplete)
	{
		location = OOG_GetLocation();
		switch(location)
		{
			case OOG_D2SPLASH:
				clickControl(*p_D2WIN_FirstControl);
				break;
			
			case OOG_CHAR_SELECT:
				//Sleep(5000);
				if (!OOG_SelectCharacter(charname))
					 errorMsg = "Invalid character name";
				break;
			case OOG_MAIN_MENU:
				if(type == PROFILETYPE_SINGLEPLAYER)
					if(!clickControl(findControl(6, (char *)NULL, -1, 264,324,272,35)))
						 errorMsg = "Failed to click the Single button?";
				if(type == PROFILETYPE_BATTLENET)
				{
					OOG_SelectGateway(gateway, 256);
					if(!clickControl(findControl(6, (char *)NULL, -1, 264, 366, 272, 35)))
						 errorMsg = "Failed to click the 'Battle.net' button?";
				}
				if(isOtherMP(type))
				{
					if(!clickControl(findControl(6, (char *)NULL, -1, 264, 433, 272, 35)))
						errorMsg =  "Failed to click the 'Other Multiplayer' button?";
					else
						skippedToBnet = FALSE;
				}
				break;
			case OOG_LOGIN:
				if((type == PROFILETYPE_SINGLEPLAYER || isOtherMP(type)) && skippedToBnet)
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
				switch(diff)
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
				if (type == PROFILETYPE_OPEN_BATTLENET)
					if(!clickControl(findControl(6, (char *)NULL, -1, 264, 310, 272, 35)))
						errorMsg = "Failed to click the 'Open Battle.net' button?";
				// TCP/IP Game
				if (isTcpIp(type))
					if(!clickControl(findControl(6, (char *)NULL, -1, 264,350,272,35)) && !clickedOnce)
						errorMsg = "Failed to click the 'TCP/IP Game' button?";
					else
						clickedOnce = true;

				break;
			case OOG_TCP_IP:
				if (type == PROFILETYPE_TCPIP_HOST)
					if(!clickControl(findControl(6, (char *)NULL, -1,265,206,272,35)))
						errorMsg = "Failed to click the 'Host Game' button?";
				if (type == PROFILETYPE_TCPIP_JOIN)
					if(!clickControl(findControl(6, (char *)NULL, -1,265,264,272,35)))
						errorMsg = "Failed to click the 'Join Game' button?";
				break;
			case OOG_ENTER_IP_ADDRESS:
				if (_strcmpi(ip, "")) {
					pControl = findControl(1, (char *)NULL, -1, 300, 268, -1, -1);
					if(pControl) {
						SetControlText(pControl, ip);

						// Click the OK button
						//Sleep(5000);
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
			case OOG_CONNECTING:
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
			*error = errorMsg;
			return 2;
		}

		if((timeout*100) > maxLoginTime)
		{
			Vars.bBlockKeys = Vars.bBlockMouse = FALSE;
			*error = "login time out";
			return 1;
		}

		if(ClientState() == ClientStateInGame)
			loginComplete = TRUE;
		
		Sleep(100);
	}

	Vars.bBlockKeys = Vars.bBlockMouse = FALSE;

	return 0;
}