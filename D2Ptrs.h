#ifndef __D2PTRS_ONCE_
#pragma once
#endif
#ifndef __D2PTRS_H__
#define __D2PTRS_H__

#include "D2Structs.h"

#pragma warning ( push )
#pragma warning ( disable: 4245 )

#pragma optimize ( "", off )

#ifdef _DEFINE_VARS

#ifdef __D2PTRS_LIST_

#define FUNCPTR(d1,v1,t1,t2,o1)	&d1##_##v1,
#define VARPTR(d1,v1,t1,o1)		&p_##d1##_##v1,
#define ASMPTR(d1,v1,o1)		&d1##_##v1,
void *d2ptrs_list[] = {

#else
enum {DLLNO_D2CLIENT, DLLNO_D2COMMON, DLLNO_D2GFX, DLLNO_D2LANG, DLLNO_D2WIN, DLLNO_D2NET, DLLNO_D2GAME, DLLNO_D2LAUNCH, DLLNO_FOG, DLLNO_BNCLIENT, DLLNO_STORM, DLLNO_D2CMP, DLLNO_D2MULTI};

#define DLLOFFSET(a1,b1) ((DLLNO_##a1)|((b1)<<8))
#define FUNCPTR(d1,v1,t1,t2,o1)	typedef t1 d1##_##v1##_t t2; d1##_##v1##_t *d1##_##v1 = (d1##_##v1##_t *)DLLOFFSET(d1,o1);
#define VARPTR(d1,v1,t1,o1)		typedef t1 d1##_##v1##_t;    d1##_##v1##_t *p_##d1##_##v1 = (d1##_##v1##_t *)DLLOFFSET(d1,o1);
#define ASMPTR(d1,v1,o1)			DWORD d1##_##v1 = DLLOFFSET(d1,o1);
#endif

#else

#define FUNCPTR(d1,v1,t1,t2,o1)	typedef t1 d1##_##v1##_t t2; extern d1##_##v1##_t *d1##_##v1;
#define VARPTR(d1,v1,t1,o1)		typedef t1 d1##_##v1##_t;    extern d1##_##v1##_t *p_##d1##_##v1;
#define ASMPTR(d1,v1,o1)			extern DWORD d1##_##v1;

#endif

FUNCPTR(D2CLIENT, GetQuestInfo, void* __stdcall, (void), 0xB32D0) //Updated 1.14d //004B32D0-BASE 

FUNCPTR(D2CLIENT, SubmitItem, void __fastcall, (DWORD dwItemId), 0xB2370) //Updated 1.14d //004B2370-BASE
FUNCPTR(D2CLIENT, Transmute, void __fastcall, (void), 0x8A0D0) //Updated 1.14d //0048A0D0-BASE

FUNCPTR(D2CLIENT, FindClientSideUnit, UnitAny* __fastcall, (DWORD dwId, DWORD dwType), 0x63990) //Updated 1.14d //00463990-BASE
FUNCPTR(D2CLIENT, FindServerSideUnit, UnitAny* __fastcall, (DWORD dwId, DWORD dwType), 0x639B0) //Updated 1.14d //004639B0-BASE
FUNCPTR(D2CLIENT, GetCurrentInteractingNPC, UnitAny* __fastcall, (void), 0xB1620)//Updated 1.14d //004B1620-BASE
FUNCPTR(D2CLIENT, GetSelectedUnit, UnitAny * __stdcall, (), 0x67A10)//Updated 1.14d //00467A10-BASE
FUNCPTR(D2CLIENT, GetCursorItem, UnitAny* __fastcall, (void), 0x680A0) //Updated 1.14d //004680A0-BASE
//FUNCPTR(D2CLIENT, GetMercUnit, UnitAny* __fastcall, (void), 0x78A90) //Updated 1.14d //00478A90-BASE 478F20 with 7 0 args
FUNCPTR(D2CLIENT, SetSelectedUnit_I, void __fastcall, (UnitAny *pUnit), 0x66DE0) //Updated 1.14d //00466DE0-BASE
FUNCPTR(D2CLIENT, GetItemName, BOOL __fastcall, (UnitAny* pItem, wchar_t* wBuffer, DWORD dwSize), 0x8C060)//Updated 1.14d //0048C060-BASE
FUNCPTR(D2CLIENT, LoadItemDesc, BOOL __stdcall, (UnitAny* pItem, int type), 0x8DD90) //Updated 1.14d //0048DD90-BASE
FUNCPTR(D2CLIENT, GetMonsterOwner, DWORD __fastcall, (DWORD nMonsterId), 0x79150) //Updated 1.14d //00479150-BASE
FUNCPTR(D2CLIENT, GetUnitHPPercent, DWORD __fastcall, (DWORD dwUnitId), 0x79080) //Updated 1.14d //00479080-BASE
FUNCPTR(D2CLIENT, InitInventory, void __fastcall, (void), 0x845A0) //Updated 1.14d //004845A0-BASE
FUNCPTR(D2CLIENT, SetUIVar, DWORD __fastcall, (DWORD varno, DWORD howset, DWORD unknown1), 0x55F20)//Updated 1.14d //00455F20-BASE
FUNCPTR(D2CLIENT, GetUnitX, int __fastcall, (UnitAny* pUnit), 0x5ADF0) //Updated 1.14d //0045ADF0-BASE
FUNCPTR(D2CLIENT, GetUnitY, int __fastcall, (UnitAny* pUnit), 0x5AE20) //Updated 1.14d //0045AE20-BASE

FUNCPTR(D2CLIENT, ShopAction, void __fastcall, (UnitAny* pNpc, UnitAny* pItem, DWORD dwSell, DWORD unk, DWORD dwItemCost, DWORD dwMode, DWORD _2, DWORD _3), 0xB3870) //Updated 1.14d //004B3870-BASE

FUNCPTR(D2CLIENT, CloseNPCInteract, void __fastcall, (void), 0xB3F10) //Updated 1.14d //004B3F10-BASE
FUNCPTR(D2CLIENT, CloseInteract, void __fastcall, (void), 0x4C6B0) //Updated 1.14d //0044C6B0-BASE

FUNCPTR(D2CLIENT, GetAutomapSize, DWORD __stdcall, (void), 0x5A710) //Updated 1.14d //0045A710-BASE
FUNCPTR(D2CLIENT, NewAutomapCell, AutomapCell * __fastcall, (), 0x57C30) //Updated 1.14d //00457C30-BASE
FUNCPTR(D2CLIENT, AddAutomapCell, void __fastcall, (AutomapCell *aCell, AutomapCell **node), 0x57B00) //Updated 1.14d //00457B00-BASE
FUNCPTR(D2CLIENT, RevealAutomapRoom, void __stdcall, (Room1 *pRoom1, DWORD dwClipFlag, AutomapLayer *aLayer), 0x58F40) //Updated 1.14d //00458F40-BASE
FUNCPTR(D2CLIENT, InitAutomapLayer_I, AutomapLayer* __fastcall, (DWORD nLayerNo), 0x58D40)//Updated 1.14d //00458D40-BASE

FUNCPTR(D2CLIENT, ClickMap, void __fastcall, (DWORD MouseFlag, DWORD x, DWORD y, DWORD Type), 0x62D00) //Updated 1.14d //00462D00-BASE
FUNCPTR(D2CLIENT, LeftClickItem_I, void __stdcall, (UnitAny* pPlayer, Inventory* pInventory, int x, int y, DWORD dwClickType, InventoryLayout* pLayout, DWORD Location), 0x8FFE0) //Updated 1.14d //0048FFE0-BASE

FUNCPTR(D2CLIENT, GetMouseXOffset, DWORD __fastcall, (VOID), 0x5AFC0)//Updated 1.14d //0045AFC0-BASE
FUNCPTR(D2CLIENT, GetMouseYOffset, DWORD __fastcall, (VOID), 0x5AFB0)//Updated 1.14d //0045AFB0-BASE

FUNCPTR(D2CLIENT, PrintGameString, void __fastcall, (wchar_t *wMessage, int nColor), 0x9E3A0) //Updated 1.14d //0049E3A0-BASE
FUNCPTR(D2CLIENT, PrintPartyString, void __fastcall, (wchar_t *wMessage, int nColor), 0x9E5C0)//Updated 1.14d //0049E5C0-BASE

FUNCPTR(D2CLIENT, LeaveParty, void __fastcall, (void), 0x79FC0) //Updated 1.14d //00479FC0-BASE

FUNCPTR(D2CLIENT, AcceptTrade, void __fastcall, (void), 0xB9070) //Updated 1.14d //004B9070-BASE
FUNCPTR(D2CLIENT, CancelTrade, void __fastcall, (void), 0xB90B0) //Updated 1.14d //004B90B0-BASE
FUNCPTR(D2CLIENT, TradeOK, void __stdcall, (void), 0xB8A30) //Updated 1.14d //004B8A30-BASE

FUNCPTR(D2CLIENT, GetDifficulty, BYTE __stdcall, (), 0x4DCD0)//Updated 1.14d //0044DCD0-BASE

FUNCPTR(D2CLIENT, ExitGame, void __fastcall, (void), 0x4DD60) //Updated 1.14d //0044DD60-BASE

FUNCPTR(D2CLIENT, GetUiVar_I, DWORD __fastcall, (DWORD dwVarNo), 0x538D0) //Updated 1.14d //004538D0-BASE

FUNCPTR(D2CLIENT, DrawRectFrame, VOID __fastcall, (DWORD Rect), 0x52E50) //Updated 1.14d //00452E50-BASE

FUNCPTR(D2CLIENT, PerformGoldDialogAction, void __fastcall, (void), 0x54080) //Updated 1.14d //00454080-BASE

FUNCPTR(D2CLIENT, GetPlayerUnit, UnitAny*  __stdcall, (), 0x63DD0)  //Updated 1.14d //00463DD0-BASE

//FUNCPTR(D2CLIENT, GetLevelName_I, wchar_t* __fastcall, (DWORD levelId), 0x53E70) //Updated 1.14d //00453E70-BASE

FUNCPTR(D2CLIENT, ClearScreen, void __fastcall, (void), 0xB4620) //Updated 1.14d //004B4620-BASE

FUNCPTR(D2CLIENT, CloseNPCTalk, DWORD __stdcall, (void* unk), 0xA17D0) //Updated 1.14d //004A17D0-BASE

FUNCPTR(D2CLIENT, TestPvpFlag, DWORD __fastcall, (DWORD dwUnitId1, DWORD dwUnitId2, DWORD dwFlag), 0xDC440) //Updated 1.14d //004DC440-BASE

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Client Globals
////////////////////////////////////////////////////////////////////////////////////////////////

VARPTR(D2CLIENT, ScreenSizeX, DWORD, 0x31146C) //Updated 1.14d //0071146C-BASE
VARPTR(D2CLIENT, ScreenSizeY, DWORD, 0x311470) //Updated 1.14d //00711470-BASE

VARPTR(D2CLIENT, CursorHoverX, DWORD, 0x321E4C) //Updated 1.14d //00721E4C-BASE
VARPTR(D2CLIENT, CursorHoverY, DWORD, 0x321E50) //Updated 1.14d //00721E50-BASE

VARPTR(D2CLIENT, MouseY, DWORD, 0x3A6AAC)//Updated 1.14d //007A6AAC-BASE
VARPTR(D2CLIENT, MouseX, DWORD, 0x3A6AB0)//Updated 1.14d //007A6AB0-BASE

VARPTR(D2CLIENT, MouseOffsetY, int, 0x3A5208) //Updated 1.14d //007A5208-BASE
VARPTR(D2CLIENT, MouseOffsetZ, int, 0x3A5214)//Updated 1.14d //007A5214-BASE
VARPTR(D2CLIENT, MouseOffsetX, int, 0x3A520C)//Updated 1.14d //007A520C-BASE

VARPTR(D2CLIENT, AutomapOn, DWORD, 0x3A27E8) //Updated 1.14d //007A27E8-BASE
VARPTR(D2CLIENT, AutomapMode, int, 0x311254) //Updated 1.14d //00711254-BASE **Divisor**
VARPTR(D2CLIENT, Offset, POINT, 0x3A5198) //Updated 1.14d //007A5198-BASE
VARPTR(D2CLIENT, AutomapLayer, AutomapLayer*, 0x3A5164)//Updated 1.14d //007A5164-BASE 

//VARPTR(D2CLIENT, MercStrIndex, WORD, 0x325494) //Updated 1.14d //00725494-BASE
VARPTR(D2CLIENT, MercReviveCost, DWORD, 0x3C0DD0) //Updated 1.14d //007C0DD0-BASE

VARPTR(D2CLIENT, ServerSideUnitHashTables, UnitHashTable, 0x3A5E70) //Updated 1.14d //007A5E70-BASE
VARPTR(D2CLIENT, ClientSideUnitHashTables, UnitHashTable, 0x3A5270) //Updated 1.14d //007A5270-BASE

VARPTR(D2CLIENT, ViewportY, int, 0x3A5208) //Updated 1.14d //007A5208-BASE **MouseOffsetY
VARPTR(D2CLIENT, ViewportX, int, 0x3A520C)//Updated 1.14d //007A520C-BASE **MouseOffsetX

VARPTR(D2CLIENT, GoldDialogAction, DWORD, 0x3A279C) //Updated 1.14d //007A279C-BASE
VARPTR(D2CLIENT, GoldDialogAmount, DWORD, 0x3A2A68) //Updated 1.14d //007A2A68-BASE

VARPTR(D2CLIENT, NPCMenu, NPCMenu*, 0x326C48) //Updated 1.14d //00726C48-BASE
VARPTR(D2CLIENT, NPCMenuAmount, DWORD, 0x325A74) //Updated 1.14d //00725A74-BASE

VARPTR(D2CLIENT, TradeLayout, InventoryLayout*, 0x3BCA30) //Updated 1.14d //007BCA30-BASE
VARPTR(D2CLIENT, StashLayout, InventoryLayout*, 0x3BCA78) //Updated 1.14d //007BCA78-BASE
VARPTR(D2CLIENT, StoreLayout, InventoryLayout*, 0x3BCB58) //Updated 1.14d //007BCB58-BASE
VARPTR(D2CLIENT, CubeLayout, InventoryLayout*, 0x3BCB70) //Updated 1.14d //007BCB70-BASE
VARPTR(D2CLIENT, InventoryLayout, InventoryLayout*, 0x3BCB88) //Updated 1.14d //007BCB88-BASE
VARPTR(D2CLIENT, MercLayout, InventoryLayout*, 0x3BCD4C) //Updated 1.14d //007BCD4C-BASE

VARPTR(D2CLIENT, RegularCursorType, DWORD, 0x3A6AF0) //Updated 1.14d //007A6AF0-BASE
VARPTR(D2CLIENT, ShopCursorType, DWORD, 0x3BCBF0) //Updated 1.14d //007BCBF0-BASE


VARPTR(D2CLIENT, Ping, DWORD, 0x3A04A4) //Updated 1.14d  //007A04A4-BASE
VARPTR(D2CLIENT, Skip, DWORD, 0x3A04B0) //Updated 1.14d //007A04B0-BASE
VARPTR(D2CLIENT, FPS, DWORD, 0x3BB390)//Updated 1.14d //007BB390-BASE
VARPTR(D2CLIENT, Divisor, int, 0x311254) //Updated 1.14d //00711254-BASE

VARPTR(D2CLIENT, OverheadTrigger, DWORD, 0x3BF20E) //Updated 1.14d  //007BF20E-BASE

VARPTR(D2CLIENT, RecentInteractId, DWORD, 0x3C0D25)//Updated 1.14d //007C0D25-BASE

VARPTR(D2CLIENT, ItemPriceList, DWORD, 0x3C0D43) //Updated 1.14d //007C0D43-BASE

VARPTR(D2CLIENT, TransactionDialog, void*, 0x3C0D63)//Updated 1.14d //007C0D63-BASE
VARPTR(D2CLIENT, TransactionDialogs, DWORD, 0x3C0E5C) //Updated 1.14d //007C0E5C-BASE
VARPTR(D2CLIENT, TransactionDialogs_2, DWORD, 0x3C0E58) //Updated 1.14d //007C0E58-BASE
VARPTR(D2CLIENT, pTransactionDialogsInfo, TransactionDialogsInfo_t*, 0x3C0E54) //Updated 1.14d //007C0E54-BASE

VARPTR(D2CLIENT, GameInfo, GameStructInfo*, 0x3A0438) //Updated 1.14d //007A0438-BASE

VARPTR(D2CLIENT, WaypointTable, DWORD, 0x3BF081) //Updated 1.14d //007BF081-BASE

VARPTR(D2CLIENT, PlayerUnit, UnitAny*, 0x3A6A70) //Updated 1.14d //007A6A70-BASE
VARPTR(D2CLIENT, SelectedInvItem, UnitAny*, 0x3BCBF4) //Updated 1.14d //007BCBF4-BASE
//VARPTR(D2CLIENT, SelectedUnit, UnitAny*, 0x11C4D8) // unused, but can we use it somewhere maybe? // 1.12 still
VARPTR(D2CLIENT, PlayerUnitList, RosterUnit*, 0x3BB5C0)//Updated 1.14d //007BB5C0-BASE

VARPTR(D2CLIENT, bWeapSwitch, DWORD, 0x3BCC4C) //Updated 1.14d //007BCC4C-BASE

VARPTR(D2CLIENT, bTradeAccepted, DWORD, 0x3BCE18) //Updated 1.14d //007BCE18-BASE
VARPTR(D2CLIENT, bTradeBlock, DWORD, 0x3BCE28) //Updated 1.14d //007BCE28-BASE

//VARPTR(D2CLIENT, RecentTradeName, wchar_t*, 0x12334C)
VARPTR(D2CLIENT, RecentTradeId, DWORD, 0x3C0E7C) //Updated 1.14d //007C0E7C-BASE

VARPTR(D2CLIENT, ExpCharFlag, DWORD, 0x3A04F4) //Updated 1.14d //007A04F4-BASE

VARPTR(D2CLIENT, MapId, DWORD, 0x3A0638) //Updated 1.14d //007A0638-BASE // unused but I want to add it

VARPTR(D2CLIENT, AlwaysRun, DWORD, 0x3A0660) //Updated 1.14d //007A0660-BASE
VARPTR(D2CLIENT, NoPickUp, DWORD, 0x3A6A90) //Updated 1.14d //007A6A90-BASE // unused but I want to add it

//VARPTR(D2CLIENT, ScreenCovered, DWORD, 0x1E8F9) // unused, appears to be an int specifying which screens (if any) are opened...

VARPTR(D2CLIENT, ChatMsg, wchar_t*, 0x3BB638) //Updated 1.14d //007BB638-BASE
VARPTR(D2CLIENT, OrificeId, DWORD, 0x3C547C) //Updated 1.14d //007C547C-BASE
VARPTR(D2CLIENT, CursorItemMode, DWORD, 0x3C5474) //Updated 1.14d //007C5474-BASE


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Client Stubs
////////////////////////////////////////////////////////////////////////////////////////////////

ASMPTR(D2CLIENT, TakeWaypoint_I, 0x9D0F1) //Updated 1.14d //0049D0F1-BASE

ASMPTR(D2CLIENT, ClickShopItem_I, 0xB2653) //Updated 1.14d //004B2653-BASE
ASMPTR(D2CLIENT, ClickBelt_I, 0x98870) //Updated 1.14d //00498870-BASE
ASMPTR(D2CLIENT, ClickBeltRight_I, 0x98A90) //Updated 1.14d //00498A90-BASE
ASMPTR(D2CLIENT, ClickItemRight_I, 0x87740) //Updated 1.14d //00487740-BASE
ASMPTR(D2CLIENT, MercItemAction_I, 0x785B0) //Updated 1.14d //004785B0-BASE

ASMPTR(D2CLIENT, Interact_I, 0x2B2E0) //6FADB2E0

ASMPTR(D2CLIENT, ClickParty_I, 0x79EB0) //Updated 1.14d //00479EB0-BASE
ASMPTR(D2CLIENT, ClickParty_II, 0x9B990) //Updated 1.14d //0049B990-BASE

ASMPTR(D2CLIENT, ShopAction_I, 0x7D030) //6FB2D030 **ShopAction**

ASMPTR(D2CLIENT, GetUnitName_I, 0x64A60) //Updated 1.14d //00464A60-BASE
ASMPTR(D2CLIENT, GetItemDesc_I, 0x2E380) //6FADE380

ASMPTR(D2CLIENT, AssignPlayer_I, 0x66200) //Updated 1.14d //00466200-BASE

//ASMPTR(D2CLIENT, TestPvpFlag_I, 0x6A720) //6FB1A720 - Fastcall now

ASMPTR(D2CLIENT, InputCall_I, 0x787B0) //Updated 1.14d //004787B0-BASE

ASMPTR(D2CLIENT, Say_I, 0x7CBDA) //Updated 1.14d //0047CBDA-BASE

ASMPTR(D2CLIENT, BodyClickTable, 0x321E58) //Updated 1.14d //00721E58-BASE

ASMPTR(D2CLIENT, LoadUIImage_I, 0x788B0) //Updated 1.14d //004788B0-BASE /// switch to d2win LoadCellFile //

ASMPTR(D2CLIENT, GetMinionCount_I, 0x78EE0) //Updated 1.14d //00478EE0-BASE

ASMPTR(D2CLIENT, GameLeave_I, 0x81830) //Updated 1.14d //00481830-BASE

//ASMPTR(D2CLIENT, ClickMap_I, 0x11C8B4) //6FBCC8B4
//ASMPTR(D2CLIENT, ClickMap_II, 0x11D2CC)//6FBCD2CC //MoveOn
//ASMPTR(D2CLIENT, ClickMap_III, 0x5BB50)//ClickMap_III //CallOne
//ASMPTR(D2CLIENT, ClickMap_IV, 0x2B499)//6FADB499

ASMPTR(D2CLIENT, GameAddUnit_I, 0x628E0) //6FB128E0

//ASMPTR(D2CLIENT, LoadAct_1, 0x547F0) //Updated 1.14b //004547F0-BASE
//ASMPTR(D2CLIENT, LoadAct_2, 0x5E4A0) //Updated 1.14b //0045E4A0-BASE

//ASMPTR(D2CLIENT, GetUnitFromId_I, 0x61480) //GetUnitFromId_II //6FB11480
//VARPTR(D2CLIENT, pUnitTable, POINT, 0x1047B8) //6FBB47B8 //GetUnitFromId_I //IrC

//ASMPTR(D2CLIENT, OverrideShrinePatch_ORIG, 0x3A3150)//Updated 1.14d //007A3150-BASE

ASMPTR(D2CLIENT, SendGamePacket_I, 0x78350)//Updated 1.14d //00478350-BASE //SendPacket_I
ASMPTR(D2CLIENT, SendPacket_II, 0x12AE62)//Updated **NEW** 1.14d //0052AE62-BASE

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Common Ordinals
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2COMMON, InitLevel, void __stdcall, (Level *pLevel), 0x2424A0)//Updated 1.14d //006424A0-BASE
FUNCPTR(D2COMMON, UnloadAct, unsigned __stdcall, (Act* pAct), 0x21AFD3)//Updated 1.14d //0061AFD3-BASE
FUNCPTR(D2COMMON, GetObjectTxt, ObjectTxt * __stdcall, (DWORD objno), 0x240E90) // Updated 1.14d //00640E90-BASE
FUNCPTR(D2COMMON, LoadAct, Act* __stdcall, (DWORD ActNumber, DWORD MapId, DWORD Unk, DWORD Unk_2, DWORD Unk_3, DWORD Unk_4, DWORD TownLevelId, DWORD Func_1, DWORD Func_2), 0x2194A0) //Updated 1.14d //006194A0-BASE

FUNCPTR(D2COMMON, GetLevelText, LevelTxt * __stdcall, (DWORD levelno), 0x21DB70) //Updated 1.14d //0061DB70-BASE
FUNCPTR(D2COMMON, GetObjectText, ObjectTxt * __stdcall, (DWORD objno), 0x240E90) // Updated 1.14d //00640E90-BASE
FUNCPTR(D2COMMON, GetItemText, ItemTxt * __stdcall, (DWORD itemno), 0x2335F0)//Updated 1.14d //006335F0-BASE

FUNCPTR(D2COMMON, GetLayer, AutomapLayer2* __fastcall, (DWORD dwLevelNo), 0x21E470) //Updated 1.14d //0061E470-BASE
FUNCPTR(D2COMMON, GetLevel, Level * __fastcall, (ActMisc *pMisc, DWORD dwLevelNo), 0x242AE0) //Updated 1.14d //00642AE0-BASE

FUNCPTR(D2COMMON, GetStatList, StatList* __stdcall, (UnitAny* pUnit, DWORD dwUnk, DWORD dwMaxEntries ), 0x2257D0) //Updated 1.14d //006257D0-BASE
FUNCPTR(D2COMMON, CopyStatList, DWORD __stdcall, (StatList* pStatList, Stat* pStatArray, DWORD dwMaxEntries), 0x225C90) //Updated 1.14d //0625C90-BASE
FUNCPTR(D2COMMON, GetUnitStat, DWORD __stdcall, (UnitAny* pUnit, DWORD dwStat, DWORD dwStat2), 0x225480) //Updated 1.14d //00625480-BASE
FUNCPTR(D2COMMON, GetUnitState, int __stdcall, (UnitAny *pUnit, DWORD dwStateNo), 0x239DF0)//Updated 1.14d //00639DF0-BASE

FUNCPTR(D2COMMON, CheckUnitCollision, DWORD __stdcall, (UnitAny* pUnitA, UnitAny* pUnitB, DWORD dwBitMask), 0x222AA0)//Updated 1.14d //00622AA0-BASE
FUNCPTR(D2COMMON, GetRoomFromUnit,  Room1* __stdcall, (UnitAny * ptUnit), 0x220BB0) //Updated 1.14d //00620BB0-BASE
FUNCPTR(D2COMMON, GetTargetUnitType, Path* __stdcall, (Path* pPath), 0x773C0)

FUNCPTR(D2COMMON, GetSkillLevel, INT __stdcall, (UnitAny* pUnit, Skill* pSkill, BOOL bTotal), 0x2442A0) //Updated 1.14d //006442A0-BASE

FUNCPTR(D2COMMON, GetItemLevelRequirement, DWORD __stdcall, (UnitAny* pItem, UnitAny* pPlayer), 0x22BA60)//Updated 1.14d //0062BA60-BASE

FUNCPTR(D2COMMON, GetItemPrice, DWORD __stdcall, (UnitAny* MyUnit, UnitAny* pItem, DWORD U1_,DWORD U2_,DWORD U3_,DWORD U4_), 0x22FDC0) //Updated 1.14d //0062FDC0-BASE
FUNCPTR(D2COMMON, GetRepairCost, DWORD __stdcall, (DWORD _1, UnitAny* pUnit, DWORD dwNpcId, DWORD dwDifficulty, DWORD dwItemPriceList, DWORD _2), 0x22FE60) //Updated 1.14d //0062FE60-BASE
FUNCPTR(D2COMMON, GetItemMagicalMods, char* __stdcall, (WORD wPrefixNum), 0x233EE0) //Updated 1.14d //00633EE0-BASE
FUNCPTR(D2COMMON, GetItemFromInventory, UnitAny *__stdcall, (Inventory* inv), 0x23B2C0) //Updated 1.14d //0063B2C0-BASE
FUNCPTR(D2COMMON, GetNextItemFromInventory, UnitAny *__stdcall, (UnitAny *pItem), 0x23DFA0) //Updated 1.14d //0063DFA0-BASE

FUNCPTR(D2COMMON, GenerateOverheadMsg, OverheadMsg* __stdcall, (DWORD dwUnk, CHAR* szMsg, DWORD dwTrigger), 0x261110) //Updated 1.14d //00661110-BASE
FUNCPTR(D2COMMON, FixOverheadMsg, VOID __stdcall, (OverheadMsg* pMsg, DWORD dwUnk), 0x261230)//Updated 1.14d //00661230-BASE

FUNCPTR(D2COMMON, AddRoomData, void __stdcall, (Act * ptAct, int LevelId, int Xpos, int Ypos, Room1 * pRoom), 0x21A070) //Updated 1.14d //0061A070-BASE
FUNCPTR(D2COMMON, RemoveRoomData, void __stdcall, (Act * ptAct, int LevelId, int Xpos, int Ypos, Room1 * pRoom), 0x21A0C0) //Updated 1.14d //0061A0C0-BASE

FUNCPTR(D2COMMON, GetQuestFlag, int __stdcall, (void* QuestInfo, DWORD dwAct, DWORD dwQuest), 0x25C310) //Updated 1.14d //0065C310-BASE

FUNCPTR(D2COMMON, MapToAbsScreen, void __stdcall, (long *pX, long *pY), 0x243260)//Updated 1.14d //00643260-BASE
FUNCPTR(D2COMMON, AbsScreenToMap, void __stdcall, (long *ptMouseX, long *ptMouseY), 0x243510) //Updated 1.14d  //00643510-BASE

FUNCPTR(D2COMMON, CheckWaypoint, DWORD __stdcall, (DWORD WaypointTable, DWORD dwLevelId), 0x260E50) //Updated 1.14d //00660E50-BASE

FUNCPTR(D2COMMON, IsTownByLevelNo, BOOL __stdcall, (DWORD dwLevelNo), 0x21AAF0) //Updated 1.14d //0061AAF0-BASE
FUNCPTR(D2COMMON, GetLevelNoFromRoom, BOOL __stdcall, (Room1* pRoom1), 0x21A1B0) //Updated 1.14d //0061A1B0-BASE
FUNCPTR(D2COMMON, FindRoom1, Room1* __stdcall, (Act* pAct, int x, int y), 0x219DA3) //Updated 1.14d //00619DA3-BASE
FUNCPTR(D2COMMON, GetItemPalette, int __stdcall, (UnitAny* pPlayer, UnitAny* pItem, BYTE* pColor, int nTransType), 0x22C100) //Updated 1.14d //0062C100-BASE
////////////////////////////////////////////////////////////////////////////////////////////////
// D2Common Globals
////////////////////////////////////////////////////////////////////////////////////////////////

VARPTR(D2COMMON, sgptDataTable, DWORD, 0x344304) //Updated 1.14d //00744304-BASE 


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Common Stubs
////////////////////////////////////////////////////////////////////////////////////////////////

ASMPTR(D2COMMON, DisplayOverheadMsg_I, 0x2611A0) //Updated 1.14d //006611A0-BASE
ASMPTR(D2COMMON, GetLevelIdFromRoom_I, 0x21A1B0) //Updated 1.14d //0061A1B0-BASE


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Net Functions
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2NET, SendPacket, void __stdcall, (size_t aLen, DWORD arg1, BYTE* aPacket), 0x12AE50) //Updated 1.14d //0052AE50-BASE
FUNCPTR(D2NET, ReceivePacket, void __fastcall, (BYTE *aPacket, DWORD aLen), 0x12AEB0) //Updated 1.14d //0052AEB0-BASE
FUNCPTR(D2NET, ReceivePacket_I, void __fastcall, (BYTE *aPacket, DWORD aLen, int*arg3), 0x12B920) //Updated 1.14d //0052B920-BASE


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Net Globals
////////////////////////////////////////////////////////////////////////////////////////////////

VARPTR(D2NET, CriticalPacketSection, CRITICAL_SECTION, 0xB400) // unused but we need to use it


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Gfx Ordinals
////////////////////////////////////////////////////////////////////////////////////////////////

//FUNCPTR(D2GFX, DrawAutomapCell, void __stdcall, (CellContext *context, DWORD xpos, DWORD ypos, RECT *cliprect, DWORD bright), 0xF6510) //Updated 1.14d //004F6510-BASE
FUNCPTR(D2GFX, DrawRectangle, void __stdcall, (int X1, int Y1, int X2, int Y2, DWORD dwColor, DWORD dwTrans), 0xF6300) //Updated 1.14d //004F6300-BASE
FUNCPTR(D2GFX, DrawLine, void __stdcall, (int X1, int Y1, int X2, int Y2, DWORD dwColor, DWORD dwUnk), 0xF6380) //Updated 1.14d //004F6380-BASE
FUNCPTR(D2GFX, DrawAutomapCell2, void __stdcall, (CellContext* context, DWORD xpos, DWORD ypos, DWORD bright2, DWORD bright, BYTE *coltab), 0xF6480) //Updated 1.14d //004F6480-BASE
FUNCPTR(D2GFX, GetHwnd, HWND __stdcall, (void), 0xF59A0) //Updated 1.14d //004F59A0-BASE
FUNCPTR(D2GFX, GetScreenSize, DWORD __stdcall, (), 0xF5160) //Updated 1.14d //004F5160-BASE

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Multi Functions
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2MULTI, DoChat, void __fastcall, (void), 0x42810) //Updated 1.14d //00442810-BASE
FUNCPTR(D2MULTI, PrintChannelText_, void __fastcall, (int unused, char *szText, DWORD dwColor),  0x47AB0) //Updated 1.14d //00447AB0-BASE


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Multi Globals
////////////////////////////////////////////////////////////////////////////////////////////////

VARPTR(D2MULTI, ChatBoxMsg, char*, 0x37AE40) //Updated 1.14d //0077AE40-BASE
VARPTR(D2MULTI, GameListControl, Control*, 0x398BC0) //Updated 1.14d //00798BC0-BASE

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Multi Stubs
////////////////////////////////////////////////////////////////////////////////////////////////

ASMPTR(D2MULTI, ChannelChat_I, 0x14BE6)
ASMPTR(D2MULTI, ChannelEmote_I, 0x14856)
ASMPTR(D2MULTI, ChannelWhisper_I, 0x142F0)
ASMPTR(D2MULTI, ChannelInput_I, 0x428D0) //Updated 1.14d //004428D0-BASE

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Cmp Ordinals
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2CMP, InitCellFile, VOID __stdcall, (LPVOID File, CellFile **Out, LPSTR SourceFile, DWORD Line, DWORD FileVersion, LPSTR Filename), 0x201340)//Updated 1.14d //00601340-BASE
FUNCPTR(D2CMP, DeleteCellFile, void __stdcall, (CellFile * File), 0x201A50)//Updated 1.14d //00601A50-BASE


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Lang Ordinals
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2LANG, GetLocaleText, wchar_t* __fastcall, (WORD nLocaleTxtNo), 0x124A30) //Updated 1.14d //00524A30-BASE


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Lang Stubs
////////////////////////////////////////////////////////////////////////////////////////////////

ASMPTR(D2LANG, Say_II, 0x126800) //Updated 1.14d //00526800-BASE


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Launch Globals
////////////////////////////////////////////////////////////////////////////////////////////////

VARPTR(D2LAUNCH, BnData, BnetData *, 0x3795D4) //Updated 1.14d //007795D4-BASE


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Win Functions
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2WIN, SetControlText, void* __fastcall, (Control* box, wchar_t* txt), 0xFF5A0) //Updated 1.14d //004FF5A0-BASE
FUNCPTR(D2WIN, DrawSprites, void __fastcall, (void), 0xF9870) //Updated 1.14d //004F9870-BASE
FUNCPTR(D2WIN, LoadCellFile, CellFile* __fastcall, (const char* szFile, int Type), 0xFA9B0) //Updated 1.14d //004FA9B0-BASE

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Win Ordinals
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2WIN, TakeScreenshot, void __fastcall, (), 0xFA7A0) //Updated 1.14d //004FA7A0-BASE
FUNCPTR(D2WIN, DrawText, void __fastcall, (const wchar_t *wStr, int xPos, int yPos, DWORD dwColor, DWORD dwUnk), 0x102320) //Updated 1.14d //00502320-BASE
FUNCPTR(D2WIN, GetTextSize, DWORD __fastcall, (wchar_t *wStr, DWORD* dwWidth, DWORD* dwFileNo), 0x102520) //Updated 1.14d //00502520-BASE
FUNCPTR(D2WIN, SetTextSize, DWORD __fastcall, (DWORD dwSize), 0x102EF0) //Updated 1.14d //00502EF0-BASE
FUNCPTR(D2WIN, GetTextWidthFileNo, DWORD __fastcall, (wchar_t *wStr, DWORD* dwWidth, DWORD* dwFileNo), 0x102520) //Updated 1.14d //00502520-BASE
FUNCPTR(D2WIN, DestroyEditBox, DWORD __fastcall, (Control* box), 0xFDAA0) //Updated 1.14d //004FDAA0-BASE
FUNCPTR(D2WIN, DestroyControl, VOID __stdcall, (Control* pControl), 0xf95c0) //Updated 1.14d // 004f95c0-BASE
FUNCPTR(D2WIN, SetEditBoxCallback, VOID __fastcall, (Control* pControl, BOOL(__stdcall *FunCallBack)(Control* pControl, DWORD dwInputType, char* pChar)), 0xFDAD0) //Updated 1.14d //004FDAD0-BASE
FUNCPTR(D2WIN, SetEditBoxProc, void __fastcall, (Control* box, BOOL(__stdcall *FunCallBack)(Control*, DWORD, DWORD)), 0xFDAD0) // Updated 1.14d //004FDAD0-BASE
FUNCPTR(D2WIN, SelectEditBoxText, void __fastcall, (Control* box), 0xfdd00) //Updated 1.14d //004fdd00-BASE
FUNCPTR(D2WIN, InitMPQ, DWORD __fastcall, (const char *mpqfile, char *mpqname, int v4, int v5), 0x117332) //Updated 1.14d //00517332-BASE

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Win Globals
////////////////////////////////////////////////////////////////////////////////////////////////

VARPTR(D2WIN, FirstControl, Control*, 0x3D55BC) //Updated 1.14d //007D55BC-BASE
VARPTR(D2WIN, FocusedControl, Control*, 0x3D55CC) //Updated 1.14d //007D55CC-BASE unused, but we ought to use it
VARPTR(D2MULTI, ChatInputBox, DWORD*, 0x398C80) //Updated 1.14d //00798C80-BASE


ASMPTR(BNCLIENT, DClass, 0x123673) //Updated 1.14d //00523673-BASE
ASMPTR(BNCLIENT, DLod, 0x12395D) //Updated 1.14d //0052395D-BASE

VARPTR(BNCLIENT, ClassicKey, char*, 0x482744) //Updated 1.14d //00882744-BASE 
VARPTR(BNCLIENT, XPacKey, char*, 0x48274C) //Updated 1.14d //0088274C-BASE
VARPTR(BNCLIENT, KeyOwner, char*, 0x482750) //Updated 1.14d //00882750-BASE
FUNCPTR(BNCLIENT, DecodeAndLoadKeys, char __cdecl, (), 0x1234D0) //Updated 1.14d //005234D0-BASE
VARPTR(STORM, WindowHandlers, WindowHandlerHashTable, 0x379300) //Updated 1.14d //00779300-BASE

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Game Functions
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2GAME, Rand, DWORD __fastcall, (DWORD* seed), 0x5C370) //Updated 1.14d //0045C370-BASE

FUNCPTR(D2GAME, exit0, DWORD __fastcall, (), 0x576F) //NEW 1.14d //0040576F-BASE




#undef FUNCPTR
#undef VARPTR
#undef ASMPTR

#ifdef __D2PTRS_LIST_
};
#else

//#define D2CLIENT_TestPvpFlag(dwId1, dwId2, dwFlag)		(D2CLIENT_TestPvpFlag_STUB(dwId1, dwId2, dwFlag))
#define D2MULTI_PrintChannelText(text,color)					(D2MULTI_PrintChannelText_(0,text,color))
#define D2CLIENT_GetUIState(dwVarNo)					(D2CLIENT_GetUIVar_STUB(dwVarNo))
#define D2CLIENT_SetUIState(dwVarNo, value)				(D2CLIENT_SetUIVar(dwVarNo, value, 0))
#define D2CLIENT_InitAutomapLayer(layerlvl)				((AutomapLayer*)D2CLIENT_InitAutomapLayer_STUB(layerlvl))
#define D2CLIENT_GetUnitName(x)							(wchar_t*)D2CLIENT_GetUnitName_STUB((DWORD)x)
#define D2CLIENT_SetSelectedUnit(x)						(D2CLIENT_SetSelectedUnit_STUB((DWORD)x))
//#define D2CLIENT_LoadUIImage(x)							((CellFile*)D2CLIENT_LoadUIImage_ASM(x))
#define D2CLIENT_Interact_STUB(x)						(D2CLIENT_Interact_ASM((DWORD)x))
#define D2CLIENT_ClickParty(x,y)						(D2CLIENT_ClickParty_ASM((DWORD)x, (DWORD)y))
#define D2CLIENT_RightClickItem(x, y, loc, player, invdata) D2CLIENT_ClickItemRight_ASM(x,y, loc, (DWORD)player, (DWORD)invdata)
#define D2CLIENT_ClickBeltRight(pPlayer, pInventory, dwShift, dwPotPos)	D2CLIENT_ClickBeltRight_ASM((DWORD)pPlayer, (DWORD)pInventory, dwShift, dwPotPos)
#define D2CLIENT_GetItemDesc(pUnit, pBuffer)			D2CLIENT_GetItemDesc_ASM((DWORD)pUnit, pBuffer)
#define D2CLIENT_MercItemAction(bPacketType, dwSlotId)	D2CLIENT_MercItemAction_ASM(bPacketType, dwSlotId)
#define D2CLIENT_SendGamePacket(dwLen, bPacket)			D2CLIENT_SendGamePacket_ASM(dwLen, bPacket)

#define D2COMMON_DisplayOverheadMsg(pUnit)				D2COMMON_DisplayOverheadMsg_ASM((DWORD)pUnit)

#define D2GFX_DrawFrame(Rect)							D2GFX_DrawRectFrame_STUB(Rect)

#endif

#pragma warning ( pop )
#pragma optimize ( "", on )

#endif
