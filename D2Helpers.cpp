#include <io.h>
#include <errno.h>
#include <ctime>
#include <cmath>
#include <sstream>
#include <string>

#include "D2Helpers.h"
#include "Constants.h"
#include "Helpers.h"
#include "D2Skills.h"
#include "D2Intercepts.h"
#include "D2BS.h"
#include "stringhash.h"
#include "CriticalSections.h"

void Log(char* szFormat, ...)
{
	va_list vaArgs;

	va_start(vaArgs, szFormat);
	int len = _vscprintf(szFormat, vaArgs);
	char* szString = new char[len+1];
	vsprintf_s(szString, len+1, szFormat, vaArgs);
	va_end(vaArgs);

	LogNoFormat(szString);

	delete[] szString;
}

void LogNoFormat(char* szString)
{
	time_t tTime;
	time(&tTime);
	char szTime[128] = "";
	struct tm time;
	localtime_s(&time, &tTime);
	strftime(szTime, sizeof(szTime), "%x %X", &time);

	char path[_MAX_PATH+_MAX_FNAME] = "";
	sprintf_s(path, sizeof(path), "%sd2bs.log", Vars.szPath);

#ifdef DEBUG
	FILE* log = stderr;
#else
	FILE* log = _fsopen(path, "a+", _SH_DENYNO);
#endif
	fprintf(log, "[%s] D2BS %d: %s\n", szTime, GetProcessId(GetCurrentProcess()), szString);
#ifndef DEBUG
	fflush(log);
	fclose(log);
#endif
}

// NOTE TO CALLERS: szTmp must be a PRE-INITIALIZED string.
const char* GetUnitName(UnitAny* pUnit, char* szTmp, size_t bufSize)
{
	if(!pUnit)
	{
		strcpy_s(szTmp, bufSize, "Unknown");
		return szTmp;
	}
	if(pUnit->dwType == UNIT_MONSTER) {
		wchar_t* wName = D2CLIENT_GetUnitName(pUnit);
		WideCharToMultiByte(CP_ACP, 0, wName, -1, szTmp, bufSize, 0, 0);
		return szTmp;
	}
	if(pUnit->dwType == UNIT_PLAYER && pUnit->pPlayerData)
	{
		//	return pUnit->pPlayerData->szName;
		strcpy_s(szTmp, bufSize, pUnit->pPlayerData->szName);
		return szTmp;
	}
	if(pUnit->dwType == UNIT_ITEM)
	{
		wchar_t wBuffer[256] = L"";
		D2CLIENT_GetItemName(pUnit, wBuffer, sizeof(wBuffer));
		char* szBuffer = UnicodeToAnsi(wBuffer);
		if(strchr(szBuffer, '\n'))
			*strchr(szBuffer,'\n') = 0x00;

		strcpy_s(szTmp, bufSize, szBuffer);
		delete[] szBuffer;
		return szTmp;
	}
	if(pUnit->dwType == UNIT_OBJECT || pUnit->dwType == UNIT_TILE)
	{
		if(pUnit->pObjectData && pUnit->pObjectData->pTxt)
		{
			strcpy_s(szTmp, bufSize, pUnit->pObjectData->pTxt->szName);
			return szTmp;
		}
	}
	strcpy_s(szTmp, bufSize, "Unknown");
	return szTmp;
}

// szBuf must be a 4-character string
void GetItemCode(UnitAny* pUnit, char* szBuf)
{
	if(pUnit->dwType == UNIT_ITEM)
	{
		ItemTxt* pTxt = D2COMMON_GetItemText(pUnit->dwTxtFileNo);
		if(pTxt)
		{
			memcpy(szBuf, pTxt->szCode, 3);
			szBuf[3] = 0x00;
		}
	}
}

bool InArea(int x, int y, int x2, int y2, int sizex, int sizey) {
	return !!(x >= x2 && x < x2+sizex && y >= y2 && y < y2+sizey);
}

/*UnitAny* FindItemByPosition(DWORD x, DWORD y, DWORD Location) {
for(UnitAny* pItem = D2COMMON_GetItemFromInventory(D2CLIENT_GetPlayerUnit()->pInventory); pItem; pItem = D2COMMON_GetNextItemFromInventory(pItem)) {
if((DWORD)GetItemLocation(pItem) == Location && InArea(x,y,pItem->pObjectPath->dwPosX,pItem->pObjectPath->dwPosY,D2COMMON_GetItemText(pItem->dwTxtFileNo)->xSize,D2COMMON_GetItemText(pItem->dwTxtFileNo)->ySize))
return pItem;
}
return NULL;
}*/

/*void SelectInventoryItem(DWORD x, DWORD y, DWORD dwLocation)
{
*(DWORD*)&p_D2CLIENT_SelectedInvItem = (DWORD)FindItemByPosition(x, y, dwLocation);
}*/

// Do not edit without the express consent of bsdunx or lord2800
ClientGameState ClientState(void)
{
	ClientGameState state = ClientStateNull;
	UnitAny* player = D2CLIENT_GetPlayerUnit();
	Control *firstControl = *p_D2WIN_FirstControl;

	if(player && !firstControl)
	{
		if(player && player->pUpdateUnit)
		{
			state = ClientStateBusy;
			return state;
		}
		if(player->pInventory &&
			player->pPath &&
			//player->pPath->xPos &&
			player->pPath->pRoom1 &&
			player->pPath->pRoom1->pRoom2 &&
			player->pPath->pRoom1->pRoom2->pLevel &&
			player->pPath->pRoom1->pRoom2->pLevel->dwLevelNo)
			state = ClientStateInGame;
		else
			state = ClientStateBusy;
	}
	else if(!player && firstControl)
		state = ClientStateMenu;
	else if(!player && !firstControl)
		state = ClientStateNull;

	return state;
}

bool GameReady(void)
{
	return (ClientState() == ClientStateInGame ? true : false);
}

bool WaitForGameReady(void)
{
	DWORD start = GetTickCount();
	do
	{
		switch(ClientState())
		{
		case ClientStateNull: case ClientStateMenu: return false;
		case ClientStateInGame: return true;
		}
		Sleep(10);
	} while((Vars.dwGameTimeout == 0 ) || (Vars.dwGameTimeout > 0 && (GetTickCount() - start) < Vars.dwGameTimeout));
	return false;
}

DWORD GetPlayerArea(void)
{
	return (ClientState() == ClientStateInGame ? D2CLIENT_GetPlayerUnit()->pPath->pRoom1->pRoom2->pLevel->dwLevelNo : NULL);
}

Level* GetLevel(DWORD dwLevelNo)
{
	AutoCriticalRoom* cRoom = new AutoCriticalRoom;
	
	if (!GameReady())
		return nullptr;

	Level* pLevel = D2CLIENT_GetPlayerUnit()->pAct->pMisc->pLevelFirst;

    while(pLevel)
    {
        if(pLevel->dwLevelNo == dwLevelNo)
        {
            if (!pLevel->pRoom2First)
                D2COMMON_InitLevel(pLevel);

            if (!pLevel->pRoom2First)
                break;
			delete cRoom;
            return pLevel;
        }
        pLevel = pLevel->pNextLevel;
    }

	// this crashes pretty much every time it's called
	//pLevel = D2COMMON_GetLevel(D2CLIENT_GetPlayerUnit()->pAct->pMisc, dwLevelNo);
	delete cRoom;
	return pLevel;

}

// TODO: make this use SIZE for clarity
POINT CalculateTextLen(const char* szwText, int Font)
{
	POINT ret = {0,0};

	if(!szwText)
		return ret;

	wchar_t* Buffer = AnsiToUnicode(szwText);

	DWORD dwWidth, dwFileNo;
	DWORD dwOldSize = D2WIN_SetTextSize(Font);
	ret.y = D2WIN_GetTextSize(Buffer, &dwWidth, &dwFileNo);
	ret.x = dwWidth;
	D2WIN_SetTextSize(dwOldSize);

	delete[] Buffer;
	return ret;
}

int GetSkill(WORD wSkillId)
{
	if(!D2CLIENT_GetPlayerUnit()) return 0;

	for(Skill* pSkill = D2CLIENT_GetPlayerUnit()->pInfo->pFirstSkill; pSkill; pSkill = pSkill->pNextSkill)
		if(pSkill->pSkillInfo->wSkillId == wSkillId)
			return D2COMMON_GetSkillLevel(D2CLIENT_GetPlayerUnit(), pSkill, TRUE);

	return 0;
}

BOOL SetSkill(JSContext* cx, WORD wSkillId, BOOL bLeft, DWORD dwItemId)
{
	if(ClientState() != ClientStateInGame) 
		return FALSE;

	if(!GetSkill(wSkillId))
		return FALSE;

	BYTE aPacket[9];

	aPacket[0] = 0x3C;
	*(WORD*)&aPacket[1] = wSkillId;
	aPacket[3] = 0;
	aPacket[4] = (bLeft) ? 0x80 : 0;
	*(DWORD*)&aPacket[5] = dwItemId;

	D2CLIENT_SendGamePacket(9, aPacket);

	UnitAny* Me = D2CLIENT_GetPlayerUnit();

	int timeout = 0;
	Skill* hand = NULL;
	while(ClientState() == ClientStateInGame)
	{
		hand = (bLeft ? Me->pInfo->pLeftSkill : Me->pInfo->pRightSkill);
		if(hand->pSkillInfo->wSkillId != wSkillId)
		{
			if(timeout > 10)
				return FALSE;
			timeout++;
		}
		else
			return TRUE;

	Script* script = (Script*)JS_GetContextPrivate(cx);  // run events to avoid packet block deadlock
	DWORD start = GetTickCount();
	int amt = 100 -(GetTickCount() - start);

		while(amt > 0 )  
		{	// had a script deadlock here, make sure were positve with amt		
			WaitForSingleObjectEx(script->eventSignal, amt, true);
			ResetEvent(script->eventSignal);
			while(script->EventList.size() > 0 && !!!(JSBool)(script->IsAborted() || ((script->GetState() == InGame) && ClientState() == ClientStateMenu)))
			{
				EnterCriticalSection(&Vars.cEventSection);
					Event* evt = script->EventList.back();
					script->EventList.pop_back();
				LeaveCriticalSection(&Vars.cEventSection);				
				ExecScriptEvent(evt,false);				
			}			
			amt = 100 -(GetTickCount() - start);
			//SleepEx(10,true);	// ex for delayed setTimer
		}		
	}

	return FALSE;
}

// Compare the skillname to the Game_Skills struct to find the right skill ID to return
WORD GetSkillByName(char* skillname)
{
	for(int i = 0; i < 216; i++)
		if(_stricmp(Game_Skills[i].name, skillname) == 0)
			return Game_Skills[i].skillID;
	return (WORD)-1;
}

char* GetSkillByID(WORD id)
{
	for(int i = 0; i < 216; i++)
		if(id == Game_Skills[i].skillID)
			return Game_Skills[i].name;
	return NULL;
}

void SendMouseClick(int x, int y, int clicktype)
{
	// HACK: Using PostMessage instead of SendMessage--need to fix this ASAP!
	LPARAM lp = x + (y << 16);
	switch(clicktype)
	{
	case 0:
		PostMessage(D2GFX_GetHwnd(), WM_LBUTTONDOWN, 0, lp);
		break;
	case 1:
		PostMessage(D2GFX_GetHwnd(), WM_LBUTTONUP, 0, lp);
		break;
	case 2:
		PostMessage(D2GFX_GetHwnd(), WM_RBUTTONDOWN, 0, lp);
		break;
	case 3:
		PostMessage(D2GFX_GetHwnd(), WM_RBUTTONUP, 0, lp);
		break;
	}
}

DWORD __declspec(naked) __fastcall D2CLIENT_InitAutomapLayer_STUB(DWORD nLayerNo)
{
	__asm 
	{
		push edi; //Updated 1.14d Registers changed.
		mov edi, ecx;
		call D2CLIENT_InitAutomapLayer_I;
		pop edi;
		ret;
	}
}

AutomapLayer* InitAutomapLayer(DWORD levelno)
{
	AutomapLayer2 *pLayer = D2COMMON_GetLayer(levelno);
	return D2CLIENT_InitAutomapLayer(pLayer->nLayerNo);
}

void WorldToScreen(POINT* pPos)
{
	D2COMMON_MapToAbsScreen(&pPos->x, &pPos->y);
	pPos->x -= D2CLIENT_GetMouseXOffset();
	pPos->y -= D2CLIENT_GetMouseYOffset();
}

void ScreenToWorld( POINT *pPos)
{
	D2COMMON_AbsScreenToMap(&pPos->x, &pPos->y);
	pPos->x += D2CLIENT_GetMouseXOffset();
	pPos->y += D2CLIENT_GetMouseYOffset();
}

POINT ScreenToAutomap(int x, int y)
{
	POINT result = {0,0};
	x *= 32;
	y *= 32;
	result.x = ((x - y) / 2 / (*p_D2CLIENT_Divisor)) - (*p_D2CLIENT_Offset).x + 8;
	result.y = ((x + y) / 4 / (*p_D2CLIENT_Divisor)) - (*p_D2CLIENT_Offset).y - 8;

	if (D2CLIENT_GetAutomapSize())
	{
		--result.x;
		result.y += 5;
	}
	return result;
}

void AutomapToScreen(POINT* pPos)
{
	pPos->x = 8 - p_D2CLIENT_Offset->x + (pPos->x * (*p_D2CLIENT_AutomapMode));
	pPos->y = 8 + p_D2CLIENT_Offset->y + (pPos->y * (*p_D2CLIENT_AutomapMode));
}

void myDrawText(const char* szwText, int x, int y, int color, int font) 
{
	size_t found;
	wchar_t* text = AnsiToUnicode(szwText);
	std::string temp(szwText);
	found=temp.find_first_of(-1);
  
	while (found!=std::string::npos)
    {
       text[found] = 0xff;
       found=temp.find_first_of(-1,found+1);
    }
	

	DWORD dwOld = D2WIN_SetTextSize(font);
	D2WIN_DrawText(text, x, y, color, 0);
	D2WIN_SetTextSize(dwOld);

	delete[] text;
}


void myDrawCenterText(const char* szText, int x, int y, int color, int font, int div) 
{
	DWORD dwWidth = NULL, dwFileNo = NULL, dwOldSize = NULL;
	wchar_t* Buffer = AnsiToUnicode(szText);

	dwOldSize = D2WIN_SetTextSize(font);
	D2WIN_GetTextSize(Buffer, &dwWidth, &dwFileNo);
	D2WIN_SetTextSize(dwOldSize);
	myDrawText(szText,x-(dwWidth >> div),y,color,font);

	delete[] Buffer;
}

void D2CLIENT_Interact(UnitAny* pUnit, DWORD dwMoveType) {

	if(!pUnit)
		return;

	if(!D2CLIENT_FindUnit(pUnit->dwUnitId,pUnit->dwType))
		return;

	UnitInteraction pInteract = {
		dwMoveType,
		D2CLIENT_GetPlayerUnit(),
		pUnit,
		D2CLIENT_GetUnitX(pUnit),
		D2CLIENT_GetUnitY(pUnit),
		0, 0
	};

	D2CLIENT_Interact_STUB(&pInteract);
}

typedef void (*fnClickEntry) (void);

BOOL ClickNPCMenu(DWORD NPCClassId, DWORD MenuId)
{
	NPCMenu* pMenu = (NPCMenu*)p_D2CLIENT_NPCMenu;
	fnClickEntry pClick = (fnClickEntry) NULL;

	for(UINT i = 0; i < *p_D2CLIENT_NPCMenuAmount; i++)
	{
		if(pMenu->dwNPCClassId == NPCClassId)
		{
			if(pMenu->wEntryId1 == MenuId)
			{
				pClick = (fnClickEntry)pMenu->dwEntryFunc1;
				if(pClick)
					pClick();
				else return FALSE;
				return TRUE;
			}
			else if(pMenu->wEntryId2 == MenuId)
			{
				pClick = (fnClickEntry)pMenu->dwEntryFunc2;
				if(pClick)
					pClick();
				else return FALSE;
				return TRUE;
			}
			else if(pMenu->wEntryId3 == MenuId)
			{
				pClick = (fnClickEntry)pMenu->dwEntryFunc3;
				if(pClick)
					pClick();
				else return FALSE;
				return TRUE;
			}
			else if(pMenu->wEntryId4 == MenuId)
			{
				pClick = (fnClickEntry)pMenu->dwEntryFunc4;
				if(pClick)
					pClick();
				else return FALSE;
				return TRUE;
			}
		}
		pMenu = (NPCMenu*) ((DWORD)pMenu + sizeof(NPCMenu));
	}

	return FALSE;
}

int GetItemLocation(UnitAny *pItem)
{
	if(!pItem || !pItem->pItemData)
		return -1;

	return (pItem->pItemData->GameLocation);
}

BYTE CalcPercent(DWORD dwVal, DWORD dwMaxVal, BYTE iMin)
{
	if(dwVal == 0 || dwMaxVal == 0)
		return 0;

	BYTE iRes = (BYTE)((double)dwVal / (double)dwMaxVal * 100.0);
	if (iRes > 100)
		iRes = 100;

	return max(iRes, iMin);
}


DWORD GetTileLevelNo(Room2* lpRoom2, DWORD dwTileNo)
{
	for(RoomTile* pRoomTile = lpRoom2->pRoomTiles; pRoomTile; pRoomTile = pRoomTile->pNext)
	{
		if(*(pRoomTile->nNum) == dwTileNo)
			return pRoomTile->pRoom2->pLevel->dwLevelNo;
	}

	return NULL;
}

UnitAny* GetMercUnit(UnitAny* pUnit)
{
	for(Room1* pRoom = pUnit->pAct->pRoom1; pRoom; pRoom = pRoom->pRoomNext)
		for(UnitAny* pMerc = pRoom->pUnitFirst; pMerc; pMerc = pMerc->pRoomNext)
			if(pMerc->dwType == UNIT_MONSTER &&
				(pMerc->dwTxtFileNo == MERC_A1 || pMerc->dwTxtFileNo == MERC_A2 ||
				pMerc->dwTxtFileNo == MERC_A3 || pMerc->dwTxtFileNo == MERC_A5) &&
				D2CLIENT_GetMonsterOwner(pMerc->dwUnitId) == pUnit->dwUnitId)
				return pMerc;
	return NULL;

#if 0
	// Wanted way of doing things, but D2CLIENT_GetMercUnit does some wierd internal things (drawing, causing screen flicker)
	for(UnitAny* pMerc = D2CLIENT_GetMercUnit(); pMerc; pMerc = pMerc->pRoomNext)
		if (D2CLIENT_GetMonsterOwner(pMerc->dwUnitId) == pUnit->dwUnitId)
			return pMerc;
	return NULL;
#endif
}

UnitAny* D2CLIENT_FindUnit(DWORD dwId, DWORD dwType)
{
	if(dwId == -1) return NULL;
	UnitAny* pUnit = D2CLIENT_FindServerSideUnit(dwId, dwType);
	return pUnit ? pUnit : D2CLIENT_FindClientSideUnit(dwId, dwType);
}

// TODO: Rewrite this and split it into two functions

CellFile* LoadCellFile(char* lpszPath, DWORD bMPQ)
{
	// AutoDetect the Cell File
	if(bMPQ == 3)
	{
		// Check in our directory first
		char path[_MAX_FNAME+_MAX_PATH];
		sprintf_s(path, sizeof(path), "%s\\%s", Vars.szScriptPath, lpszPath);


		HANDLE hFile = OpenFileRead(path);

		if(hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			return LoadCellFile(path, FALSE);
		}
		else
		{
			return LoadCellFile(lpszPath, TRUE);
		}

		//return NULL;
	}

	unsigned __int32 hash = sfh(lpszPath, (int)strlen(lpszPath));
	if(Vars.mCachedCellFiles.count(hash) > 0)
		return Vars.mCachedCellFiles[hash];
	if(bMPQ == TRUE)
	{
		CellFile* result = (CellFile*)D2WIN_LoadCellFile(lpszPath, 0);		
		Vars.mCachedCellFiles[hash] = result;
		return result;
	}
	else if(bMPQ == FALSE)
	{
		// see if the file exists first
		if(!(_access(lpszPath, 0) != 0 && errno == ENOENT))
		{
			CellFile* result = myInitCellFile((CellFile*)LoadBmpCellFile(lpszPath));
			Vars.mCachedCellFiles[hash] = result;
			return result;
		}
	}

	return NULL;
}

POINT GetScreenSize()
{
	// HACK: p_D2CLIENT_ScreenSize is wrong for out of game, which is hardcoded to 800x600
	POINT ingame = {*p_D2CLIENT_ScreenSizeX, *p_D2CLIENT_ScreenSizeY},
		oog = {800, 600},
		p = {0};
	if(ClientState() == ClientStateMenu) p = oog;
	else p = ingame;
	return p;
}

int D2GetScreenSizeX()
{
	return GetScreenSize().x;
}

int D2GetScreenSizeY()
{
	return GetScreenSize().y;
}

void myDrawAutomapCell(CellFile *cellfile, int xpos, int ypos, BYTE col)
{
	if(!cellfile)return;
	CellContext ct;
	memset(&ct, 0, sizeof(ct));
	ct.pCellFile = cellfile;

	xpos -= (cellfile->cells[0]->width/2);
	ypos += (cellfile->cells[0]->height/2);

	int xpos2 = xpos - cellfile->cells[0]->xoffs, ypos2 = ypos - cellfile->cells[0]->yoffs;
	if ((xpos2 >= D2GetScreenSizeX()) || ((xpos2 + (int)cellfile->cells[0]->width) <= 0) || (ypos2 >= D2GetScreenSizeY()) || ((ypos2 + (int)cellfile->cells[0]->height) <= 0)) return;

	static BYTE coltab[2][256];//, tabno = 0, lastcol = 0;
	if (!coltab[0][1]) for (int k = 0; k < 255; k++) coltab[0][k] = coltab[1][k] = (BYTE)k;
	cellfile->mylastcol = coltab[cellfile->mytabno ^= (col != cellfile->mylastcol)][255] = col;

	D2GFX_DrawAutomapCell2(&ct, xpos, ypos, (DWORD)-1, 5, coltab[cellfile->mytabno]);
}

DWORD ReadFile(HANDLE hFile, void *buf, DWORD len)
//NOTE :- validates len bytes of buf
{
	DWORD numdone = 0;
	return ::ReadFile(hFile, buf, len, &numdone, NULL) != 0 ? numdone : -1;
}

void *memcpy2(void *dest, const void *src, size_t count)
{
	return (char *)memcpy(dest, src, count)+count;
}

HANDLE OpenFileRead(char *filename)
{
	return CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

BYTE *AllocReadFile(char *filename)
{
	HANDLE hFile = OpenFileRead(filename);
	int filesize = GetFileSize(hFile, 0);
	if (filesize <= 0) return 0;
	BYTE *buf = new BYTE[filesize];
	ReadFile(hFile, buf, filesize);
	CloseHandle(hFile);
	return buf;
}

CellFile *LoadBmpCellFile(BYTE *buf1, int width, int height)
{
	BYTE *buf2 = new BYTE[(width*height*2)+height], *dest = buf2;

	for (int i = 0; i < height; i++) {
		BYTE *src = buf1+(i*((width+3)&-4)), *limit = src+width;
		while (src < limit) {
			BYTE *start = src, *limit2 = min(limit, src+0x7f), trans = !*src;
			do src++; while ((trans == (BYTE)!*src) && (src < limit2));
			if (!trans || (src < limit)) *dest++ = (BYTE)((trans?0x80:0)+(src-start));
			if (!trans) while (start < src) *dest++ = *start++;
		}
		*dest++ = 0x80;
	}

	static DWORD dc6head[] = {6, 1, 0, 0xeeeeeeee, 1, 1, 0x1c,  0, (DWORD)-1, (DWORD)-1, 0, 0, 0, (DWORD)-1, (DWORD)-1};
	dc6head[8] = width;
	dc6head[9] = height;
	dc6head[14] = dest - buf2;
	dc6head[13] = sizeof(dc6head)+(dc6head[14])+3;
	BYTE *ret = new BYTE[dc6head[13]];
	memset(memcpy2(memcpy2(ret, dc6head, sizeof(dc6head)), buf2, dc6head[14]), 0xee, 3);
	delete[] buf2;

	return (CellFile *)ret;
}

CellFile *LoadBmpCellFile(char *filename)
{
	BYTE *ret = 0;

	BYTE *buf1 = AllocReadFile(filename);
	BITMAPFILEHEADER *bmphead1 = (BITMAPFILEHEADER *)buf1;
	BITMAPINFOHEADER *bmphead2 = (BITMAPINFOHEADER *)(buf1+sizeof(BITMAPFILEHEADER));
	if (buf1 && (bmphead1->bfType == 'MB') && (bmphead2->biBitCount == 8) && (bmphead2->biCompression == BI_RGB)) {
		ret = (BYTE *)LoadBmpCellFile(buf1+bmphead1->bfOffBits, bmphead2->biWidth, bmphead2->biHeight);
	}
	delete[] buf1;

	return (CellFile *)ret;
}

CellFile *myInitCellFile(CellFile *cf)
{
	if(cf)
		D2CMP_InitCellFile(cf, &cf, "?", 0, (DWORD)-1, "?");
	return cf;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// End of Sting's or Mousepad's
///////////////////////////////////////////////////////////////////////////////////////////////////



DWORD __declspec(naked) __fastcall D2CLIENT_GetUnitName_STUB(DWORD UnitAny)
{
	__asm
	{
		//mov eax, ecx
			jmp D2CLIENT_GetUnitName_I
	}
}

DWORD __declspec(naked) __fastcall D2CLIENT_GetUIVar_STUB(DWORD varno)
{
	__asm 
	{
		mov eax, ecx;
		jmp D2CLIENT_GetUiVar_I;
	}
}

void __declspec(naked) __fastcall D2CLIENT_SetSelectedUnit_STUB(DWORD UnitAny)
{
	__asm
	{
		mov eax, ecx
			jmp D2CLIENT_SetSelectedUnit_I
	}
}
//DWORD __declspec(naked) __fastcall D2CLIENT_LoadUIImage_ASM(char* Path) 
//{
//	__asm {
//		mov eax, ecx
//			push 0
//			call D2CLIENT_LoadUIImage_I
//			retn
//	}
//}

void __declspec(naked) __fastcall D2CLIENT_Interact_ASM(DWORD Struct)
{
	__asm {
		mov esi, ecx
			jmp D2CLIENT_Interact_I
	}
}

DWORD __declspec(naked) __fastcall D2CLIENT_ClickParty_ASM(DWORD RosterUnit, DWORD Mode)
{
	__asm
	{
		mov eax, ecx
			jmp D2CLIENT_ClickParty_I
	}
}

// obsoleted - use D2CLIENT_ShopAction instead
// This isn't finished anyway!
void __declspec(naked) __fastcall D2CLIENT_ClickShopItem_ASM(DWORD x, DWORD y, DWORD BuyOrSell)
{
	__asm
	{
		mov esi, ecx
			mov edi, edx
			pop eax // Save return address to eax
			pop ecx // Buy or sell to ecx
			push ecx
			push 1
			push eax
			jmp D2CLIENT_ClickShopItem_I
	}
}

void __declspec(naked) __fastcall D2CLIENT_ShopAction_ASM(DWORD pItem, DWORD pNpc, DWORD pNPC, DWORD _1, DWORD pTable2 /* Could be also the ItemCost?*/, DWORD dwMode, DWORD _2, DWORD _3)
{
	__asm {
		jmp D2CLIENT_ShopAction_I
	}
}

void __declspec(naked) __fastcall D2CLIENT_ClickBelt(DWORD x, DWORD y, Inventory* pInventoryData)
{
	__asm {
		mov eax, edx
			jmp D2CLIENT_ClickBelt_I
	}
}

void __declspec(naked) __stdcall D2CLIENT_LeftClickItem(DWORD Location, UnitAny* pPlayer, Inventory* pInventory, int x, int y, DWORD dwClickType, InventoryLayout* pLayout)
{
	__asm
	{
		pop eax // pop return address
		xchg eax, [esp] // return address to stack, location to eax
		jmp D2CLIENT_LeftClickItem_I
	}
}

void __declspec(naked) __fastcall D2CLIENT_ClickItemRight_ASM(DWORD x, DWORD y, DWORD Location, DWORD Player, DWORD pUnitInventory)
{
	__asm
	{
		xchg edx, ecx // x, y -> y, x
		pop eax // pop return address
		xchg eax, [esp] // return address to stack, location to eax
		jmp D2CLIENT_ClickItemRight_I
	}
}

void __declspec(naked) __fastcall D2CLIENT_ClickBeltRight_ASM(DWORD pInventory, DWORD pPlayer, DWORD HoldShift, DWORD dwPotPos)
{
	__asm
	{
		mov eax, [esp + 4]
		push eax
		mov eax, [esp + 8 + 4]
		call D2CLIENT_ClickBeltRight_I
		retn 8
	}
}

void __declspec(naked) __fastcall D2CLIENT_GetItemDesc_ASM(DWORD pUnit, wchar_t* pBuffer)
{
	__asm 
	{
		PUSH EDI
			MOV EDI, EDX
			PUSH NULL
			PUSH 1		// TRUE = New lines, FALSE = Comma between each stat value
			PUSH ECX
			CALL D2CLIENT_GetItemDesc_I
			POP EDI
			RETN
	}
}

void __declspec(naked) __fastcall D2COMMON_DisplayOverheadMsg_ASM(DWORD pUnit)
{
	__asm
	{
		LEA ESI, [ECX+0xA4]
		MOV EAX, [ECX+0xA4]

		PUSH EAX
			PUSH 0
			call D2COMMON_DisplayOverheadMsg_I

			RETN
	}
}

void __declspec(naked) __fastcall D2CLIENT_MercItemAction_ASM(DWORD bPacketType, DWORD dwSlot)
{
	__asm 
	{
		//mov eax, ecx
			//mov ecx, edx
			jmp D2CLIENT_MercItemAction_I
	}
}

void __declspec(naked) __fastcall D2CLIENT_PlaySound(DWORD dwSoundId)
{
	__asm
	{
		MOV EBX, ECX
			PUSH NULL
			PUSH NULL
			PUSH NULL
			MOV EAX, p_D2CLIENT_PlayerUnit
			MOV EAX, [EAX]
		MOV ECX, EAX
			MOV EDX, EBX
			//CALL D2CLIENT_PlaySound_I
			RETN
	}
}

__declspec(naked) void __stdcall D2CLIENT_TakeWaypoint(DWORD dwWaypointId, DWORD dwArea)
{
	__asm
	{
		PUSH EBP
			MOV EBP, ESP
			SUB ESP, 0x20
			PUSH EBX
			PUSH ESI
			PUSH EDI
			AND DWORD PTR SS:[EBP-0x20],0
			PUSH 0
			CALL _TakeWaypoint
			POP EDI
			POP ESI
			POP EBX
			LEAVE
			RETN 8

_TakeWaypoint:
		PUSH EBP
			PUSH EBX
			PUSH ESI
			PUSH EDI

			XOR EDI, EDI
			MOV EBX, 1
			MOV EDX, DWORD PTR SS : [EBP + 8] // waypointId
			MOV ECX, DWORD PTR SS : [EBP + 0xC] // area
			PUSH ECX
			LEA ESI, DWORD PTR SS : [EBP - 0x20] // struct
		JMP [D2CLIENT_TakeWaypoint_I]

	}
}
/*DWORD __declspec(naked) __fastcall D2CLIENT_TestPvpFlag_STUB(DWORD planum1, DWORD planum2, DWORD flagmask)
{
	__asm 
	{
		push esi;
		push [esp+8];
		mov esi, edx; // p2
		mov edx, ecx; // p1
		call D2CLIENT_TestPvpFlag_I;
		pop esi;
		ret 4;
	}
}*/

void __declspec(naked) __fastcall D2GFX_DrawRectFrame_STUB(RECT* rect)
{
	__asm
	{
		mov eax, ecx;
		jmp D2CLIENT_DrawRectFrame;
	}
}

DWORD __cdecl D2CLIENT_GetMinionCount(UnitAny* pUnit, DWORD dwType)
{
	DWORD dwResult;

	__asm
	{
		push eax
			push ecx
			push edx
			mov ecx, pUnit
			mov edx, dwType
			call D2CLIENT_GetMinionCount_I
			mov [dwResult], eax
			pop edx
			pop ecx
			pop eax
	}

	return dwResult;
}

__declspec(naked) void __fastcall D2CLIENT_HostilePartyUnit(RosterUnit* pUnit, DWORD dwButton)
{
	__asm
	{
		mov eax, edx
			jmp [D2CLIENT_ClickParty_II]
	}
}

__declspec(naked) DWORD __fastcall D2CLIENT_SendGamePacket_ASM(DWORD dwLen, BYTE* bPacket)
{
	__asm
	{
		push edi
			mov edi, ecx
			push edx
			call D2CLIENT_SendGamePacket_I
			pop edi
			ret
	}
}

double GetDistance(long x1, long y1, long x2, long y2, DistanceType type)
{
	double dist = 0;
	switch(type)
	{
	case Euclidean:
		{
			double dx = (double)(x2 - x1);
			double dy = (double)(y2 - y1);
			dx = pow(dx, 2);
			dy = pow(dy, 2);
			dist = sqrt(dx + dy); 
		}
		break;
	case Chebyshev:
		{
			long dx = (x2 - x1);
			long dy = (y2 - y1);
			dx = abs(dx);
			dy = abs(dy);
			dist = max(dx, dy); 
		}
		break;
	case Manhattan:
		{
			long dx = (x2 - x1);
			long dy = (y2 - y1);
			dx = abs(dx);
			dy = abs(dy);
			dist = (dx + dy);
		}
		break;
	default: 
		dist = -1; 
		break;
	}
	return dist;
}

bool IsScrollingText()
{

	if(!WaitForGameReady())
		return false;

	HWND d2Hwnd = D2GFX_GetHwnd();
	WindowHandlerList* whl = p_STORM_WindowHandlers->table[(0x534D5347^(DWORD)d2Hwnd)%p_STORM_WindowHandlers->length];
	MessageHandlerHashTable* mhht;
	MessageHandlerList* mhl;

	while(whl)
	{
		if(whl->unk_0 == 0x534D5347 && whl->hWnd == d2Hwnd)
		{
			mhht = whl->msgHandlers;
			if(mhht != NULL && mhht->table != NULL && mhht->length != 0)
			{
				// 0x201 - WM_something click
				mhl = mhht->table[0x201 % mhht->length];

				if(mhl != NULL)
				{
					while(mhl)
					{
						if(mhl->message && mhl->unk_4 < 0xffffffff && mhl->handler == D2CLIENT_CloseNPCTalk)
						{
							return true;
						}
						mhl = mhl->next;
					}
				}
			}
		}
		whl = whl->next;
	}

	return false;
}

void ReadProcessBYTES(HANDLE hProcess, DWORD lpAddress, void* buf, int len)
{
   DWORD oldprot, dummy = 0;
   VirtualProtectEx(hProcess, (void*) lpAddress, len, PAGE_READWRITE, &oldprot);
   ReadProcessMemory(hProcess, (void*) lpAddress, buf, len, 0);
   VirtualProtectEx(hProcess, (void*) lpAddress, len, oldprot, &dummy);
}