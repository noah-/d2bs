#pragma once

#include "D2Structs.h"
#include "Constants.h"

Control* findControl(int Type, int LocaleID, int Disabled, int PosX, int PosY, int SizeX, int SizeY);
Control* findControl(int Type, const wchar_t* Text, int Disabled, int PosX, int PosY, int SizeX, int SizeY);
bool clickControl(Control* pControl, int x = -1, int y = -1);
BOOL OOG_CreateCharacter(const wchar_t* szCharacter, int type, bool hardcore, bool ladder);
BOOL OOG_SelectCharacter(const wchar_t* szCharacter);
BOOL OOG_SelectGateway(const wchar_t* szGateway, size_t strSize);
void SetControlText(Control* pControl, const wchar_t* Text);
OOG_Location OOG_GetLocation(void);
bool OOG_CreateGame(const wchar_t* name, const wchar_t* pass, int difficulty);
bool OOG_JoinGame(const wchar_t* name, const wchar_t* pass);
