#include "Constants.h"
#include "JSGame.h"
#include "D2Helpers.h"
#include "CriticalSections.h"
#include "D2Skills.h"
#include "MPQStats.h"
#include "Core.h"
#include "Helpers.h"
#include "Game.h"
#include "JSArea.h"
#include "JSGlobalClasses.h"
#include "TimedAlloc.h"
#include "MapHeader.h"

#include <cassert>
#include <cmath>

JSAPI_FUNC(my_copyUnit)
{
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	if(argc >= 1 && JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[0]) && !JSVAL_IS_NULL(JS_ARGV(cx, vp)[0]))
	{
		JS_SET_RVAL(cx, vp, JSVAL_VOID);
		Private* myPrivate = (Private*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]));

		if(!myPrivate)
			return JS_TRUE;

		if(myPrivate->dwPrivateType == PRIVATE_UNIT)
		{
			myUnit* lpOldUnit = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]));
			myUnit* lpUnit = new myUnit;

			if(lpUnit)
			{
				memcpy(lpUnit, lpOldUnit, sizeof(myUnit));
				JSObject* jsunit = BuildObject(cx, &unit_class, unit_methods, unit_props, lpUnit);
				if(!jsunit)
				{
					delete lpUnit;
					lpUnit = NULL;
					THROW_ERROR(cx, "Couldn't copy unit");
				}
				JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsunit));
			
			}
		}
		else if(myPrivate->dwPrivateType == PRIVATE_ITEM)
		{
			invUnit* lpOldUnit = (invUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]));
			invUnit* lpUnit = new invUnit;

			if(lpUnit)
			{
				memcpy(lpUnit, lpOldUnit, sizeof(invUnit));
				JSObject* jsunit = BuildObject(cx, &unit_class, unit_methods, unit_props, lpUnit);
				if(!jsunit)
				{
					delete lpUnit;
					lpUnit = NULL;
					THROW_ERROR(cx, "Couldn't copy unit");
				}
				JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsunit));				
			}
		}
	}

	return JS_TRUE;
}

JSAPI_FUNC(my_clickMap)
{	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	uint16 nClickType = NULL, nShift = NULL, nX = NULL, nY = NULL;

	JS_SET_RVAL(cx, vp, JSVAL_FALSE);
	
	if(argc < 3)
		return JS_TRUE;
	JS_BeginRequest(cx);
	if(JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		JS_ValueToUint16(cx, JS_ARGV(cx, vp)[0], &nClickType);
	if(JSVAL_IS_INT(JS_ARGV(cx, vp)[1]) || JSVAL_IS_BOOLEAN(JS_ARGV(cx, vp)[1]))
		JS_ValueToUint16(cx, JS_ARGV(cx, vp)[1], &nShift);
	if(JSVAL_IS_INT(JS_ARGV(cx, vp)[2]))
		JS_ValueToUint16(cx, JS_ARGV(cx, vp)[2], &nX);
	if(JSVAL_IS_INT(JS_ARGV(cx, vp)[3]))
		JS_ValueToUint16(cx, JS_ARGV(cx, vp)[3], &nY);
	JS_EndRequest(cx);
	if(argc == 3 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) &&
		(JSVAL_IS_INT(JS_ARGV(cx, vp)[1]) || JSVAL_IS_BOOLEAN(JS_ARGV(cx, vp)[1])) &&
		JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[2]) && !JSVAL_IS_NULL(JS_ARGV(cx, vp)[2]))
	{
		myUnit* mypUnit = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[2]));

		if(!mypUnit || (mypUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
			return JS_TRUE;

		UnitAny* pUnit = D2CLIENT_FindUnit(mypUnit->dwUnitId, mypUnit->dwType);

		if(!pUnit)
			return JS_TRUE;

		Vars.dwSelectedUnitId = NULL;
		Vars.dwSelectedUnitType = NULL;
		JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(ClickMap(nClickType, nX, nY, nShift, pUnit)));		
	}
	else if(argc > 3 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) &&
			(JSVAL_IS_INT(JS_ARGV(cx, vp)[1]) || JSVAL_IS_BOOLEAN(JS_ARGV(cx, vp)[1])) &&
			JSVAL_IS_INT(JS_ARGV(cx, vp)[2]) && JSVAL_IS_INT(JS_ARGV(cx, vp)[3]))
	{
		JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(ClickMap(nClickType, nX, nY, nShift, NULL)));
	}

	return JS_TRUE;
}

JSAPI_FUNC(my_acceptTrade)
{	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	// TODO: Fix this nonsense.
	if(argc > 0 && JSVAL_TO_INT(JS_ARGV(cx, vp)[0]) == 1) // Called with a '1' it will return if we already accepted it or not
	{
		JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL((*p_D2CLIENT_bTradeAccepted)));
		return JS_TRUE;
	}
	else if(JSVAL_TO_INT(JS_ARGV(cx, vp)[0]) == 2) // Called with a '2' it will return the trade flag
	{
		JS_SET_RVAL(cx, vp, INT_TO_JSVAL((*p_D2CLIENT_RecentTradeId)));
		return JS_TRUE;
	}
	else if(JSVAL_TO_INT(JS_ARGV(cx, vp)[0]) == 3) // Called with a '3' it will return if the 'check' is red or not
	{
		JS_SET_RVAL(cx, vp,  BOOLEAN_TO_JSVAL((*p_D2CLIENT_bTradeBlock)));
		return JS_TRUE;
	}

	CriticalMisc myMisc;
	myMisc.EnterSection();

	if((*p_D2CLIENT_RecentTradeId) == 3 || (*p_D2CLIENT_RecentTradeId) == 5 || (*p_D2CLIENT_RecentTradeId) == 7)
	{
		if((*p_D2CLIENT_bTradeBlock))
		{
			// Don't operate if we can't trade anyway ...
			JS_SET_RVAL(cx, vp, JSVAL_FALSE);
		}
		else if((*p_D2CLIENT_bTradeAccepted))
		{
			(*p_D2CLIENT_bTradeAccepted) = FALSE;
			D2CLIENT_CancelTrade();
			JS_SET_RVAL(cx, vp, JSVAL_TRUE);
		}
		else
		{
			(*p_D2CLIENT_bTradeAccepted) = TRUE;
			D2CLIENT_AcceptTrade();
			JS_SET_RVAL(cx, vp, JSVAL_TRUE);
		}
		return JS_TRUE;
	}

	THROW_ERROR(cx, "Invalid parameter passed to acceptTrade!");
}

JSAPI_FUNC(my_tradeOk)
{	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	CriticalMisc myMisc;
	TransactionDialogsInfo_t* pTdi = *p_D2CLIENT_pTransactionDialogsInfo;
	unsigned int i;

	myMisc.EnterSection();

	if(pTdi != NULL)
	{
		for(i = 0; i < pTdi->numLines; ++i)
		{
			// Not sure if *p_D2CLIENT_TransactionDialogs == 1 necessary if it's in
			// the dialog list, but if it's not 1, a crash is guaranteed. (CrazyCasta)
			if(pTdi->dialogLines[i].handler == D2CLIENT_TradeOK &&
				*p_D2CLIENT_TransactionDialogs == 1)
			{
				D2CLIENT_TradeOK();
				return JS_TRUE;
			}
		}
	}

	THROW_ERROR(cx, "Not in proper state to click ok to trade.");
}

JSAPI_FUNC(my_getDialogLines)
{
	CriticalMisc myMisc;
	TransactionDialogsInfo_t* pTdi = *p_D2CLIENT_pTransactionDialogsInfo;
	unsigned int i;
	JSObject* pReturnArray;
	JSObject* line;
	jsval js_text, js_selectable, js_line;
	char* ansi_text;

	myMisc.EnterSection();

	JS_EnterLocalRootScope(cx);

	if(pTdi != NULL)
	{
		pReturnArray = JS_NewArrayObject(cx, pTdi->numLines, NULL);
		for(i = 0; i < pTdi->numLines; ++i)
		{
			ansi_text = UnicodeToAnsi(pTdi->dialogLines[i].text);
			js_text = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, ansi_text));
			delete[] ansi_text;

			js_selectable = BOOLEAN_TO_JSVAL(pTdi->dialogLines[i].bMaybeSelectable);

			line = BuildObject(cx);
			JS_SetProperty(cx, line, "text", &js_text);
			JS_SetProperty(cx, line, "selectable", &js_selectable);

			js_line = OBJECT_TO_JSVAL(line);
			JS_SetElement(cx, pReturnArray, i, &js_line);
		}

		JS_SET_RVAL(cx,vp, OBJECT_TO_JSVAL(pReturnArray));
	}

	JS_LeaveLocalRootScope(cx);

	return JS_TRUE;
}

JSAPI_FUNC(my_getPath)
{	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	if(argc < 5)
		THROW_ERROR(cx, "Not enough parameters were passed to getPath!");

	CriticalRoom myMisc;
	myMisc.EnterSection();

	uint lvl = 0, x = 0, y = 0, dx = 0, dy = 0, reductionType = 0, radius = 20;

	JS_BeginRequest(cx);
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "uuuuu/uu", &lvl, &x, &y, &dx, &dy, &reductionType, &radius))
	{
		JS_EndRequest(cx);
		return JS_FALSE;
	}
	JS_EndRequest(cx);
	if(reductionType == 3 &&
		!(JSVAL_IS_FUNCTION(cx, JS_ARGV(cx, vp)[7]) && JSVAL_IS_FUNCTION(cx, JS_ARGV(cx, vp)[8]) && JSVAL_IS_FUNCTION(cx, JS_ARGV(cx, vp)[9])))
		THROW_ERROR(cx, "Invalid function values for reduction type");

	if (lvl == 0)
		THROW_ERROR(cx, "Invalid level passed to getPath");
	Level* level = GetLevel(lvl);
	
	ActMap* map = ActMap::GetMap(level);

	Point start(x, y), end(dx, dy);

	PathReducer* reducer = NULL;
	switch(reductionType)
	{
		case 0: reducer = new WalkPathReducer(map, DiagonalShortcut, radius); break;
		case 1: reducer = new TeleportPathReducer(map, DiagonalShortcut, radius); break;
		case 2: reducer = new NoPathReducer(map); break;
		case 3: reducer = new JSPathReducer(map, cx, JS_THIS_OBJECT(cx, vp), JS_ARGV(cx, vp)[7], JS_ARGV(cx, vp)[8], JS_ARGV(cx, vp)[9]); break;
		default: THROW_ERROR(cx, "Invalid path reducer value!"); break;
	}

	PointList list;
#if defined(_TIME)
	AStarPath<TimedAlloc<Node, std::allocator<Node> > > path(map, reducer);
#else
	AStarPath<> path(map, reducer);
#endif

	//box18jsrefcount depth = JS_SuspendRequest(cx);
	
	path.GetPath(start, end, list, true);
	map->CleanUp();
	
	//box18JS_ResumeRequest(cx, depth);
#if defined(_TIME)
	char p[510];
	sprintf_s(p, 510, "%s\\stats.txt", Vars.szPath);
	FILE* f;
	fopen_s(&f, p, "a+");
	path.GetAllocator().DumpStats(f);
	fclose(f);
#endif

	int count = list.size();

	JS_EnterLocalRootScope(cx);

	jsval* vec = new jsval[count];
	for(int i = 0; i < count; i++)
	{
		jsval jx = INT_TO_JSVAL(list[i].first),
			  jy = INT_TO_JSVAL(list[i].second);

		JSObject* point = BuildObject(cx);
		JS_SetProperty(cx, point, "x", &jx);
		JS_SetProperty(cx, point, "y", &jy);

		vec[i] = OBJECT_TO_JSVAL(point);
	}

	JSObject* arr = JS_NewArrayObject(cx, count, vec);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(arr));
	
	JS_LeaveLocalRootScope(cx);

	delete reducer;

	return JS_TRUE;
}

JSAPI_FUNC(my_getCollision)
{	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	uint32 nLevelId, nX, nY;
	JS_BeginRequest(cx);
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "uuu", &nLevelId, &nX, &nY))
	{
		JS_EndRequest(cx);
		return JS_FALSE;
	}
	JS_EndRequest(cx);
	CriticalRoom myMisc;
	myMisc.EnterSection();


	Point point(nX, nY);
	Level* level = GetLevel(nLevelId);

	ActMap* map = ActMap::GetMap(level);
	if(!map->IsValidPoint(point))
		THROW_ERROR(cx, "Invalid point!");

	jsval rval;
	JS_BeginRequest(cx);
	JS_NewNumberValue(cx, map->GetMapData(point, true), &rval);
	JS_EndRequest(cx);
	JS_SET_RVAL(cx, vp,rval );
	map->CleanUp();
	
	return JS_TRUE;
}

JSAPI_FUNC(my_clickItem)
{
	typedef void __fastcall clickequip(UnitAny * pPlayer, Inventory * pIventory, int loc);

	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	CriticalMisc myMisc;
	myMisc.EnterSection();

	if(*p_D2CLIENT_TransactionDialog != 0 || *p_D2CLIENT_TransactionDialogs != 0 || *p_D2CLIENT_TransactionDialogs_2 != 0)
	{
		JS_SET_RVAL(cx, vp, JSVAL_FALSE);
		return JS_TRUE;
	}

	myUnit* pmyUnit = NULL;
	UnitAny* pUnit = NULL;

	//int ScreenSize = D2GFX_GetScreenSize();

	POINT Belt[] =
	{
		{0,0}, // 0
		{1,0}, // 1
		{2,0}, // 2
		{3,0}, // 3

		{0,1}, // 4
		{1,1}, // 5
		{2,1}, // 6
		{3,1}, // 7

		{0,2}, // 8
		{1,2}, // 9
		{2,2}, // 10
		{3,2}, // 11

		{0,3}, // 12
		{1,3}, // 13
		{2,3}, // 14
		{3,3}, // 15
	};

	*p_D2CLIENT_CursorHoverX = 0xFFFFFFFF;
	*p_D2CLIENT_CursorHoverY = 0xFFFFFFFF;
	
	if(argc == 1 && JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[0]))
	{
		pmyUnit = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]));
		
		if(!pmyUnit || (pmyUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
			return JS_TRUE;

		pUnit = D2CLIENT_FindUnit(pmyUnit->dwUnitId, pmyUnit->dwType);

		if(!pUnit)
			return JS_TRUE;

		clickequip * click = (clickequip*)*(DWORD*)(D2CLIENT_BodyClickTable + (4 * pUnit->pItemData->BodyLocation));
		
		if(!click)
			return JS_TRUE;

		click(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory, pUnit->pItemData->BodyLocation);
	
		return JS_TRUE;
	}
	else if(argc == 2 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) && JSVAL_IS_INT(JS_ARGV(cx, vp)[1]))
	{
		jsint nClickType = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
		jsint nBodyLoc = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);


		if(nClickType == NULL)
		{
			clickequip * click = (clickequip*)*(DWORD*)(D2CLIENT_BodyClickTable + (4 * nBodyLoc));
			
			if(!click)
				return JS_TRUE;

			click(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory, nBodyLoc);	
		}
		// Click Merc Gear
		else if(nClickType == 4)
		{
			if(nBodyLoc == 1 || nBodyLoc == 3 || nBodyLoc == 4)
			{
				UnitAny* pMerc = D2CLIENT_GetMercUnit();

				if(pMerc)
				{
					D2CLIENT_MercItemAction(0x61, nBodyLoc);
				}
			}
		}

		return JS_TRUE;
	}
	else if(argc == 2 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) && JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[1]))
	{
		pmyUnit = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[1]));
		
		if(!pmyUnit || (pmyUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
			return JS_TRUE;

		pUnit = D2CLIENT_FindUnit(pmyUnit->dwUnitId, pmyUnit->dwType);

		jsint nClickType = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);

		if(!pUnit || !(pUnit->dwType == UNIT_ITEM) || !pUnit->pItemData)
			THROW_ERROR(cx, "Object is not an item!");

		int InventoryLocation = GetItemLocation(pUnit);
		int ClickLocation = LOCATION_NULL;
		
		int x = pUnit->pItemPath->dwPosX;
		int y = pUnit->pItemPath->dwPosY;

		*p_D2CLIENT_CursorHoverX = x;
		*p_D2CLIENT_CursorHoverY = y;

		InventoryLayout* pLayout = NULL;

		if(nClickType == 4)
		{
			UnitAny* pMerc = D2CLIENT_GetMercUnit();

			if(pMerc)
				if(pUnit->pItemData && pUnit->pItemData->pOwner)
					if(pUnit->pItemData->pOwner->dwUnitId == pMerc->dwUnitId)
					{
						D2CLIENT_MercItemAction(0x61, pUnit->pItemData->BodyLocation);
					}

			return JS_TRUE;
		}
		else if(InventoryLocation == LOCATION_INVENTORY || InventoryLocation == LOCATION_STASH || InventoryLocation == LOCATION_CUBE)
		{
			switch(InventoryLocation)
			{
				case LOCATION_INVENTORY:
					pLayout = (InventoryLayout*)p_D2CLIENT_InventoryLayout;
					ClickLocation = CLICKTARGET_INVENTORY;
					break;
				case LOCATION_STASH:
					pLayout = (InventoryLayout*)p_D2CLIENT_StashLayout;
					ClickLocation = CLICKTARGET_STASH;
					break;
				case LOCATION_CUBE:
					pLayout = (InventoryLayout*)p_D2CLIENT_CubeLayout;
					ClickLocation = CLICKTARGET_CUBE;
					break;
			}

			x = pLayout->Left + x * pLayout->SlotPixelWidth + 10; 
			y = pLayout->Top + y * pLayout->SlotPixelHeight + 10;

			if(nClickType == NULL)
				D2CLIENT_LeftClickItem(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory, x, y, nClickType, pLayout, ClickLocation);
				//D2CLIENT_LeftClickItem(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory, x, y, nClickType, pLayout, pUnit->pItemData->ItemLocation);
			else
				D2CLIENT_RightClickItem(x,y, ClickLocation, D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory);
				//D2CLIENT_RightClickItem(x,y, pUnit->pItemData->ItemLocation, D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory);

		}
		else if(InventoryLocation == LOCATION_BELT)
		{
			int i = x;

			if( i < 0 || i > 0x0F)
				return JS_TRUE;

			if(D2GFX_GetScreenSize() == 2)
			{
				x = 440 + (Belt[i].x * 29);
				y = 580 - (Belt[i].y * 29);
			}
			else {
				x = 360 + (Belt[i].x * 29);
				y = 460 - (Belt[i].y * 29);
			}
			if(nClickType == NULL)
				D2CLIENT_ClickBelt(x, y, D2CLIENT_GetPlayerUnit()->pInventory);
			else
				D2CLIENT_ClickBeltRight(D2CLIENT_GetPlayerUnit()->pInventory, D2CLIENT_GetPlayerUnit(), nClickType == 1 ? FALSE : TRUE, i);
		}
		else if(D2CLIENT_GetCursorItem() == pUnit)
		{
			if(nClickType < 1 || nClickType > 12)
				return JS_TRUE;

			clickequip * click = (clickequip*)*(DWORD*)(D2CLIENT_BodyClickTable + (4 * nClickType));

			if(!click)
				return JS_TRUE;
			
			click(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory, nClickType);			
		}
	}
	else if(argc == 4)
	{
		if(JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) && JSVAL_IS_INT(JS_ARGV(cx, vp)[1]) && JSVAL_IS_INT(JS_ARGV(cx, vp)[2]) && JSVAL_IS_INT(JS_ARGV(cx, vp)[3]))
		{
			jsint nButton = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
			jsint nX = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);
			jsint nY = JSVAL_TO_INT(JS_ARGV(cx, vp)[2]);
			jsint nLoc = JSVAL_TO_INT(JS_ARGV(cx, vp)[3]);

			int clickTarget = LOCATION_NULL;
			InventoryLayout* pLayout = NULL;

			*p_D2CLIENT_CursorHoverX = nX;
			*p_D2CLIENT_CursorHoverY = nY;

			// Fixing the x/y click spot for items taking more than one inventory square- so Diablo can handle it!
			if(nLoc != LOCATION_BELT) 
			{
				UnitAny* pItem = D2CLIENT_GetCursorItem();
				if(pItem)
				{
					ItemTxt* pTxt = D2COMMON_GetItemText(pItem->dwTxtFileNo);
					if(pTxt)
					{
						if(pTxt->ySize > 1)
							nY += 1;

						if(pTxt->xSize > 1)
							nX += 1;
					}
					
				}
			}
			//nLoc is click target locations=: LOCATION_INVENTORY=inventory, LOCATION_TRADE=player trade, LOCATION_CUBE=cube, LOCATION_STASH=stash, LOCATION_BELT=belt
			if(nLoc == LOCATION_INVENTORY || nLoc == LOCATION_TRADE || nLoc == LOCATION_CUBE || nLoc == LOCATION_STASH)
			{
				switch(nLoc)
				{
					case LOCATION_INVENTORY:
						pLayout = (InventoryLayout*)p_D2CLIENT_InventoryLayout;
						clickTarget = CLICKTARGET_INVENTORY;
						break;
					case LOCATION_TRADE:
						pLayout = (InventoryLayout*)p_D2CLIENT_TradeLayout;
						clickTarget = CLICKTARGET_TRADE;
						break;
					case LOCATION_CUBE:
						pLayout = (InventoryLayout*)p_D2CLIENT_CubeLayout;
						clickTarget = CLICKTARGET_CUBE;
						break;
					case LOCATION_STASH:
						pLayout = (InventoryLayout*)p_D2CLIENT_StashLayout;
						clickTarget = CLICKTARGET_STASH;
						break;
				}

				int	x = pLayout->Left + nX * pLayout->SlotPixelWidth + 10; 
				int	y = pLayout->Top + nY * pLayout->SlotPixelHeight + 10;
				
				if(nButton == 0) // Left Click
					D2CLIENT_LeftClickItem(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory, x, y, 1, pLayout, clickTarget);
				else if(nButton == 1) // Right Click
					D2CLIENT_RightClickItem(x, y, clickTarget, D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory);
				else if(nButton == 2) // Shift Left Click
					D2CLIENT_LeftClickItem(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory, x, y, 5, pLayout, clickTarget);
			
				return JS_TRUE;
			}
			else if(nLoc == LOCATION_BELT) // Belt
			{

				int z = -1;

				for(UINT i = 0; i < ArraySize(Belt); i++)
				{
					if(Belt[i].x == nX && Belt[i].y == nY)
					{
						z = (int)i;
						break;
					}
				}

				if(z == -1)
					return JS_TRUE;

				int x = NULL;
				int y = NULL;

				if(D2GFX_GetScreenSize() == 2)
				{
					x = 440 + (Belt[z].x * 29);
					y = 580 - (Belt[z].y * 29);
				}
				else {
					x = 360 + (Belt[z].x * 29);
					y = 460 - (Belt[z].y * 29);
				}

				if(nButton == 0)
					D2CLIENT_ClickBelt(x, y, D2CLIENT_GetPlayerUnit()->pInventory);	
				else if(nButton == 1)
					D2CLIENT_ClickBeltRight(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory, FALSE, z);
				else if(nButton == 2)
					D2CLIENT_ClickBeltRight(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory, TRUE, z);

				return JS_TRUE;
			}	
		}
	}

	return JS_TRUE;
}


JSAPI_FUNC(my_getLocaleString)
{
	if(argc < 1 || !JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		return JS_TRUE;

	uint16 localeId;
	JS_BeginRequest(cx);
	JS_ValueToUint16(cx, JS_ARGV(cx, vp)[0], &localeId);
	wchar_t* wString = D2LANG_GetLocaleText(localeId);
	char* szTmp = UnicodeToAnsi(wString);
	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, szTmp)));
	JS_EndRequest(cx);
	delete[] szTmp;
	
	return JS_TRUE;
}

JSAPI_FUNC(my_rand)
{
	if(argc < 2 || !JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) || !JSVAL_IS_INT(JS_ARGV(cx, vp)[1]))
	{
		JS_SET_RVAL(cx, vp, INT_TO_JSVAL(0));
		return JS_TRUE;
	}

	// only seed the rng once
	static bool seeded = false;
	if(!seeded)
	{
		srand(GetTickCount());
		seeded = true;
	}

	long long seed = 0;
	if(ClientState() == ClientStateInGame)
		seed = D2GAME_Rand(D2CLIENT_GetPlayerUnit()->dwSeed);
	else
		seed = rand();

	jsint high;
	jsint low;
	JS_BeginRequest(cx);
	if(JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp),"ii", &low, &high) == JS_FALSE)
	{
		JS_EndRequest(cx);
		THROW_ERROR(cx, "Could not convert Rand aruments");
	}
	JS_EndRequest(cx);

	if(high > low+1)
	{
		int i = (seed % (high - low + 1)) + low;
		JS_SET_RVAL(cx, vp, INT_TO_JSVAL(i));
	}
	else
		JS_SET_RVAL(cx, vp, INT_TO_JSVAL(high));
	
	return JS_TRUE;
}

JSAPI_FUNC(my_getDistance)
{	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	// TODO: Add the type of distance to the api design
	jsint nX1 = NULL;
	jsint nX2 = NULL;
	jsint nY1 = NULL;
	jsint nY2 = NULL;

	if(argc == 1 && JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[0])) 
	{
		jsval x1, y1;
		if(JS_GetProperty(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]), "x", &x1) && JS_GetProperty(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]), "y", &y1))
		{ 
			nX1 = D2CLIENT_GetUnitX(D2CLIENT_GetPlayerUnit());
			nY1 = D2CLIENT_GetUnitY(D2CLIENT_GetPlayerUnit());
			JS_BeginRequest(cx);
			JS_ValueToECMAInt32(cx, x1, &nX2);
			JS_ValueToECMAInt32(cx, y1, &nY2);
			JS_EndRequest(cx);
		}
	}
	else if(argc == 2)
	{ 
		if (JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) && JSVAL_IS_INT(JS_ARGV(cx, vp)[1])) 
		{
			nX1 = D2CLIENT_GetUnitX(D2CLIENT_GetPlayerUnit());
			nY1 = D2CLIENT_GetUnitY(D2CLIENT_GetPlayerUnit());
			JS_BeginRequest(cx);
			JS_ValueToECMAInt32(cx, JS_ARGV(cx, vp)[0], &nX2);
			JS_ValueToECMAInt32(cx, JS_ARGV(cx, vp)[1], &nY2);
			JS_EndRequest(cx);
		}
		else if(JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[0]) && JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[1])) 
		{			
			jsval x, y, x2, y2; 
			if(JS_GetProperty(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]), "x", &x) && JS_GetProperty(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]), "y", &y) &&
				JS_GetProperty(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[1]), "x", &x2) && JS_GetProperty(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[1]), "y", &y2)) 
			{ 
				JS_BeginRequest(cx);
				JS_ValueToECMAInt32(cx, x, &nX1);
				JS_ValueToECMAInt32(cx, y, &nY1);
				JS_ValueToECMAInt32(cx, x2, &nX2);
				JS_ValueToECMAInt32(cx, y2, &nY2);
				JS_EndRequest(cx);
			}			
		}
	}
	else if(argc == 3)
	{
		if(JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[0]) && JSVAL_IS_INT(JS_ARGV(cx, vp)[1]) && JSVAL_IS_INT(JS_ARGV(cx, vp)[2]))
		{
			myUnit* pUnit1 = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]));

			if(!pUnit1 || (pUnit1->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
				return JS_TRUE;	

			UnitAny* pUnitA = D2CLIENT_FindUnit(pUnit1->dwUnitId, pUnit1->dwType);

			if(!pUnitA)
				return JS_TRUE;

			nX1 = D2CLIENT_GetUnitX(pUnitA);
			nY1 = D2CLIENT_GetUnitY(pUnitA);
			JS_BeginRequest(cx);
			JS_ValueToECMAInt32(cx, JS_ARGV(cx, vp)[1], &nX2);
			JS_ValueToECMAInt32(cx, JS_ARGV(cx, vp)[2], &nY2);
			JS_EndRequest(cx);
		}
		else if(JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) && JSVAL_IS_INT(JS_ARGV(cx, vp)[1]) && JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[2]))
		{
			myUnit* pUnit1 = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[2]));

			if(!pUnit1 || (pUnit1->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
				return JS_TRUE;	

			UnitAny* pUnitA = D2CLIENT_FindUnit(pUnit1->dwUnitId, pUnit1->dwType);

			if(!pUnitA)
				return JS_TRUE;

			nX1 = D2CLIENT_GetUnitX(pUnitA);
			nY1 = D2CLIENT_GetUnitY(pUnitA);
			JS_BeginRequest(cx);
			JS_ValueToECMAInt32(cx, JS_ARGV(cx, vp)[0], &nX2);
			JS_ValueToECMAInt32(cx, JS_ARGV(cx, vp)[1], &nY2);
			JS_EndRequest(cx);
		}
	}
	else if(argc == 4)
	{
		if(JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) && JSVAL_IS_INT(JS_ARGV(cx, vp)[1]) && JSVAL_IS_INT(JS_ARGV(cx, vp)[2]) && JSVAL_IS_INT(JS_ARGV(cx, vp)[3]))
		{
			JS_BeginRequest(cx);
			JS_ValueToECMAInt32(cx, JS_ARGV(cx, vp)[0], &nX1);
			JS_ValueToECMAInt32(cx, JS_ARGV(cx, vp)[1], &nY1);
			JS_ValueToECMAInt32(cx, JS_ARGV(cx, vp)[2], &nX2);
			JS_ValueToECMAInt32(cx, JS_ARGV(cx, vp)[3], &nY2);
			JS_EndRequest(cx);
		}
	}
	jsdouble jsdist = (jsdouble)abs(GetDistance(nX1, nY1, nX2, nY2));
	jsval rval;
	JS_BeginRequest(cx);
	JS_NewNumberValue(cx, jsdist, &rval);
	JS_EndRequest(cx);
	JS_SET_RVAL(cx, vp, rval);
	return JS_TRUE;
}

JSAPI_FUNC(my_gold)
{
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	jsint nGold = NULL;
	jsint nMode = 1;

	if(argc > 0 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		nGold = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);

	if(argc > 1 && JSVAL_IS_INT(JS_ARGV(cx, vp)[1]))
		nMode = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);

	SendGold(nGold, nMode);
	return JS_TRUE;
}

JSAPI_FUNC(my_checkCollision)
{	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	if(argc == 3 && JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[0]) && JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[1]) && JSVAL_IS_INT(JS_ARGV(cx, vp)[2]))
	{
		myUnit*	pUnitA = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]));
		myUnit*	pUnitB = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[1]));
		jsint nBitMask = JSVAL_TO_INT(JS_ARGV(cx, vp)[2]);

		if(!pUnitA || (pUnitA->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT || !pUnitB || (pUnitB->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
			return JS_TRUE;

		UnitAny* pUnit1 = D2CLIENT_FindUnit(pUnitA->dwUnitId, pUnitA->dwType);
		UnitAny* pUnit2 = D2CLIENT_FindUnit(pUnitB->dwUnitId, pUnitB->dwType);

		if(!pUnit1 || !pUnit2)
			return JS_TRUE;
		
		JS_SET_RVAL(cx, vp,INT_TO_JSVAL(D2COMMON_CheckUnitCollision(pUnit1, pUnit2, (WORD)nBitMask)) );
		return JS_TRUE;
	}

	return JS_TRUE;
}

JSAPI_FUNC(my_getCursorType)
{
	jsint nType = NULL;
	
	if(argc > 0)
		nType = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);

	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(nType == 1 ? *p_D2CLIENT_ShopCursorType : *p_D2CLIENT_RegularCursorType));

	return JS_TRUE;
}

JSAPI_FUNC(my_getSkillByName)
{
	if(argc < 1 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		return JS_TRUE;

	char *lpszText = JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
	if(!(lpszText && lpszText[0]))
		THROW_ERROR(cx, "Could not convert string");

	for(int i = 0; i < ArraySize(Game_Skills); i++)
	{
		if(!_strcmpi(Game_Skills[i].name, lpszText))
		{
			JS_SET_RVAL(cx, vp, INT_TO_JSVAL(Game_Skills[i].skillID));
			return JS_TRUE;
		}
	}

	return JS_TRUE;
}

JSAPI_FUNC(my_getSkillById)
{
	if(argc < 1 || !JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		return JS_TRUE;

	jsint nId = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
	JS_BeginRequest(cx);
	JS_SET_RVAL(cx, vp,  STRING_TO_JSVAL(JS_NewStringCopyZ(cx, "Unknown")));
	JS_EndRequest(cx);
	int row = 0;
	if(FillBaseStat("skills", nId, "skilldesc", &row, sizeof(int)))
		if(FillBaseStat("skilldesc", row, "str name", &row, sizeof(int)))
		{
			wchar_t* szName = D2LANG_GetLocaleText((WORD)row);
			char* str = UnicodeToAnsi(szName);
			JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, str)));
			delete[] str;
		}

	return JS_TRUE;
}

JSAPI_FUNC(my_getTextSize)
{
	if(argc < 2 || !JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]) || !JSVAL_IS_INT(JS_ARGV(cx, vp)[1]))
	{
		JS_SET_RVAL(cx, vp, JSVAL_FALSE);
		return JS_TRUE;
	}

	char* pString = JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
	if(!pString)
		THROW_ERROR(cx, "Could not convert string");

	POINT r = CalculateTextLen(pString, JSVAL_TO_INT(JS_ARGV(cx, vp)[1]));
	jsval x = INT_TO_JSVAL(r.x);
	jsval y = INT_TO_JSVAL(r.y);

	JSObject* pObj = NULL;
	if(argc > 2 && JSVAL_IS_BOOLEAN(JS_ARGV(cx, vp)[2]) && JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[2]) == TRUE)
	{
		// return an object with a height/width rather than an array
		pObj = BuildObject(cx, NULL);
		if(!pObj)
			THROW_ERROR(cx, "Could not build object");
		JS_AddRoot(cx, &pObj);
		if(JS_SetProperty(cx, pObj, "width", &x) == JS_FALSE)
			THROW_ERROR(cx, "Could not set width property");
		if(JS_SetProperty(cx, pObj, "height", &y) == JS_FALSE)
			THROW_ERROR(cx, "Could not set height property");
	}
	else
	{
		JS_BeginRequest(cx);
		pObj = JS_NewArrayObject(cx, NULL, NULL);
		JS_AddRoot(cx, &pObj);
		JS_SetElement(cx, pObj, 0, &x);
		JS_SetElement(cx, pObj, 1, &y);
		JS_EndRequest(cx);
	}
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(pObj));
	JS_RemoveRoot(cx, &pObj);
	JS_free(cx, pString);
	return JS_TRUE;
}

JSAPI_FUNC(my_getTradeInfo)
{
	// set default return value to false 
	JS_SET_RVAL(cx, vp, JSVAL_FALSE);
	
	if(argc < 1)
		return JS_TRUE;

	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	if(!JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		return JS_TRUE;

	jsint nMode = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
	
	switch( nMode )
	{
		case 0:
			JS_SET_RVAL(cx, vp, INT_TO_JSVAL((*p_D2CLIENT_RecentTradeId)));
			break;
		case 1:
			{
			//FIXME
			//char* tmp = UnicodeToAnsi((wchar_t*)(*p_D2CLIENT_RecentTradeName));
			//*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, tmp));
			//delete[] tmp;
			//Temporary return value to keep it kosher
			JS_SET_RVAL(cx, vp,JSVAL_NULL );
			}
			break;
		case 2:
			JS_SET_RVAL(cx, vp, INT_TO_JSVAL((*p_D2CLIENT_RecentTradeId)));
			break;
		default:
			JS_SET_RVAL(cx, vp, JSVAL_FALSE);
			break;
	}
	return JS_TRUE;
}

JSAPI_FUNC(my_getUIFlag)
{
	if(argc < 1 || !JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
	{
		JS_SET_RVAL(cx, vp, JSVAL_VOID);
		return JS_TRUE;
	}

	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	jsint nUIId = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(D2CLIENT_GetUIState(nUIId)));
	
	return JS_TRUE;
}

JSAPI_FUNC(my_getWaypoint)
{	
	if(argc < 1 || !JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
	{
		JS_SET_RVAL(cx, vp, JSVAL_FALSE);
		return JS_TRUE;
	}

	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	jsint nWaypointId = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);

	if(nWaypointId > 40)
		nWaypointId = NULL;

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(!!D2COMMON_CheckWaypoint((*p_D2CLIENT_WaypointTable), nWaypointId)));
	
	return JS_TRUE;
}

JSAPI_FUNC(my_quitGame)
{
	if(ClientState() != ClientStateMenu)
		D2CLIENT_ExitGame();

	// give the core a chance to shut down
	Shutdown();
	TerminateProcess(GetCurrentProcess(), 0);

	return JS_TRUE;
}

JSAPI_FUNC(my_quit)
{
	if(ClientState() != ClientStateMenu)
		D2CLIENT_ExitGame();

	return JS_TRUE;
}

JSAPI_FUNC(my_playSound)
{
// I need to take a closer look at the D2CLIENT_PlaySound function
	if(argc < 1 || !JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
	{
		JS_SET_RVAL(cx, vp, JSVAL_FALSE);
		return JS_TRUE;
	}

	jsint nSoundId = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
	D2CLIENT_PlaySound(nSoundId);

	JS_SET_RVAL(cx, vp, JSVAL_TRUE);
	return JS_TRUE;
}

JSAPI_FUNC(my_say)
{
	for(uintN i = 0; i < argc; i++)
	{
		if(!JSVAL_IS_NULL(JS_ARGV(cx, vp)[i]))
		{
			JS_BeginRequest(cx);
			if(!JS_ConvertValue(cx, JS_ARGV(cx, vp)[i], JSTYPE_STRING, &(JS_ARGV(cx, vp)[i])))
			{
				JS_EndRequest(cx);
				JS_ReportError(cx, "Converting to string failed");
				return JS_FALSE;
			}

			char* Text = JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[i]));
			if(Text == NULL)
			{
				JS_EndRequest(cx);
				JS_ReportError(cx, "Could not get string for value");
				return JS_FALSE;
			}
			JS_EndRequest(cx);
			//box18jsrefcount depth = JS_SuspendRequest(cx);
			if(Text)
				Say(Text);
			//box18JS_ResumeRequest(cx, depth);
			JS_free(cx, Text);
		}
	}

	return JS_TRUE;
}

JSAPI_FUNC(my_clickParty)
{	
	JS_SET_RVAL(cx, vp, JSVAL_FALSE);

	if(argc < 2 || !JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[0]) || !JSVAL_IS_INT(JS_ARGV(cx, vp)[1]))
		return JS_TRUE;

	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	UnitAny* myUnit = D2CLIENT_GetPlayerUnit();
	RosterUnit* pUnit = (RosterUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]));
	RosterUnit* mypUnit = *p_D2CLIENT_PlayerUnitList;

	if(!pUnit || !mypUnit)
		return JS_TRUE;

	BOOL bFound = FALSE;

	for(RosterUnit* pScan = mypUnit; pScan; pScan = pScan->pNext)
		if(pScan->dwUnitId == pUnit->dwUnitId)
			bFound = TRUE;

	if(!bFound)
		return JS_TRUE;

	jsint nMode = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);

	BnetData* pData = (*p_D2LAUNCH_BnData);

	// Attempt to loot player, check first if it's hardcore
	if(nMode == 0 && pData && !(pData->nCharFlags & PLAYER_TYPE_HARDCORE))
		return JS_TRUE;

	// Attempt to party a player who is already in party ^_~
	if(nMode == 2 && pUnit->wPartyId != 0xFFFF && mypUnit->wPartyId == pUnit->wPartyId && pUnit->wPartyId != 0xFFFF)
		return JS_TRUE;

	// don't leave a party if we are in none!
	if(nMode == 3 && pUnit->wPartyId == 0xFFFF)
		return JS_TRUE;
	else if(nMode == 3 && pUnit->wPartyId != 0xFFFF) {
		JS_SET_RVAL(cx, vp, JSVAL_TRUE);
		D2CLIENT_LeaveParty();
		return JS_TRUE;	
	}

	if(nMode < 0 || nMode > 3)
		return JS_TRUE;
	
	// Trying to click self
	if (pUnit->dwUnitId == myUnit->dwUnitId)
		return JS_TRUE;

	if(nMode == 1)
		D2CLIENT_HostilePartyUnit(pUnit, 1);
	else
		D2CLIENT_ClickParty(pUnit, nMode);
	
	JS_SET_RVAL(cx, vp, JSVAL_TRUE);
	
	return JS_TRUE;
}

JSAPI_FUNC(my_useStatPoint)
{
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	WORD stat = 0;
	int32 count = 1;
	JS_BeginRequest(cx);
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "c/u", &stat, &count))
	{
		JS_EndRequest(cx);
		return JS_FALSE;
	}
	JS_EndRequest(cx);
	UseStatPoint(stat, count);
	return JS_TRUE;
}

JSAPI_FUNC(my_useSkillPoint)
{
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	WORD skill = 0;
	int32 count = 1;
	JS_BeginRequest(cx);
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "c/u", &skill, &count))
	{
		JS_EndRequest(cx);
		return JS_FALSE;
	}
	JS_EndRequest(cx);
	UseSkillPoint(skill, count);
	return JS_TRUE;
}

JSAPI_FUNC(my_getBaseStat)
{
	if(argc > 2)
	{
		char *szStatName = NULL, *szTableName = NULL;
		jsint nBaseStat = 0;
		jsint nClassId = 0;
		jsint nStat = -1;
		JS_BeginRequest(cx);
		if(JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		{
			szTableName = JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
			if(!szTableName)
			{
				JS_EndRequest(cx);
				THROW_ERROR(cx, "Invalid table value");
				
			}
		}
		else if(JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[0]))
			JS_ValueToECMAInt32(cx, JS_ARGV(cx, vp)[0], &nBaseStat);
		else
		{
			JS_EndRequest(cx);
			THROW_ERROR(cx, "Invalid table value");
		}

		JS_ValueToECMAInt32(cx, JS_ARGV(cx, vp)[1], &nClassId);

		if(JSVAL_IS_STRING(JS_ARGV(cx, vp)[2]))
		{
			szStatName = JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[2]));
			if(!szStatName)
			{
				JS_EndRequest(cx);
				THROW_ERROR(cx, "Invalid column value");
			}
		}
		else if(JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[2]))
			JS_ValueToECMAInt32(cx, JS_ARGV(cx, vp)[2], &nStat);
		else
		{
			JS_EndRequest(cx);
			THROW_ERROR(cx, "Invalid column value");
		}
		jsval rval;
		FillBaseStat(cx, &rval, nBaseStat, nClassId, nStat, szTableName, szStatName);
		JS_SET_RVAL(cx, vp, rval);
		JS_EndRequest(cx);
	}

	return JS_TRUE;
}

JSAPI_FUNC(my_weaponSwitch)
{	
	JS_SET_RVAL(cx, vp, JSVAL_FALSE);
	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	jsint nParameter = NULL;
	if(argc > 0)
	{
		JS_BeginRequest(cx);
		if(JS_ValueToInt32(cx, JS_ARGV(cx, vp)[0], &nParameter) == JS_FALSE)
		{
			JS_EndRequest(cx);
			THROW_ERROR(cx, "Could not convert value");
		}
		JS_EndRequest(cx);
	}
	
	if(nParameter == NULL)
	{
		// don't perform a weapon switch if current gametype is classic
		BnetData* pData = (*p_D2LAUNCH_BnData);
		if(pData)
		{
			if(!(pData->nCharFlags & PLAYER_TYPE_EXPAC))
				return JS_TRUE;
		}
		else
			THROW_ERROR(cx, "Could not acquire BnData");

		BYTE aPacket[1];
		aPacket[0] = 0x60;
		D2NET_SendPacket(1, 1, aPacket);
		JS_SET_RVAL(cx, vp, JSVAL_TRUE);
	}
	else
		JS_SET_RVAL(cx, vp,  INT_TO_JSVAL((*p_D2CLIENT_bWeapSwitch)));
		
	return JS_TRUE;
}

JSAPI_FUNC(my_transmute)
{
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	bool cubeOn = !!D2CLIENT_GetUIState(UI_CUBE);
	if(!cubeOn)
		D2CLIENT_SetUIState(UI_CUBE, TRUE);

	D2CLIENT_Transmute();

	if(!cubeOn)
		D2CLIENT_SetUIState(UI_CUBE, FALSE);

	return JS_TRUE;
}

JSAPI_FUNC(my_getPlayerFlag)
{
	if(argc != 3 || !JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[0]) || !JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[1]) || !JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[2]))
		return JS_TRUE;
	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	uint32 nFirstUnitId = (uint32)-1;
	uint32 nSecondUnitId = (uint32)-1;
	JS_BeginRequest(cx);
	JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[0], &nFirstUnitId);
	JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[1], &nSecondUnitId);
	JS_EndRequest(cx);
	
	DWORD nFlag = JSVAL_TO_INT(JS_ARGV(cx, vp)[2]);
	
	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(D2CLIENT_TestPvpFlag(nFirstUnitId, nSecondUnitId, nFlag)));

	return JS_TRUE;
}

JSAPI_FUNC(my_getMouseCoords)
{
	bool nFlag = false, nReturn = false;

	if(argc > 0 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) || JSVAL_IS_BOOLEAN(JS_ARGV(cx, vp)[0]))
		nFlag = !!JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[0]);
	if(argc > 1 && JSVAL_IS_INT(JS_ARGV(cx, vp)[1]) || JSVAL_IS_BOOLEAN(JS_ARGV(cx, vp)[1]))
		nReturn = !!JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[1]);

	JSObject* pObj = NULL;

	POINT Coords = {*p_D2CLIENT_MouseX, *p_D2CLIENT_MouseY};

	if(nFlag)
	{
		Coords.x += *p_D2CLIENT_ViewportX;
		Coords.y += *p_D2CLIENT_ViewportY;

		D2COMMON_AbsScreenToMap(&Coords.x, &Coords.y);
	}

	jsval jsX = INT_TO_JSVAL(Coords.x);
	jsval jsY = INT_TO_JSVAL(Coords.y);

	if(nReturn)
	{
		pObj = BuildObject(cx, NULL);
		JS_AddRoot(cx, &pObj);
		if(!pObj)
			THROW_ERROR(cx, "Could not build object");
		if(JS_SetProperty(cx, pObj, "x", &jsX) == JS_FALSE)
			THROW_ERROR(cx, "Could not set x property");
		if(JS_SetProperty(cx, pObj, "y", &jsY) == JS_FALSE)
			THROW_ERROR(cx, "Could not set y property");
	}
	else
	{
		JS_BeginRequest(cx);
		pObj = JS_NewArrayObject(cx, NULL, NULL);
		JS_AddRoot(cx, &pObj);
		JS_SetElement(cx, pObj, 0, &jsX);
		JS_SetElement(cx, pObj, 1, &jsY);
		JS_EndRequest(cx);
	}

	if(!pObj)
		return JS_TRUE;

	JS_SET_RVAL(cx, vp,  OBJECT_TO_JSVAL(pObj));
	JS_RemoveRoot(cx, &pObj);
	return JS_TRUE;
}

JSAPI_FUNC(my_submitItem)
{
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	if(UnitAny* pUnit = D2CLIENT_GetCursorItem())
	{
		if(D2CLIENT_GetPlayerUnit()->dwAct == 1)
		{
			if(GetPlayerArea() == D2CLIENT_GetPlayerUnit()->pAct->pMisc->dwStaffTombLevel)
			{
				*p_D2CLIENT_CursorItemMode = 3;
				BYTE aPacket[17] = { NULL };
				aPacket[0] = 0x44;
				*(DWORD*)&aPacket[1] = D2CLIENT_GetPlayerUnit()->dwUnitId;
				*(DWORD*)&aPacket[5] = *p_D2CLIENT_OrificeId;
				*(DWORD*)&aPacket[9] = pUnit->dwUnitId;
				*(DWORD*)&aPacket[13] = 3;
				D2NET_SendPacket(17, 1, aPacket);
				JS_SET_RVAL(cx, vp, JSVAL_TRUE);
			}
			else
				JS_SET_RVAL(cx, vp, JSVAL_FALSE);
		}
		else if(D2CLIENT_GetPlayerUnit()->dwAct == 1 || D2CLIENT_GetPlayerUnit()->dwAct == 5)
		{
			if(*p_D2CLIENT_RecentInteractId && D2COMMON_IsTownByLevelNo(GetPlayerArea()))
			{
				D2CLIENT_SubmitItem(pUnit->dwUnitId);
				JS_SET_RVAL(cx, vp, JSVAL_TRUE);
			}
			else
				JS_SET_RVAL(cx, vp, JSVAL_FALSE);
			}
		else
			JS_SET_RVAL(cx, vp, JSVAL_FALSE);
		}
	else
		JS_SET_RVAL(cx, vp, JSVAL_FALSE);
		
	return JS_TRUE;
}

JSAPI_FUNC(my_getIsTalkingNPC)
{
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(IsScrollingText()));
	return JS_TRUE;
}

JSAPI_FUNC(my_getInteractedNPC)
{
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	UnitAny* pNPC = D2CLIENT_GetCurrentInteractingNPC();
	if(!pNPC)
	{
		JS_SET_RVAL(cx, vp, JSVAL_FALSE);
		return JS_TRUE;
	}

	myUnit* pmyUnit = new myUnit; // leaked?
	if(!pmyUnit)
		return JS_TRUE;

	char szName[256] = "";
	pmyUnit->_dwPrivateType = PRIVATE_UNIT;
	pmyUnit->dwClassId = pNPC->dwTxtFileNo;
	pmyUnit->dwMode = pNPC->dwMode;
	pmyUnit->dwType = pNPC->dwType;
	pmyUnit->dwUnitId = pNPC->dwUnitId;
	strcpy_s(pmyUnit->szName, sizeof(pmyUnit->szName), szName);

	JSObject *jsunit = BuildObject(cx, &unit_class, unit_methods, unit_props, pmyUnit);
	if(!jsunit)
		return JS_TRUE;

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsunit));
	return JS_TRUE;
}

JSAPI_FUNC(my_takeScreenshot)
{
	D2WIN_TakeScreenshot();
	return JS_TRUE;
}
JSAPI_FUNC(my_moveNPC)
{   
	if(!WaitForGameReady())
        THROW_WARNING(cx, "Game not ready");

	if(!Vars.bEnableUnsupported)
	{
		THROW_WARNING(cx, "moveNPC requires EnableUnsupported = true in d2bs.ini");
		return JS_TRUE;
	}

    if(argc   < 2)
        THROW_ERROR(cx, "Not enough parameters were passed to moveNPC!");

	JS_SET_RVAL(cx, vp, JSVAL_FALSE);
 
    myUnit* pNpc =   (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]));
 
    if(!pNpc || pNpc->dwType !=1)
        THROW_ERROR(cx, "Invalid NPC passed to moveNPC!");

    DWORD dwX = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);
    DWORD dwY = JSVAL_TO_INT(JS_ARGV(cx, vp)[2]);

    if(!WaitForGameReady())
        THROW_WARNING(cx, "Game not ready");

    BYTE aPacket[17];
    aPacket[0] = 0x59;
    *(DWORD*)&aPacket[1] =  pNpc->dwType;  
    *(DWORD*)&aPacket[5] =   pNpc->dwUnitId;
    *(DWORD*)&aPacket[9] =  dwX;
    *(DWORD*)&aPacket[13] = dwY;

    D2NET_SendPacket(sizeof(aPacket), 1, aPacket);
	JS_SET_RVAL(cx, vp, JSVAL_TRUE);
    return JS_TRUE;
}