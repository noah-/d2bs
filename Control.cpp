#include "Control.h"
#include "D2Ptrs.h"
#include "D2Helpers.h"
#include "Helpers.h"
#include "Constants.h"

Control* findControl(int Type, int LocaleID, int Disabled, int PosX, int PosY, int SizeX, int SizeY)
{
	if(ClientState() != ClientStateMenu)
		return NULL;

	char* localeStr = NULL; 
	if(LocaleID >= 0)
	{
		localeStr = UnicodeToAnsi(D2LANG_GetLocaleText((WORD)LocaleID));
		if(!localeStr)
			return NULL;
		Control* res = findControl(Type, localeStr, Disabled, PosX, PosY, SizeX, SizeY);
		delete[] localeStr;
		return res;
	}

	return NULL;
}

Control* findControl(int Type, char* Text, int Disabled, int PosX, int PosY, int SizeX, int SizeY)
{
	if(ClientState() != ClientStateMenu)
		return NULL;

	if(Type == -1 && Text == NULL && Disabled == -1 && PosX == -1 && PosY == -1 && SizeX == -1 && SizeY == -1)
		return *p_D2WIN_FirstControl;

	BOOL bFound = FALSE;

	for(Control* pControl = *p_D2WIN_FirstControl; pControl; pControl = pControl->pNext)
	{
		if(Type >= 0 && static_cast<int>(pControl->dwType) == Type)
			bFound = TRUE;
		else if(Type >= 0 && static_cast<int>(pControl->dwType) != Type)
		{
			bFound = FALSE;
			continue;
		}

		if(Disabled >= 0 && static_cast<int>(pControl->dwDisabled) == Disabled)
		{
			if(pControl->dwType == CONTROL_BUTTON && pControl->unkState == 1)
			{
				bFound = FALSE;
				continue;
			}
			bFound = TRUE;
		}
		else if(Disabled >= 0 && static_cast<int>(pControl->dwDisabled) != Disabled)
		{			
			bFound = FALSE;
			continue;
		}

		if(PosX >= 0 && static_cast<int>(pControl->dwPosX) == PosX)
			bFound = TRUE;
		else if(PosX >= 0 && static_cast<int>(pControl->dwPosX) != PosX)
		{
			bFound = FALSE;
			continue;
		}

		if(PosY >= 0 && static_cast<int>(pControl->dwPosY) == PosY)
			bFound = TRUE;
		else if(PosY >= 0 && static_cast<int>(pControl->dwPosY) != PosY)
		{
			bFound = FALSE;
			continue;
		}

		if(SizeX >= 0 && static_cast<int>(pControl->dwSizeX) == SizeX)
			bFound = TRUE;
		else if(SizeX >= 0 && static_cast<int>(pControl->dwSizeX) != SizeX)
		{
			bFound = FALSE;
			continue;
		}

		if(SizeY >= 0 && static_cast<int>(pControl->dwSizeY) == SizeY)
			bFound = TRUE;
		else if(SizeY >= 0 && static_cast<int>(pControl->dwSizeY) != SizeY)
		{
			bFound = FALSE;
			continue;
		}

		if(Text && pControl->dwType == CONTROL_BUTTON)
		{
			char* text2 = UnicodeToAnsi(pControl->wText2);
			if(!text2)
				return NULL;
			if(strcmp(text2, Text) == 0)
			{
				bFound = TRUE;
				delete[] text2;
			}
			else
			{
				bFound = FALSE;
				delete[] text2;
				continue;
			}
		}

		if(Text && pControl->dwType == CONTROL_TEXTBOX)
		{
			if(pControl->pFirstText != NULL && pControl->pFirstText->wText != NULL)
			{
				char* text2 = UnicodeToAnsi(pControl->pFirstText->wText);
				if(!text2)
					return NULL;
				if(strstr(Text, text2) != 0)
				{
					bFound = TRUE;
					delete[] text2;
				}
				else
				{
					bFound = FALSE;
					delete[] text2;
					continue;
				}
			}
			else
			{
				bFound = FALSE;
				continue;
			}
		}
		if(bFound)
			return pControl;
	}

	return NULL;
}

bool clickControl(Control* pControl, int x, int y)
{
	if(ClientState() != ClientStateMenu)
		return false;

	if(pControl)
	{
		if(x == -1)
			x = pControl->dwPosX + (pControl->dwSizeX / 2);
		if(y == -1)
			y = pControl->dwPosY - (pControl->dwSizeY / 2);
		
		Sleep(100);
		SendMouseClick(x, y, 0);
		Sleep(100);
		SendMouseClick(x, y, 1);
		Sleep(100);
		
		return true;
	}
	return false;
}

BOOL OOG_CreateCharacter(char* szCharacter, int type, bool hardcore, bool ladder)
{
	if(OOG_GetLocation() != OOG_CHAR_SELECT || strlen(szCharacter) > 15 || type > 6 || type < 0)
		return FALSE;

	// click the create character button
	Control* ctrl = findControl(CONTROL_BUTTON, (char*)NULL, -1, 33, 528, 168, 60);
	if(ctrl)
		clickControl(ctrl);

	// TODO: replace with real time checking code
	int i = 0;
	while((OOG_GetLocation() != OOG_CHARACTER_CREATE) && i++ < 30)
		Sleep(100);

	int locs[7][5] = {
		{100, 337, 80,  330, 4011}, // amazon
		{626, 353, 600, 300, 4010}, // sorceress
		{301, 333, 300, 330, 4009}, // necromancer
		{521, 339, 500, 330, 4008}, // paladin
		{400, 330, 390, 330, 4007}, // barbarian
		{720, 370, 700, 370, 4012}, // druid
		{232, 364, 200, 300, 4013}, // assassin
	};

	ctrl = findControl(CONTROL_IMAGE, (char*)NULL, -1, locs[type][0], locs[type][1], 88, 184);
	if(ctrl)
	{
		clickControl(ctrl, locs[type][2], locs[type][3]);
		Sleep(500);
		clickControl(ctrl, locs[type][2], locs[type][3]);
	}

	// verify that the correct type got selected
	ctrl = findControl(CONTROL_TEXTBOX, (char*)NULL, -1, 0, 180, 800, 100);
	wchar_t* name = D2LANG_GetLocaleText((WORD)locs[type][4]);
	if(_wcsicmp(name, ctrl->pFirstText->wText) != 0)
		return FALSE; // something bad happened?

	// set the name
//	ctrl = findControl();
	// still need to find the name editbox, set the name, and click the various
	// checkboxes and the ok button

	return FALSE;
} 

BOOL OOG_SelectCharacter(char* szCharacter)
{
	if(ClientState() != ClientStateMenu)
		return NULL;

	// Select the first control on the character selection screen.
	Control* pControl = findControl(CONTROL_TEXTBOX, (char *)NULL, -1, 237, 178, 72, 93);
	ControlText* cText;

	while (pControl != NULL)
	{
		if(pControl->dwType == CONTROL_TEXTBOX && pControl->pFirstText != NULL && pControl->pFirstText->pNext != NULL)
			cText = pControl->pFirstText->pNext;
		else
			cText = NULL;

		if(cText != NULL)
		{
			char * szLine = UnicodeToAnsi(cText->wText);
			if(!szLine)
				return FALSE;
			if(strlen(szLine) == strlen(szCharacter) && strstr(szLine,szCharacter) != NULL)
			{
				delete[] szLine;
				if(!clickControl(pControl))
					return FALSE;

				// OK Button
				pControl = findControl(CONTROL_BUTTON, (char *)NULL, -1, 627, 572, 128, 35);
				if(pControl)
				{
					if(!clickControl(pControl))
						return FALSE;

					return TRUE;
				}
				else
					return FALSE;

			}
			else
				delete[] szLine;
		}
		pControl = pControl->pNext;
	}
	return FALSE;
}

void SetControlText(Control* pControl, const char* Text)
{
	if(ClientState() != ClientStateMenu)
		return;

	if(pControl && Text)
	{
		wchar_t* szwText = AnsiToUnicode(Text);
		if(!szwText)
			return;
		D2WIN_SetControlText(pControl, szwText);
		delete[] szwText;
	}
}

BOOL OOG_SelectGateway(char * szGateway, size_t strSize)
{
	if(ClientState() != ClientStateMenu)
		return FALSE;

	if(strstr(szGateway,"ERROR"))
		return FALSE;
	// Select the gateway control.
	Control* pControl = findControl(CONTROL_BUTTON, (char *)NULL, -1, 264, 391, 272, 25);

	// if the control exists and has the text label, check if it matches the selected gateway
	if(pControl && pControl->wText2)
	{
		char* szLine = UnicodeToAnsi(pControl->wText2);
		if(!szLine)
			return FALSE;

		_strlwr_s(szLine, strlen(szLine)+1);
		_strlwr_s(szGateway, strSize);
		if(strstr(szLine, szGateway))
		{
			// gateway is correct, do nothing and return true
			delete[] szLine;
			return TRUE;
		}
		else
		{
			delete[] szLine;
			// gateway is NOT correct, change gateway to selected gateway if it exists
			// open the gateway select screen
			if(!clickControl(pControl))
				return FALSE;

			int index = 0;
			bool gatefound = false;

			// loop here till we find the right gateway if we can
			pControl = findControl(CONTROL_TEXTBOX, (char *)NULL, -1, 257, 500, 292, 160);
			ControlText* cText;
			if(pControl && pControl->pFirstText)
			{
				cText = pControl->pFirstText;
				while(cText)
				{
					char * szGatelist = UnicodeToAnsi(cText->wText);
					if(!szGatelist)
						return FALSE;

					_strlwr_s(szGatelist, strlen(szGatelist)+1);
					if(strstr(szGatelist, szGateway))
					{
						// chosen gateway IS in the list and matches, cleanup and break the loop
						gatefound = true;
						delete[] szGatelist;
						break;
					}
					delete[] szGatelist;
					index++;
					cText = cText->pNext;
				}
				if(gatefound)
				{
				// click the correct gateway using the control plus a default x and a y based on (index*24)+12
					if(!clickControl(pControl, -1, 344+((index*24)+12)))
						return FALSE;
				}
			}

			// OK Button, gateway select screen
			pControl = findControl(CONTROL_BUTTON, (char *)NULL, -1, 281, 538, 96, 32);
			if(pControl)
			{
				if(!clickControl(pControl))
					return FALSE;
			}
			else
				return FALSE;

			return TRUE;
		}
	}
	return FALSE;
}

OOG_Location OOG_GetLocation(void)
{
	if(ClientState() != ClientStateMenu)
		return OOG_NONE;

	if(findControl(CONTROL_BUTTON, 5103, -1, 330, 416, 128, 35))
		return OOG_MAIN_MENU_CONNECTING;					//21 Connecting to Battle.net	
	else if(findControl(CONTROL_BUTTON, 5102, -1, 335, 412, 128, 35))
		return OOG_LOGIN_ERROR;								//10 Login Error	
	else if (findControl(CONTROL_BUTTON, 5102, -1, 351,337,96,32))	//5102 =OK
	{
		if (findControl(CONTROL_TEXTBOX, 5351, -1, 268,320,264,120))	
			return OOG_LOST_CONNECTION;						//17 lost connection	
		else if (findControl(CONTROL_TEXTBOX, 5347, -1, 268,320,264,120))
			return OOG_DISCONNECTED;						//14  Disconnected
		else
			return OOG_CHARACTER_CREATE_ALREADY_EXISTS;		//30 Character Create - Dupe Name									
	}
	else if(findControl(CONTROL_BUTTON, 5103, -1, 351,337,96,32))		//5103 = CANCEL
	{
		if(findControl(CONTROL_TEXTBOX, 5243, -1, 268, 300, 264, 100))
			return OOG_CHARACTER_SELECT_PLEASE_WAIT;		//16 char select please wait...
		if (findControl(CONTROL_TEXTBOX, (char *)NULL, -1, 268,320,264,120))
			return OOG_PLEASE_WAIT;							//25 "Please Wait..."single player already exists also
	}	
	else if(findControl(CONTROL_BUTTON, 5103, -1, 433, 433, 96, 32))
	{ 
		if (findControl(CONTROL_TEXTBOX, (char *)NULL, -1, 427,234,300,100))
			return OOG_INLINE;								//2 waiting in line	
		else if(findControl(CONTROL_TEXTBOX, 10018, -1, 459, 380, 150, 12))
			return OOG_CREATE;								//4 Create game
		else if(findControl(CONTROL_BUTTON, 5119, -1, 594, 433, 172, 32))
			return OOG_JOIN;								// 5 Join Game
		else if(findControl(CONTROL_BUTTON, 5102, -1, 671, 433, 96, 32))
			return OOG_CHANNEL;								//7 "Channel List"
		else
			return OOG_LADDER;								//6 "Ladder"		
	}		
	else if(findControl(CONTROL_BUTTON, 5101, -1, 33,572,128,35))		//5101 = EXIT
	{
		if(findControl(CONTROL_BUTTON, 5288, -1, 264, 484, 272, 35))
			return OOG_LOGIN;								//9 Login
		if (findControl(CONTROL_BUTTON, 5102, -1, 495,438,96,32))
			return OOG_CHARACTER_SELECT_CHANGE_REALM;		//43 char select change realm						
		if(findControl(CONTROL_BUTTON, 5102, -1, 627,572,128,35) && findControl(CONTROL_BUTTON, 10832, -1, 33,528,168,60)) //10832=create new
		{
			if (findControl(CONTROL_BUTTON, 10018, -1, 264,297,272,35)) //NORMAL
				return OOG_DIFFICULTY;						//20 single char Difficulty
			Control* pControl = findControl(CONTROL_TEXTBOX, (char *)NULL, -1, 37, 178, 200, 92);
			if(pControl && pControl->pFirstText && pControl->pFirstText->pNext)
				return OOG_CHAR_SELECT;						//12 char select
			else
			{
				if (findControl(CONTROL_TEXTBOX, 11162, -1,45,318,531,140) || findControl(CONTROL_TEXTBOX, 11066, -1,45,318,531,140))	
					return OOG_REALM_DOWN;
				// Look for CONNECTING... string
				else if (findControl(CONTROL_TEXTBOX, 11065, -1, 0x2D, 0x13E, 0x213, 0x8C))
					return OOG_CONNECTING;
				else
					return OOG_CHARACTER_SELECT_NO_CHARS;	//42 char info not loaded 
			}
		}
		if(findControl(CONTROL_BUTTON, 5101, -1, 33, 572, 128, 35))		//5101=Exit
		{
			if (findControl(CONTROL_BUTTON, 5102, 0, 627,572,128,35))			//5102=ok
				return OOG_CHARACTER_CREATE;				//29 char create screen with char selected
			else
			{
				if(findControl(CONTROL_TEXTBOX, 5226, -1, 321, 448, 300, 32))
					return OOG_NEW_ACCOUNT;					//32 create new bnet account
				else
					return OOG_NEW_CHARACTER;				//15 char create screen no char selected
			}
		}
	}
	if(findControl(CONTROL_BUTTON, 5102, -1, 335, 450, 128, 35))
	{
		if(findControl(CONTROL_TEXTBOX, 5200, -1, 162, 270, 477, 50))
			return OOG_CDKEY_IN_USE;						//19 CD-KEY in use
		else if (findControl(CONTROL_TEXTBOX, 5190, -1, 162,420,477,100))		 //5190="If using a modem"
			return OOG_UNABLE_TO_CONNECT;					//11 unable to connect
		else		
			return OOG_INVALID_CDKEY;						//22 invalid CD-KEY 		
	}	
	else if (findControl(CONTROL_TEXTBOX, 5159, -1, 438, 300, 326, 150)) 
		return OOG_GAME_DOES_NOT_EXIST;						//28 game doesn't exist
	else if (findControl(CONTROL_TEXTBOX, 5161, -1, 438, 300, 326, 150)) 
		return OOG_GAME_IS_FULL;							//38 Game is full
	else if (findControl(CONTROL_TEXTBOX, 5138, -1, 438, 300, 326, 150)) 
		return OOG_GAME_EXIST;								//26 Game already exists
	else if (findControl(CONTROL_TEXTBOX, 5139, -1, 438, 300, 326, 150)) 
		return OOG_SERVER_DOWN;								//24 server down	
	else if (findControl(CONTROL_BUTTON, 5106, -1, 264,324,272,35))				//5106="SINGLE PLAYER"
		return OOG_MAIN_MENU;								//8 Main Menu
	else if (findControl(CONTROL_BUTTON, 11126, -1, 27,480,120,20))					//11126=ENTER CHAT
		return OOG_LOBBY;									//1 base bnet 
	else if (findControl(CONTROL_BUTTON, 5308, -1, 187,470,80,20))					//5308="HELP"
		return OOG_CHAT;									//3 chat bnet 
	else if(findControl(CONTROL_TEXTBOX, 21882, -1, 100, 580, 600, 80))
		return OOG_D2SPLASH;								//18 Spash	
	else if (findControl(CONTROL_BUTTON, 5102, -1, 281,538,96,32))
		return OOG_GATEWAY;									//27 select gateway
	else if (findControl(CONTROL_BUTTON, 5181, -1, 525,513,128,35))
		return OOG_AGREE_TO_TERMS;							//31 agree to terms
	else if (findControl(CONTROL_BUTTON, 5102, -1, 525,513,128,35))
		return OOG_PLEASE_READ;								//33 please read
	else if (findControl(CONTROL_BUTTON, 11097, -1, 265,527,272,35))
		return OOG_REGISTER_EMAIL;							//34 register email
	else if (findControl(CONTROL_BUTTON, 5101, -1, 33,578,128,35))
		return OOG_CREDITS;									//35 Credits
	else if (findControl(CONTROL_BUTTON, 5103, -1, 334,488,128,35))
		return OOG_CINEMATICS;								//36 Cinematics
	else if (findControl(CONTROL_BUTTON, 5116, -1, 264,350,272,35))
		return	OOG_OTHER_MULTIPLAYER;						//39 other multi player
	else if (findControl(CONTROL_BUTTON, 5103, -1,281,337,96,32))
		return OOG_ENTER_IP_ADDRESS;						//41 enter ip	
	else if (findControl(CONTROL_BUTTON, 5118, -1,265,206,272,35))
		return	OOG_TCP_IP;									//40 tcp-ip
	
	return OOG_NONE;
}

bool OOG_CreateGame(const char* name, const char* pass, int difficulty)
{		
	if(ClientState() != ClientStateMenu)
		return FALSE;

	// reject name/password combinations over 15 characters
	if(!name || !pass || strlen(name) > 15 || strlen(pass) > 15)
		return FALSE;

	Control* pControl = NULL;

	// Battle.net/open game creation
	OOG_Location loc = OOG_GetLocation();
	if(!(loc == OOG_LOBBY || loc == OOG_CHAT || loc == OOG_DIFFICULTY || loc == OOG_CREATE))
		return FALSE;

	if(loc == OOG_DIFFICULTY)
	{
		// just click the difficulty button
		Control *normal = findControl(CONTROL_BUTTON, (char *)NULL, -1, 264, 297, 272, 35),
				*nightmare = findControl(CONTROL_BUTTON, (char *)NULL, -1, 264, 340, 272, 35),
				*hell = findControl(CONTROL_BUTTON, (char *)NULL, -1, 264, 383, 272, 35);

		switch(difficulty)
		{
			case 0: // normal button
				if(normal->dwDisabled != 0x0d || !clickControl(normal))
					return FALSE;
				break;
			case 1: // nightmare button
				if(nightmare->dwDisabled != 0x0d || !clickControl(nightmare))
					return FALSE;
				break;
			case 2: // hell button
				if(hell->dwDisabled != 0x0d || !clickControl(hell))
					return FALSE;
				break;
			case 3: //hardest difficulty available
				if(hell->dwDisabled != 0x0d) {
					if(!clickControl(hell))
						return FALSE;
				} else if(nightmare->dwDisabled != 0x0d) {
					if(!clickControl(nightmare))
						return FALSE;
				} else if(normal->dwDisabled != 0x0d) {
					if(!clickControl(normal))
						return FALSE;
				}
				break;
			default:
				return FALSE;
		}
	}
	else
	{
		// Create button
		if (loc != OOG_CREATE)
		{
			pControl = findControl(CONTROL_BUTTON, (char *)NULL, -1, 533,469,120,20);
			if(!pControl || !clickControl(pControl))
				return FALSE;
			Sleep(100);
		}
		if(OOG_GetLocation() == OOG_CREATE)
		{
			// Game name edit box
			if(name)
				SetControlText(findControl(1, (char *)NULL, -1, 432,162,158,20), name);
			else
				return FALSE;
			Sleep(100);

			// Password edit box
			if(pass)
				SetControlText(findControl(1, (char *)NULL, -1, 432,217,158,20), pass);
			else
				return FALSE;
			Sleep(100);
			Control *normal = findControl(CONTROL_BUTTON, (char *)NULL, -1, 430,381,16,16),
					*nightmare = findControl(CONTROL_BUTTON, (char *)NULL, -1, 555,381,16,16),
					*hell = findControl(CONTROL_BUTTON, (char *)NULL, -1, 698,381,16,16);

			switch(difficulty)
			{
				case 0: // normal button
					if(normal->dwDisabled == 0x4 || !clickControl(normal))
						return FALSE;
					break;
				case 1: // nightmare button
					if(nightmare->dwDisabled == 0x4 || !clickControl(nightmare))
						return FALSE;
					break;
				case 2: // hell button
					if(hell->dwDisabled == 0x4 || !clickControl(hell))
						return FALSE;
					break;
				case 3: //hardest difficulty available
					if(hell->dwDisabled != 0x4) {
						if(!clickControl(hell))
							return FALSE;
					} 
					else if(nightmare->dwDisabled != 0x4) {
						if(!clickControl(nightmare))
							return FALSE;
					} 
					else if(normal->dwDisabled != 0x4) {
						if(!clickControl(normal))
							return FALSE;
					}
					break;
				default:
					return FALSE;
			}

			// Create Game Button
			pControl = findControl(CONTROL_BUTTON, (char *)NULL, -1, 594,433,172,32);
			if(!pControl || !clickControl(pControl))
				return FALSE;
		}
	}

	return TRUE;
}

bool OOG_JoinGame(const char* name, const char* pass)
{		
	if(ClientState() != ClientStateMenu)
		return FALSE;

	// reject name/password combinations over 15 characters
	if(strlen(name) > 15 || strlen(pass) > 15)
		return FALSE;

	Control* pControl = NULL;

	// Battle.net/open lobby/chat area
	if(!(OOG_GetLocation() == OOG_LOBBY || OOG_GetLocation() == OOG_CHAT || OOG_GetLocation() == OOG_JOIN))
		return FALSE;

	// JOIN button
	if (OOG_GetLocation() != OOG_JOIN)
	{
		pControl = findControl(CONTROL_BUTTON, (char *)NULL, -1, 652,469,120,20);
		if(!pControl || !clickControl(pControl))
			return FALSE;
		Sleep(100);
	}
	if(OOG_GetLocation() == OOG_JOIN)
	{
		// Game name edit box
		if(name)
			SetControlText(findControl(1, (char *)NULL, -1, 432,148,155,20), name);
		else
			return FALSE;
		// Password edit box
		if(pass)
			SetControlText(findControl(1, (char *)NULL, -1, 606,148,155,20), pass);
		else
			return FALSE;

		// Join Game Button
		pControl = findControl(CONTROL_BUTTON, (char *)NULL, -1, 594,433,172,32);
		if(!pControl || !clickControl(pControl))
			return FALSE;
	}

	return TRUE;
}
