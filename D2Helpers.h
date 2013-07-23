#ifndef D2HELPERS_H
#define D2HELPERS_H

#include "D2Structs.h"
#include "D2Ptrs.h"
#include "js32.h"

enum DistanceType
{
	Euclidean, Chebyshev, Manhattan
};

enum ClientGameState
{
	ClientStateNull, ClientStateMenu, ClientStateInGame, ClientStateBusy
};

void	Log(char* szFormat, ...);

ClientGameState ClientState(void);
bool	GameReady(void);
bool	WaitForGameReady(void);
DWORD	GetPlayerArea(void);
void	SendMouseClick(int x, int y, int clicktype);
POINT	CalculateTextLen(const char* szwText, int Font);
void	WorldToScreen(POINT* pPos);
void	ScreenToWorld(POINT *ptPos);
POINT	ScreenToAutomap(int x, int y);
void	AutomapToScreen(POINT* pPos);
Level*	GetLevel(DWORD dwLevelNo);
void	D2CLIENT_Interact(UnitAny* pUnit, DWORD dwMoveType);
void	myDrawText(const char* szwText, int x, int y, int color, int font);
void	myDrawCenterText(const char* szwText, int x, int y, int color, int font, int div) ;
//UnitAny* FindItemByPosition(DWORD x, DWORD y, DWORD Location);
BYTE	CalcPercent(DWORD dwVal, DWORD dwMaxVal, BYTE iMin = NULL);
DWORD	GetTileLevelNo(Room2* lpRoom2, DWORD dwTileNo);
UnitAny* GetMercUnit(UnitAny* pUnit);

BOOL	ClickNPCMenu(DWORD NPCClassId, DWORD MenuId);
int		GetItemLocation(UnitAny *pItem);
//void	SelectInventoryItem(DWORD x, DWORD y, DWORD dwLocation);

int		GetSkill(WORD wSkillId);
BOOL	SetSkill(JSContext* cx, WORD wSkillId, BOOL bLeft, DWORD dwItemId = 0xFFFFFFFF);
char*	GetSkillByID(WORD id);
WORD	GetSkillByName(char* szSkillName);

const char* GetUnitName(UnitAny* pUnit, char* szBuf, size_t bufSize);
void GetItemCode(UnitAny* pUnit, char* szBuf);

InventoryLayout* GetLayoutFromTable(DWORD dwTable);
UnitAny* D2CLIENT_FindUnit(DWORD dwId, DWORD dwType);

POINT GetScreenSize();
int D2GetScreenSizeX();
int D2GetScreenSizeY();

CellFile* LoadCellFile(char* lpszPath, DWORD bMPQ = 3);

AutomapLayer* InitAutomapLayer(DWORD levelno);
DWORD __fastcall D2CLIENT_InitAutomapLayer_STUB(DWORD nLayerNo);
void myDrawAutomapCell(CellFile *cellfile, int xpos, int ypos, BYTE col);
DWORD ReadFile(HANDLE hFile, void *buf, DWORD len);
void *memcpy2(void *dest, const void *src, size_t count);
HANDLE OpenFileRead(char *filename);
BYTE *AllocReadFile(char *filename);
CellFile *LoadBmpCellFile(BYTE *buf1, int width, int height);
CellFile *LoadBmpCellFile(char *filename);
CellFile *myInitCellFile(CellFile *cf);

static inline UnitAny* GetPlayerUnit() { return D2CLIENT_GetPlayerUnit(); }
static inline void AddRoomData(Room2* room) { D2COMMON_AddRoomData(room->pLevel->pMisc->pAct, room->pLevel->dwLevelNo, room->dwPosX, room->dwPosY, room->pRoom1); }
static inline void RemoveRoomData(Room2* room) { D2COMMON_RemoveRoomData(room->pLevel->pMisc->pAct, room->pLevel->dwLevelNo, room->dwPosX, room->dwPosY, room->pRoom1); }
static inline char* __stdcall GetLevelName(const Level* level) { return D2COMMON_GetLevelText(level->dwLevelNo)->szName; }
static inline char* __stdcall GetLevelIdName(DWORD level) { return D2COMMON_GetLevelText(level)->szName; }


DWORD __fastcall D2CLIENT_GetUnitName_STUB(DWORD UnitAny);
DWORD __fastcall D2CLIENT_GetUIVar_STUB(DWORD varno);
DWORD __fastcall D2CLIENT_LoadUIImage_ASM(char* Path);
void  __fastcall D2CLIENT_SetSelectedUnit_STUB(DWORD UnitAny);
void  __fastcall D2CLIENT_Interact_ASM(DWORD Struct);
DWORD __fastcall D2CLIENT_ClickParty_ASM(DWORD RosterUnit, DWORD Mode);
void  __fastcall D2CLIENT_ClickShopItem_ASM(DWORD x, DWORD y, DWORD BuyOrSell);
void  __fastcall D2CLIENT_ShopAction_ASM(DWORD pTable, DWORD pItem, DWORD pNPC, DWORD _1, DWORD pTable2 /* Could be also the ItemCost?*/, DWORD dwMode, DWORD _2, DWORD _3);
void  __fastcall D2CLIENT_ClickItemRight_ASM(DWORD x, DWORD y, DWORD Location, DWORD pItem, DWORD pItemPath);
void  __fastcall D2CLIENT_ClickBelt(DWORD x, DWORD y, Inventory* pInventoryData);
void  __fastcall D2CLIENT_ClickBeltRight_ASM(DWORD pInventory, DWORD pPlayer, DWORD HoldShift, DWORD dwPotPos);
void  __fastcall D2CLIENT_GetItemDesc_ASM(DWORD pUnit, wchar_t* pBuffer);
void  __fastcall D2COMMON_DisplayOverheadMsg_ASM(DWORD pUnit);
void  __fastcall D2CLIENT_MercItemAction_ASM(DWORD bPacketType, DWORD dwSlot);
DWORD __fastcall D2CLIENT_LoadUIImage_ASM(char* lpszPath);
void __fastcall D2CLIENT_PlaySound(DWORD dwSoundId);
DWORD __fastcall D2CLIENT_TestPvpFlag_STUB(DWORD planum1, DWORD planum2, DWORD flagmask);
void __fastcall D2GFX_DrawRectFrame_STUB(RECT* rect);
DWORD __cdecl D2CLIENT_GetMinionCount(UnitAny* pUnit, DWORD dwType);
void __fastcall D2CLIENT_HostilePartyUnit(RosterUnit* pUnit, DWORD dwButton);
void __stdcall D2CLIENT_TakeWaypoint(DWORD dwWaypointId, DWORD dwArea);
DWORD __fastcall D2CLIENT_SendGamePacket_ASM(DWORD dwLen, BYTE* bPacket);

double GetDistance(long x1, long y1, long x2, long y2, DistanceType type = Euclidean);
bool IsScrollingText();
void ReadProcessBYTES(HANDLE hProcess, DWORD lpAddress, void* buf, int len);
#endif
