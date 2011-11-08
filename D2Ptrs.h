#pragma once
#ifndef __D2PTRS_H__
#define __D2PTRS_H__

#include "D2Structs.h"

#pragma warning ( push )
#pragma warning ( disable: 4245 )

#pragma optimize ( "", off )

#ifdef _DEFINE_VARS

enum {DLLNO_D2CLIENT, DLLNO_D2COMMON, DLLNO_D2GFX, DLLNO_D2LANG, DLLNO_D2WIN, DLLNO_D2NET, DLLNO_D2GAME, DLLNO_D2LAUNCH, DLLNO_FOG, DLLNO_BNCLIENT, DLLNO_STORM, DLLNO_D2CMP, DLLNO_D2MULTI};

#define DLLOFFSET(a1,b1) ((DLLNO_##a1)|((b1)<<8))
#define FUNCPTR(d1,v1,t1,t2,o1)	typedef t1 d1##_##v1##_t t2; d1##_##v1##_t *d1##_##v1 = (d1##_##v1##_t *)DLLOFFSET(d1,o1);
#define VARPTR(d1,v1,t1,o1)		typedef t1 d1##_##v1##_t;    d1##_##v1##_t *p_##d1##_##v1 = (d1##_##v1##_t *)DLLOFFSET(d1,o1);
#define ASMPTR(d1,v1,o1)			DWORD d1##_##v1 = DLLOFFSET(d1,o1);

#else

#define FUNCPTR(d1,v1,t1,t2,o1)	typedef t1 d1##_##v1##_t t2; extern d1##_##v1##_t *d1##_##v1;
#define VARPTR(d1,v1,t1,o1)		typedef t1 d1##_##v1##_t;    extern d1##_##v1##_t *p_##d1##_##v1;
#define ASMPTR(d1,v1,o1)			extern DWORD d1##_##v1;

#endif
#define _D2PTRS_START	D2CLIENT_GetQuestInfo

FUNCPTR(D2CLIENT, GetQuestInfo, void* __stdcall, (void), 0x78A80)

FUNCPTR(D2CLIENT, SubmitItem, void __fastcall, (DWORD dwItemId), 0x79670)
FUNCPTR(D2CLIENT, Transmute, void __fastcall, (void), 0x94370)

FUNCPTR(D2CLIENT, FindClientSideUnit, UnitAny* __fastcall, (DWORD dwId, DWORD dwType), 0x620B0)
FUNCPTR(D2CLIENT, FindServerSideUnit, UnitAny* __fastcall, (DWORD dwId, DWORD dwType), 0x620D0)
FUNCPTR(D2CLIENT, GetCurrentInteractingNPC, UnitAny* __fastcall, (void), 0x790D0)
FUNCPTR(D2CLIENT, GetSelectedUnit, UnitAny * __stdcall, (void), 0x17280)
FUNCPTR(D2CLIENT, GetCursorItem, UnitAny* __fastcall, (void), 0x144A0)
FUNCPTR(D2CLIENT, GetMercUnit, UnitAny* __fastcall, (void), 0x9C0A0)
//FUNCPTR(D2CLIENT, UnitTestSelect, DWORD __stdcall, (UnitAny* pUnit, DWORD _1, DWORD _2, DWORD _3), 0x8D030) // unused but we need to use it

FUNCPTR(D2CLIENT, SetSelectedUnit_I, void __fastcall, (UnitAny *pUnit), 0x17060)
FUNCPTR(D2CLIENT, GetItemName, BOOL __stdcall, (UnitAny* pItem, wchar_t* wBuffer, DWORD dwSize), 0x958C0)
FUNCPTR(D2CLIENT, GetMonsterOwner, DWORD __fastcall, (DWORD nMonsterId), 0x8E3D0)
FUNCPTR(D2CLIENT, GetUnitHPPercent, DWORD __fastcall, (DWORD dwUnitId), 0x8E2B0)
FUNCPTR(D2CLIENT, InitInventory, void __fastcall, (void), 0x93280)
FUNCPTR(D2CLIENT, SetUIVar, DWORD __fastcall, (DWORD varno, DWORD howset, DWORD unknown1), 0x1C190) 
FUNCPTR(D2CLIENT, GetUnitX, int __fastcall, (UnitAny* pUnit), 0x1210)
FUNCPTR(D2CLIENT, GetUnitY, int __fastcall, (UnitAny* pUnit), 0x1240)

FUNCPTR(D2CLIENT, ShopAction, void __fastcall, (UnitAny* pItem, UnitAny* pNpc, UnitAny* pNpc2, DWORD dwSell, DWORD dwItemCost, DWORD dwMode, DWORD _2, DWORD _3), 0x7D030)

FUNCPTR(D2CLIENT, CloseNPCInteract, void __fastcall, (void), 0x7BC10)
//FUNCPTR(D2CLIENT, ClearScreen, void __fastcall, (void), 0x492F0) // unused but I want to look into using it // wrong function
FUNCPTR(D2CLIENT, CloseInteract, void __fastcall, (void), 0x44980)

FUNCPTR(D2CLIENT, GetAutomapSize, DWORD __stdcall, (void), 0x6FDD0)
FUNCPTR(D2CLIENT, NewAutomapCell, AutomapCell * __fastcall, (void), 0x703C0)
FUNCPTR(D2CLIENT, AddAutomapCell, void __fastcall, (AutomapCell *aCell, AutomapCell **node), 0x71EA0)
FUNCPTR(D2CLIENT, RevealAutomapRoom, void __stdcall, (Room1 *pRoom1, DWORD dwClipFlag, AutomapLayer *aLayer), 0x73160)
FUNCPTR(D2CLIENT, InitAutomapLayer_I, AutomapLayer* __fastcall, (DWORD nLayerNo), 0x733D0)

FUNCPTR(D2CLIENT, ClickMap, void __stdcall, (DWORD MouseFlag, DWORD x, DWORD y, DWORD Type), 0x2B420)
FUNCPTR(D2CLIENT, LeftClickItem, void __stdcall, (UnitAny* pPlayer, Inventory* pInventory, int x, int y, DWORD dwClickType, InventoryLayout* pLayout, DWORD Location), 0x9AFF0)

FUNCPTR(D2CLIENT, GetMouseXOffset, DWORD __fastcall, (void), 0x5BC20)
FUNCPTR(D2CLIENT, GetMouseYOffset, DWORD __fastcall, (void), 0x5BC30)

FUNCPTR(D2CLIENT, PrintPartyString, void __stdcall, (wchar_t *wMessage, int nColor), 0x75C70) // unused but I want to look into using it too
FUNCPTR(D2CLIENT, PrintGameString, void __stdcall, (wchar_t *wMessage, int nColor), 0x75EB0)

FUNCPTR(D2CLIENT, LeaveParty, void __fastcall, (void), 0xA26A0)

FUNCPTR(D2CLIENT, AcceptTrade, void __fastcall, (void), 0x11F70)
FUNCPTR(D2CLIENT, CancelTrade, void __fastcall, (void), 0x11F30)

FUNCPTR(D2CLIENT, GetDifficulty, BYTE __stdcall, (void), 0x42980)

FUNCPTR(D2CLIENT, ExitGame, void __fastcall, (void), 0x43870)

FUNCPTR(D2CLIENT, GetUiVar_I, DWORD __fastcall, (DWORD dwVarNo), 0x17C50)

FUNCPTR(D2CLIENT, DrawRectFrame, void __fastcall, (DWORD Rect), 0x17D10)

FUNCPTR(D2CLIENT, PerformGoldDialogAction, void __fastcall, (void), 0x197F0)

FUNCPTR(D2CLIENT, GetPlayerUnit, UnitAny* __stdcall, (void), 0x613C0)

FUNCPTR(D2CLIENT, GetLevelName_I, wchar_t* __fastcall, (DWORD levelId), 0x18250)

FUNCPTR(D2CLIENT, ClearScreen, void __fastcall, (void), 0x7AB80)

FUNCPTR(D2CLIENT, CloseNPCTalk, DWORD __stdcall, (void* unk), 0x77AB0)


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Client Globals
////////////////////////////////////////////////////////////////////////////////////////////////

VARPTR(D2CLIENT, ScreenSizeX, DWORD, 0xF7034)
VARPTR(D2CLIENT, ScreenSizeY, DWORD, 0xF7038)

VARPTR(D2CLIENT, CursorHoverX, DWORD, 0xEE4AC)
VARPTR(D2CLIENT, CursorHoverY, DWORD, 0xEE4B0)

VARPTR(D2CLIENT, MouseX, DWORD, 0x11C950)
VARPTR(D2CLIENT, MouseY, DWORD, 0x11C94C)

VARPTR(D2CLIENT, MouseOffsetY, int, 0x106840)
VARPTR(D2CLIENT, MouseOffsetX, int, 0x106844)

VARPTR(D2CLIENT, AutomapOn, DWORD, 0x11C8B8)
VARPTR(D2CLIENT, AutomapMode, int, 0xF34F8)
VARPTR(D2CLIENT, Offset, POINT, 0x11CF5C)
VARPTR(D2CLIENT, AutomapLayer, AutomapLayer*, 0x11CF28)

VARPTR(D2CLIENT, MercStrIndex, WORD, 0xF02D8)
VARPTR(D2CLIENT, MercReviveCost, DWORD, 0x11CEE8)

VARPTR(D2CLIENT, ServerSideUnitHashTables, UnitHashTable, 0x1047B8)
VARPTR(D2CLIENT, ClientSideUnitHashTables, UnitHashTable, 0x103BB8)

VARPTR(D2CLIENT, ViewportY, int, 0x106840)
VARPTR(D2CLIENT, ViewportX, int, 0x106844)

VARPTR(D2CLIENT, GoldDialogAction, DWORD, 0x11C86C)
VARPTR(D2CLIENT, GoldDialogAmount, DWORD, 0x11D568)

VARPTR(D2CLIENT, NPCMenu, NPCMenu*, 0xF1A90)
VARPTR(D2CLIENT, NPCMenuAmount, DWORD, 0xF21E0)

VARPTR(D2CLIENT, TradeLayout, InventoryLayout*, 0x101598)
VARPTR(D2CLIENT, StashLayout, InventoryLayout*, 0x1015E0)
VARPTR(D2CLIENT, StoreLayout, InventoryLayout*, 0x1016C0)
VARPTR(D2CLIENT, CubeLayout, InventoryLayout*, 0x1016D8)
VARPTR(D2CLIENT, InventoryLayout, InventoryLayout*, 0x1016F0)
VARPTR(D2CLIENT, MercLayout, InventoryLayout*, 0x11CC84)

ASMPTR(D2CLIENT, clickParty_I, 0xA2250)

VARPTR(D2CLIENT, RegularCursorType, DWORD, 0x11C98C)
VARPTR(D2CLIENT, ShopCursorType, DWORD, 0x11CB24)


VARPTR(D2CLIENT, Ping, DWORD, 0x108764)
VARPTR(D2CLIENT, FPS, DWORD, 0x11CE10)
VARPTR(D2CLIENT, Skip, DWORD, 0x108770)
VARPTR(D2CLIENT, Divisor, int, 0xF34F8)

VARPTR(D2CLIENT, OverheadTrigger, DWORD, 0x101ABE)

VARPTR(D2CLIENT, RecentInteractId, DWORD, 0x101895)

VARPTR(D2CLIENT, ItemPriceList, DWORD, 0x1018B3)

VARPTR(D2CLIENT, TransactionDialog, void*, 0x1018D3)
VARPTR(D2CLIENT, TransactionDialogs, DWORD, 0x11D58C)
VARPTR(D2CLIENT, TransactionDialogs_2, DWORD, 0x11D588)

VARPTR(D2CLIENT, GameInfo, GameStructInfo*, 0x109738)

VARPTR(D2CLIENT, WaypointTable, DWORD, 0x1088FD)

VARPTR(D2CLIENT, PlayerUnit, UnitAny*, 0x11D050)
VARPTR(D2CLIENT, SelectedInvItem, UnitAny*, 0x11CB28)
//VARPTR(D2CLIENT, SelectedUnit, UnitAny*, 0x11C4D8) // unused, but can we use it somewhere maybe? // 1.12 still
VARPTR(D2CLIENT, PlayerUnitList, RosterUnit*, 0x11CB04)

VARPTR(D2CLIENT, bWeapSwitch, DWORD, 0x11CB84)

VARPTR(D2CLIENT, bTradeAccepted, DWORD, 0x11CD54)
VARPTR(D2CLIENT, bTradeBlock, DWORD, 0x11CD64)

//VARPTR(D2CLIENT, RecentTradeName, wchar_t*, 0x12334C)
VARPTR(D2CLIENT, RecentTradeId, DWORD, 0x11D5AC)

VARPTR(D2CLIENT, ExpCharFlag, DWORD, 0x1087B4)

VARPTR(D2CLIENT, MapId, DWORD, 0x11D204) // unused but I want to add it

VARPTR(D2CLIENT, AlwaysRun, DWORD, 0x11D234)
VARPTR(D2CLIENT, NoPickUp, DWORD, 0x11D574) // unused but I want to add it

//VARPTR(D2CLIENT, ScreenCovered, DWORD, 0x1E8F9) // unused, appears to be an int specifying which screens (if any) are opened...

VARPTR(D2CLIENT, ChatMsg, wchar_t*, 0x11D650)


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Client Stubs
////////////////////////////////////////////////////////////////////////////////////////////////

ASMPTR(D2CLIENT, TakeWaypoint_I, 0x3F5F3)

ASMPTR(D2CLIENT, ClickShopItem_I, 0x7A180)
ASMPTR(D2CLIENT, ClickBelt_I, 0x6E310)
ASMPTR(D2CLIENT, ClickBeltRight_I, 0x6FA40)
ASMPTR(D2CLIENT, ClickItemRight_I, 0x9CF30)
ASMPTR(D2CLIENT, MercItemAction_I, 0xB6B00)

ASMPTR(D2CLIENT, Interact_I, 0x2B2E0)

ASMPTR(D2CLIENT, ClickParty_I, 0xA2250)
ASMPTR(D2CLIENT, ClickParty_II, 0x88A50)

ASMPTR(D2CLIENT, ShopAction_I, 0x7D030)

ASMPTR(D2CLIENT, GetUnitName_I, 0x622E0)
ASMPTR(D2CLIENT, GetItemDesc_I, 0x2E380)

ASMPTR(D2CLIENT, AssignPlayer_I, 0x63C70)

ASMPTR(D2CLIENT, TestPvpFlag_I, 0x6A720)

ASMPTR(D2CLIENT, InputCall_I, 0xB6890)

ASMPTR(D2CLIENT, Say_I, 0xB27A6)

ASMPTR(D2CLIENT, BodyClickTable, 0xEE4B8)

ASMPTR(D2CLIENT, LoadUIImage_I, 0xA93E0)

ASMPTR(D2CLIENT, GetMinionCount_I, 0x8E5B0)

ASMPTR(D2CLIENT, GameLeave_I, 0xB4370)

ASMPTR(D2CLIENT, ClickMap_I, 0x11C8B4)
ASMPTR(D2CLIENT, ClickMap_II, 0x11D2CC)
ASMPTR(D2CLIENT, ClickMap_III, 0x5BB50)
ASMPTR(D2CLIENT, ClickMap_IV, 0x2B499)

ASMPTR(D2CLIENT, GameAddUnit_I, 0x628E0)

ASMPTR(D2CLIENT, LoadAct_1, 0x737F0)
ASMPTR(D2CLIENT, LoadAct_2, 0x2B420)

ASMPTR(D2CLIENT, GetUnitFromId_I, 0x61480)
VARPTR(D2CLIENT, pUnitTable, POINT, 0x1047B8)

ASMPTR(D2CLIENT, OverrideShrinePatch_ORIG, 0x101B08)

ASMPTR(D2CLIENT, SendGamePacket_I, 0xB61F0)

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Common Ordinals
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2COMMON, InitLevel, void __stdcall, (Level *pLevel), 0x6DDF0)
FUNCPTR(D2COMMON, UnloadAct, unsigned __stdcall, (Act* pAct), 0x24590)
FUNCPTR(D2COMMON, GetObjectTxt, ObjectTxt * __stdcall, (DWORD objno), 0x1ADC0)
FUNCPTR(D2COMMON, LoadAct, Act* __stdcall, (DWORD ActNumber, DWORD MapId, DWORD Unk, DWORD Unk_2, DWORD Unk_3, DWORD Unk_4, DWORD TownLevelId, DWORD Func_1, DWORD Func_2), 0x24810)

FUNCPTR(D2COMMON, GetLevelText, LevelTxt * __stdcall, (DWORD levelno), 0x30CA0)
FUNCPTR(D2COMMON, GetObjectText, ObjectTxt * __stdcall, (DWORD objno), 0x1ADC0)
FUNCPTR(D2COMMON, GetItemText, ItemTxt *__stdcall, (DWORD dwItemNo), 0x62C70)

FUNCPTR(D2COMMON, GetLayer, AutomapLayer2* __fastcall, (DWORD dwLevelNo), 0x30B00)
FUNCPTR(D2COMMON, GetLevel, Level * __fastcall, (ActMisc *pMisc, DWORD dwLevelNo), 0x6D440)

FUNCPTR(D2COMMON, GetStatList, StatList* __stdcall, (UnitAny* pUnit, DWORD dwUnk, DWORD dwMaxEntries ), 0x57830)
FUNCPTR(D2COMMON, CopyStatList, DWORD __stdcall, (StatList* pStatList, Stat* pStatArray, DWORD dwMaxEntries), 0x57D30)
FUNCPTR(D2COMMON, GetUnitStat, long __stdcall, (UnitAny* pUnit, DWORD dwStat, DWORD dwStat2), 0x584E0)
FUNCPTR(D2COMMON, GetUnitState, int __stdcall, (UnitAny *pUnit, DWORD dwStateNo), 0x2F310)

FUNCPTR(D2COMMON, CheckUnitCollision, DWORD __stdcall, (UnitAny* pUnitA, UnitAny* pUnitB, WORD dwBitMask), 0x17CF0)
FUNCPTR(D2COMMON, GetRoomFromUnit,  Room1* __stdcall, (UnitAny * ptUnit), 0x16530)
FUNCPTR(D2COMMON, GetTargetUnitType, Path* __stdcall, (Path* pPath), 0x773C0)

FUNCPTR(D2COMMON, GetSkillLevel, int __stdcall, (UnitAny* pUnit, Skill* pSkill, BOOL bTotal), 0x73D60)

FUNCPTR(D2COMMON, GetItemLevelRequirement, DWORD __stdcall, (UnitAny* pItem, UnitAny* pPlayer), 0x45660)

FUNCPTR(D2COMMON, GetItemPrice, DWORD __stdcall, (UnitAny* MyUnit, UnitAny* pItem, DWORD U1_,DWORD U2_,DWORD U3_,DWORD U4_), 0x48620)
FUNCPTR(D2COMMON, GetRepairCost, DWORD __stdcall, (DWORD _1, UnitAny* pUnit, DWORD dwNpcId, DWORD dwDifficulty, DWORD dwItemPriceList, DWORD _2), 0x48AD0)
FUNCPTR(D2COMMON, GetItemMagicalMods, char* __stdcall, (WORD wPrefixNum), 0x62AF0)
FUNCPTR(D2COMMON, GetItemFromInventory, UnitAny *__stdcall, (Inventory* inv), 0x37A00)
FUNCPTR(D2COMMON, GetNextItemFromInventory, UnitAny *__stdcall, (UnitAny *pItem), 0x38160)

FUNCPTR(D2COMMON, GenerateOverheadMsg, OverheadMsg* __stdcall, (DWORD dwUnk, char* szMsg, DWORD dwTrigger), 0x4DD10)
FUNCPTR(D2COMMON, FixOverheadMsg, void __stdcall, (OverheadMsg* pMsg, DWORD dwUnk), 0x4DC70)

FUNCPTR(D2COMMON, AddRoomData, void __stdcall, (Act * ptAct, int LevelId, int Xpos, int Ypos, Room1 * pRoom), 0x24990)
FUNCPTR(D2COMMON, RemoveRoomData, void __stdcall, (Act* ptAct, int LevelId, int Xpos, int Ypos, Room1* pRoom), 0x24930)

FUNCPTR(D2COMMON, GetQuestFlag, int __stdcall, (void* QuestInfo, DWORD dwAct, DWORD dwQuest), 0x2D7A0)

FUNCPTR(D2COMMON, AbsScreenToMap, void __stdcall, (long *pX, long *pY), 0x35810)
FUNCPTR(D2COMMON, MapToAbsScreen, void __stdcall, (long *pX, long *pY), 0x35AA0)

FUNCPTR(D2COMMON, CheckWaypoint, DWORD __stdcall, (DWORD WaypointTable, DWORD dwLevelId), 0x5D270)

FUNCPTR(D2COMMON, IsTownByLevelNo, BOOL __stdcall, (DWORD dwLevelNo), 0x23950)
FUNCPTR(D2COMMON, IsTownByRoom, BOOL __stdcall, (Room1* pRoom1), 0x23B80)

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Common Globals
////////////////////////////////////////////////////////////////////////////////////////////////

VARPTR(D2COMMON, sgptDataTable, DWORD, 0xA33F0)


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Common Stubs
////////////////////////////////////////////////////////////////////////////////////////////////

ASMPTR(D2COMMON, DisplayOverheadMsg_I, 0x4DCF0)
ASMPTR(D2COMMON, GetLevelIdFromRoom_I, 0x23B80)


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Net Functions
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2NET, SendPacket, void __stdcall, (size_t aLen, DWORD arg1, BYTE* aPacket), 0x6F20)
FUNCPTR(D2NET, ReceivePacket_I, void __stdcall, (BYTE *aPacket, DWORD aLen), 0x6020)


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Net Globals
////////////////////////////////////////////////////////////////////////////////////////////////

VARPTR(D2NET, CriticalPacketSection, CRITICAL_SECTION, 0xB400) // unused but we need to use it


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Gfx Ordinals
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2GFX, DrawLine, void __stdcall, (int X1, int Y1, int X2, int Y2, DWORD dwColor, DWORD dwUnk), 0x81A0)
FUNCPTR(D2GFX, DrawRectangle, void __stdcall, (int X1, int Y1, int X2, int Y2, DWORD dwColor, DWORD dwTrans), 0x8210)
FUNCPTR(D2GFX, DrawAutomapCell2, void __stdcall, (CellContext* context, DWORD xpos, DWORD ypos, DWORD bright2, DWORD bright, BYTE *coltab), 0x7E60)

FUNCPTR(D2GFX, GetScreenSize, DWORD __stdcall, (void), 0xA940) 

FUNCPTR(D2GFX, GetHwnd, HWND __stdcall, (void), 0xB0C0)
FUNCPTR(D2GFX, DrawAutomapCell, void __stdcall, (CellContext *context, DWORD xpos, DWORD ypos, RECT *cliprect, DWORD bright), 0x7C80)

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Multi Functions
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2MULTI, DoChat, void __fastcall, (void), 0x11770)
FUNCPTR(D2MULTI, PrintChannelText, void __stdcall, (char *szText, DWORD dwColor),  0x13F30)


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Multi Globals
////////////////////////////////////////////////////////////////////////////////////////////////

VARPTR(D2MULTI, ChatBoxMsg, char*, 0x1C150)
VARPTR(D2MULTI, GameListControl, Control*, 0x39FF0)//1.13c - Unchanged

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Multi Stubs
////////////////////////////////////////////////////////////////////////////////////////////////

ASMPTR(D2MULTI, ChannelChat_I, 0x14BE6)
ASMPTR(D2MULTI, ChannelEmote_I, 0x14856)
ASMPTR(D2MULTI, ChannelWhisper_I, 0x142F0)
ASMPTR(D2MULTI, ChannelInput_I, 0x11B80)

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Cmp Ordinals
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2CMP, InitCellFile, void __stdcall, (void *cellfile, CellFile **outptr, char *srcfile, DWORD lineno, DWORD filever, char *filename), 0x13B50)
FUNCPTR(D2CMP, DeleteCellFile, void __stdcall, (CellFile *cellfile), 0x99B0)


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Lang Ordinals
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2LANG, GetLocaleText, wchar_t* __fastcall, (WORD nLocaleTxtNo), 0x98A0)


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Lang Stubs
////////////////////////////////////////////////////////////////////////////////////////////////

ASMPTR(D2LANG, Say_II, 0x8C60)


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Launch Globals
////////////////////////////////////////////////////////////////////////////////////////////////

VARPTR(D2LAUNCH, BnData, BnetData *, 0x25B30)


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Win Functions
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2WIN, SetControlText, void* __fastcall, (Control* box, wchar_t* txt), 0x10680)
FUNCPTR(D2WIN, DrawSprites, void __fastcall, (void), 0xEAA0)


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Win Ordinals
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2WIN, TakeScreenshot, void __fastcall, (), 0xDF70)

FUNCPTR(D2WIN, DrawText, void __fastcall, (const wchar_t *wStr, int xPos, int yPos, DWORD dwColor, DWORD dwUnk), 0x13B30)

FUNCPTR(D2WIN, GetTextSize, DWORD __fastcall, (wchar_t *wStr, DWORD* dwWidth, DWORD* dwFileNo), 0x13290)
FUNCPTR(D2WIN, SetTextSize, DWORD __fastcall, (DWORD dwSize), 0x13B70)

FUNCPTR(D2WIN, GetTextWidthFileNo, DWORD __fastcall, (wchar_t *wStr, DWORD* dwWidth, DWORD* dwFileNo), 0x13290)
FUNCPTR(D2WIN, DestroyEditBox, VOID __fastcall, (Control* pControl), 0xF320)//1.13c
FUNCPTR(D2WIN, DestroyControl, VOID __stdcall, (Control* pControl), 0xE5F0)//1.13c
FUNCPTR(D2WIN, SetEditBoxCallback, VOID __fastcall, (Control* pControl, BOOL (__stdcall *FunCallBack)(Control* pControl, DWORD dwInputType, char* pChar)), 0xF1D0)//1.13c
FUNCPTR(D2WIN, SetEditBoxProc, void __fastcall, (Control* box, BOOL (__stdcall *FunCallBack)(Control*,DWORD,DWORD)), 0xF1D0)//Updated 1.13c
FUNCPTR(D2WIN, SelectEditBoxText, void __fastcall, (Control* box), 0x76D8) //Updated 1.13c
FUNCPTR(D2WIN, InitMPQ, DWORD __stdcall, (char *dll,const char *mpqfile, char *mpqname, int v4, int v5), 0x7E50)

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Win Globals
////////////////////////////////////////////////////////////////////////////////////////////////

VARPTR(D2WIN, FirstControl, Control*, 0x8DB34)
VARPTR(D2WIN, FocusedControl, Control*, 0x8DB44) // unused, but we ought to use it
VARPTR(D2MULTI, ChatInputBox, DWORD*, 0x3A0B0)

VARPTR(BNCLIENT, ClassicKey, char*, 0x1E928)
VARPTR(BNCLIENT, XPacKey, char*, 0x1E930)
VARPTR(BNCLIENT, KeyOwner, char*, 0x1E934)
//FUNCPTR(BNCLIENT, DecodeAndLoadKeys, char __cdecl, (), 0x15D10)
FUNCPTR(BNCLIENT, DecodeAndLoadKeys, char __cdecl, (), 0x10920)
VARPTR(STORM, WindowHandlers, WindowHandlerHashTable, 0x55878)

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Game Functions
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2GAME, Rand, DWORD __fastcall, (DWORD* seed), 0x1050)




#define _D2PTRS_END	D2GAME_Rand

#undef FUNCPTR
#undef VARPTR
#undef ASMPTR

#define D2CLIENT_TestPvpFlag(dwId1, dwId2, dwFlag)		(D2CLIENT_TestPvpFlag_STUB(dwId1, dwId2, dwFlag))
#define D2CLIENT_GetUIState(dwVarNo)					(D2CLIENT_GetUIVar_STUB(dwVarNo))
#define D2CLIENT_SetUIState(dwVarNo, value)				(D2CLIENT_SetUIVar(dwVarNo, value, 0))
#define D2CLIENT_InitAutomapLayer(layerlvl)				((AutomapLayer*)D2CLIENT_InitAutomapLayer_STUB(layerlvl))
#define D2CLIENT_GetUnitName(x)							(wchar_t*)D2CLIENT_GetUnitName_STUB((DWORD)x)
#define D2CLIENT_SetSelectedUnit(x)						(D2CLIENT_SetSelectedUnit_STUB((DWORD)x))
#define D2CLIENT_LoadUIImage(x)							((CellFile*)D2CLIENT_LoadUIImage_ASM(x))
#define D2CLIENT_Interact_STUB(x)						(D2CLIENT_Interact_ASM((DWORD)x))
#define D2CLIENT_ClickParty(x,y)						(D2CLIENT_ClickParty_ASM((DWORD)x, (DWORD)y))
#define D2CLIENT_RightClickItem(x, y, loc, player, invdata) D2CLIENT_ClickItemRight_ASM(x,y, loc, (DWORD)player, (DWORD)invdata)
#define D2CLIENT_ClickBeltRight(pPlayer, pInventory, dwShift, dwPotPos)	D2CLIENT_ClickBeltRight_ASM((DWORD)pPlayer, (DWORD)pInventory, dwShift, dwPotPos)
#define D2CLIENT_GetItemDesc(pUnit, pBuffer)			D2CLIENT_GetItemDesc_ASM((DWORD)pUnit, pBuffer)
#define D2CLIENT_MercItemAction(bPacketType, dwSlotId)	D2CLIENT_MercItemAction_ASM(bPacketType, dwSlotId)
#define D2CLIENT_SendGamePacket(dwLen, bPacket)			D2CLIENT_SendGamePacket_ASM(dwLen, bPacket)

#define D2COMMON_DisplayOverheadMsg(pUnit)				D2COMMON_DisplayOverheadMsg_ASM((DWORD)pUnit)

#define D2GFX_DrawFrame(Rect)							D2GFX_DrawRectFrame_STUB(Rect)

#pragma warning ( pop )
#pragma optimize ( "", on )

#endif