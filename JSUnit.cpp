#include "JSUnit.h"
#include "D2Helpers.h"
#include "Constants.h"
#include "Helpers.h"
#include "Unit.h"
#include "Core.h"
#include "CriticalSections.h"
#include "D2Skills.h"
#include "MPQStats.h"

EMPTY_CTOR(unit)

void unit_finalize(JSFreeOp *fop, JSObject *obj)
{
	Private* lpUnit = (Private*)JS_GetPrivate(obj);

	if(lpUnit)
	{
		switch(lpUnit->dwPrivateType)
		{
			case PRIVATE_UNIT:
			{
				myUnit* unit = (myUnit*)lpUnit;
				delete unit;
				break;
			}
			case PRIVATE_ITEM:
			{
				invUnit* unit = (invUnit*)lpUnit;
				delete unit;
				break;
			}
		}
	}
	JS_SetPrivate(obj, NULL);
}

//JSBool unit_equal(JSContext *cx, JSObject *obj, jsval v, JSBool *bp) // not 1.8 compatable
//{
//	if(ClientState() == ClientStateInGame) {
//		*bp = JS_InstanceOf(cx, obj, &unit_class_ex.base, NULL);
//		myUnit* one = (myUnit*)JS_GetInstancePrivate(cx, obj, &unit_class_ex.base, NULL);
//				myUnit* two = (myUnit*)JS_GetInstancePrivate(cx, JSVAL_TO_OBJECT(v), &unit_class_ex.base, NULL);
//		UnitAny* pUnit1 = D2CLIENT_FindUnit(one->dwUnitId, one->dwType);
//		UnitAny* pUnit2 = D2CLIENT_FindUnit(two->dwUnitId, two->dwType);
//		if(!pUnit1 || !pUnit2 || pUnit1->dwUnitId != pUnit2->dwUnitId)
//			*bp = JS_FALSE;
//	}
//	return JS_TRUE;
//}

JSAPI_PROP(unit_getProperty)
{	
	BnetData* pData = *p_D2LAUNCH_BnData;
	GameStructInfo* pInfo = *p_D2CLIENT_GameInfo;
	jsval ID;
	JS_IdToValue(cx,id,&ID);
	JS_BeginRequest(cx);
	switch(JSVAL_TO_INT(ID))
	{
		case ME_PID:
			JS_NewNumberValue(cx, (jsdouble)GetCurrentProcessId(), vp);			
			break;
		case ME_PROFILE:
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, Vars.szProfile));
			break;
		case ME_GAMEREADY:
			*vp = BOOLEAN_TO_JSVAL(GameReady());
			break;
		case ME_ACCOUNT:
			if(!pData)
				return JS_TRUE;
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, pData->szAccountName));
			break;
		case ME_CHARNAME:
			if(!pInfo)
				return JS_TRUE;
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, pInfo->szCharName));
			break;
		case ME_CHICKENHP:
			*vp = INT_TO_JSVAL(Vars.nChickenHP);
			break;
		case ME_CHICKENMP:
			*vp = INT_TO_JSVAL(Vars.nChickenMP);
			break;
		case ME_DIFF:
			*vp = INT_TO_JSVAL(D2CLIENT_GetDifficulty());
			break;
		case ME_MAXDIFF:
			*vp = INT_TO_JSVAL(pData->nMaxDiff);
			break;
		case ME_GAMENAME:
			if(!pInfo)
				return JS_TRUE;
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, pInfo->szGameName));
			break;
		case ME_GAMEPASSWORD:
			if(!pInfo)
				return JS_TRUE;
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, pInfo->szGamePassword));
			break;
		case ME_GAMESERVERIP:
			if(!pInfo)
				return JS_TRUE;
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, pInfo->szGameServerIp));
			break;
		case ME_GAMESTARTTIME:
			JS_NewNumberValue(cx, (jsdouble)Vars.dwGameTime, vp);
			break;
		case ME_GAMETYPE:
			*vp = INT_TO_JSVAL(*p_D2CLIENT_ExpCharFlag);
			break;
		case ME_PLAYERTYPE:
			if(pData)
				*vp = BOOLEAN_TO_JSVAL(!!(pData->nCharFlags & PLAYER_TYPE_HARDCORE));
			break;
		case ME_ITEMONCURSOR:
			*vp = BOOLEAN_TO_JSVAL(!!D2CLIENT_GetCursorItem());
			break;
		case ME_LADDER:
			if(pData)
				*vp = BOOLEAN_TO_JSVAL(!!(pData->nCharFlags & PLAYER_TYPE_LADDER));
			break;
		case ME_QUITONHOSTILE:
			*vp = BOOLEAN_TO_JSVAL(Vars.bQuitOnHostile);
			break;
		case ME_REALM:
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, pData->szRealmName));
			break;
		case ME_REALMSHORT:
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, pData->szRealmName2));
			break;
		case OOG_SCREENSIZE:
			*vp = INT_TO_JSVAL(D2GFX_GetScreenSize());
			break;
		case OOG_WINDOWTITLE:
			char szTitle[128];
			GetWindowText(D2GFX_GetHwnd(), szTitle, 128);
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, szTitle));
			break;
		case ME_PING:
			*vp = INT_TO_JSVAL(*p_D2CLIENT_Ping);
			break;
		case ME_FPS:
			*vp = INT_TO_JSVAL(*p_D2CLIENT_FPS);
			break;
		case OOG_INGAME:
			*vp = (ClientState() == ClientStateMenu ? JSVAL_FALSE : JSVAL_TRUE);
			break;
		case OOG_QUITONERROR:
			*vp = BOOLEAN_TO_JSVAL(Vars.bQuitOnError);
			break;
		case OOG_MAXGAMETIME:
			*vp = INT_TO_JSVAL(Vars.dwMaxGameTime);
			break;
		case ME_MERCREVIVECOST:
			*vp = INT_TO_JSVAL((*p_D2CLIENT_MercReviveCost));
			break;
		case ME_BLOCKKEYS:
			*vp = BOOLEAN_TO_JSVAL(Vars.bBlockKeys);
			break;
		case ME_BLOCKMOUSE:
			*vp = BOOLEAN_TO_JSVAL(Vars.bBlockMouse);
			break;
		default:
			break;
	}
	JS_EndRequest(cx);
	if(ClientState() != ClientStateInGame)
		return JS_TRUE;
	//JSObject* obj ;
	//JS_ValueToObject(cx,  JS_CALLEE(cx,vp),obj);
	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, obj);
	if(!lpUnit || (lpUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);
	if(!pUnit)
		return JS_TRUE;
	Room1* pRoom = NULL;
	
	switch(JSVAL_TO_INT(ID))
	{
		case UNIT_TYPE:
			*vp = INT_TO_JSVAL(pUnit->dwType);
			break;
		case UNIT_CLASSID:
			*vp = INT_TO_JSVAL(pUnit->dwTxtFileNo);
			break;
		case UNIT_MODE:
			*vp = INT_TO_JSVAL(pUnit->dwMode);
			break;
		case UNIT_NAME:
			{
				char tmp[128] = "";
				GetUnitName(pUnit, tmp, 128);
				*vp = STRING_TO_JSVAL(JS_InternString(cx, tmp));
			}
			break;
		case ME_MAPID:
			*vp = INT_TO_JSVAL(*p_D2CLIENT_MapId);
			break;
		case ME_NOPICKUP:
			*vp = BOOLEAN_TO_JSVAL(!!*p_D2CLIENT_NoPickUp);
			break;
		case UNIT_ACT:
			*vp = INT_TO_JSVAL(pUnit->dwAct + 1);
			break;
		case UNIT_AREA:
			pRoom = D2COMMON_GetRoomFromUnit(pUnit);
			if(pRoom && pRoom->pRoom2 && pRoom->pRoom2->pLevel)
				*vp = INT_TO_JSVAL(pRoom->pRoom2->pLevel->dwLevelNo);
			break;
		case UNIT_ID:
			JS_BeginRequest(cx);
			JS_NewNumberValue(cx, (jsdouble)pUnit->dwUnitId, vp);
			JS_EndRequest(cx);
			break;
		case UNIT_XPOS:
			*vp = INT_TO_JSVAL(D2CLIENT_GetUnitX(pUnit));
			break;
		case UNIT_YPOS:
			*vp = INT_TO_JSVAL(D2CLIENT_GetUnitY(pUnit));
			break;
		case UNIT_TARGETX:
			switch(pUnit->dwType)
			{
				case 0:
				case 1:
				case 3:
					*vp = INT_TO_JSVAL(pUnit->pPath->xTarget);
			}
			break;
		case UNIT_TARGETY:
			switch(pUnit->dwType)
			{
				case 0:
				case 1:
				case 3:
					*vp = INT_TO_JSVAL(pUnit->pPath->yTarget);
			}
			break;
		case UNIT_HP:
			*vp = INT_TO_JSVAL(D2COMMON_GetUnitStat(pUnit, 6, 0) >> 8);
			break;
		case UNIT_HPMAX:
			*vp = INT_TO_JSVAL(D2COMMON_GetUnitStat(pUnit, 7, 0) >> 8);
			break;
		case UNIT_MP:
			*vp = INT_TO_JSVAL(D2COMMON_GetUnitStat(pUnit, 8, 0) >> 8);
			break;
		case UNIT_MPMAX:
			*vp = INT_TO_JSVAL(D2COMMON_GetUnitStat(pUnit, 9, 0) >> 8);
			break;
		case UNIT_STAMINA:
			*vp = INT_TO_JSVAL(D2COMMON_GetUnitStat(pUnit, 10, 0) >> 8);
			break;
		case UNIT_STAMINAMAX:
			*vp = INT_TO_JSVAL(D2COMMON_GetUnitStat(pUnit, 11, 0) >> 8);
			break;
		case UNIT_CHARLVL:
			*vp = INT_TO_JSVAL(D2COMMON_GetUnitStat(pUnit, 12, 0));
			break;
		case ME_RUNWALK:
			 if(pUnit == D2CLIENT_GetPlayerUnit())
				*vp = INT_TO_JSVAL(*p_D2CLIENT_AlwaysRun);
			break;
		case UNIT_SPECTYPE:
			DWORD SpecType;
			SpecType = NULL;
			if(pUnit->dwType == UNIT_MONSTER && pUnit->pMonsterData)
			{
				if(pUnit->pMonsterData->fMinion & 1)
					SpecType |= 0x08;
				if(pUnit->pMonsterData->fBoss & 1)
					SpecType |= 0x04;
				if(pUnit->pMonsterData->fChamp & 1)
					SpecType |= 0x02;
				if((pUnit->pMonsterData->fBoss & 1) && (pUnit->pMonsterData->fNormal & 1))
					SpecType |= 0x01;
				if(pUnit->pMonsterData->fNormal & 1)
					SpecType |= 0x00;
				*vp = INT_TO_JSVAL(SpecType);
				return JS_TRUE;
			}
			break;
		case UNIT_UNIQUEID:
			if(pUnit->dwType == UNIT_MONSTER && pUnit->pMonsterData->fBoss && pUnit->pMonsterData->fNormal)
				*vp = INT_TO_JSVAL(pUnit->pMonsterData->wUniqueNo);
			else
				*vp = INT_TO_JSVAL(-1);
			break;
		case ITEM_CODE: // replace with better method if found
			if(!(pUnit->dwType == UNIT_ITEM) && pUnit->pItemData)
				break;
			ItemTxt* pTxt;
			pTxt = D2COMMON_GetItemText(pUnit->dwTxtFileNo);
			if(!pTxt) {
				*vp = STRING_TO_JSVAL(JS_InternString(cx, "Unknown"));
				return JS_TRUE;
			}
			char szCode[4];
			memcpy(szCode, pTxt->szCode, 3);
			szCode[3] = 0x00;
			*vp = STRING_TO_JSVAL(JS_InternString(cx, szCode));
			break;
		case ITEM_PREFIX:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData)
				if (D2COMMON_GetItemMagicalMods(pUnit->pItemData->wMagicPrefix[0]))
					*vp = STRING_TO_JSVAL(JS_InternString(cx, D2COMMON_GetItemMagicalMods(pUnit->pItemData->wMagicPrefix[0])));
			break;
		case ITEM_SUFFIX:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData)
				if (D2COMMON_GetItemMagicalMods(pUnit->pItemData->wMagicSuffix[0]))
					*vp = STRING_TO_JSVAL(JS_InternString(cx, D2COMMON_GetItemMagicalMods(pUnit->pItemData->wMagicSuffix[0])));
			break;
		case ITEM_PREFIXNUM:
				if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData)
					*vp = INT_TO_JSVAL(pUnit->pItemData->wMagicPrefix[0]);
			break;
		case ITEM_SUFFIXNUM:
				if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData)
					*vp = INT_TO_JSVAL(pUnit->pItemData->wMagicSuffix[0]);
			break;

		case ITEM_PREFIXES:  
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData)
			{
				JSObject* pReturnArray = JS_NewArrayObject(cx, 0, NULL);
				
				for (int i = 0; i < 3; i++)
				{
					if (D2COMMON_GetItemMagicalMods(pUnit->pItemData->wMagicPrefix[i]))
					{
						jsval nPrefix = STRING_TO_JSVAL(JS_InternString(cx, D2COMMON_GetItemMagicalMods(pUnit->pItemData->wMagicPrefix[i])));

						JS_SetElement(cx, pReturnArray, i, &nPrefix);
					}
				}

				*vp = OBJECT_TO_JSVAL(pReturnArray);
			}

			break;
		case ITEM_PREFIXNUMS:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData)
			{
				JSObject* pReturnArray = JS_NewArrayObject(cx, 0, NULL);

				for (int i = 0; i < 3; i++)
				{
					if (pUnit->pItemData->wMagicPrefix[i])
					{
						jsval nPrefixnum = INT_TO_JSVAL(pUnit->pItemData->wMagicPrefix[i]);

						JS_SetElement(cx, pReturnArray, i, &nPrefixnum);
					}
				}

				*vp = OBJECT_TO_JSVAL(pReturnArray);
			}

			break;
		case ITEM_SUFFIXES:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData)
			{
				JSObject* pReturnArray = JS_NewArrayObject(cx, 0, NULL);
				
				for (int i = 0; i < 3; i++)
				{
					if (D2COMMON_GetItemMagicalMods(pUnit->pItemData->wMagicSuffix[i]))
					{
						jsval nSuffix = STRING_TO_JSVAL(JS_InternString(cx, D2COMMON_GetItemMagicalMods(pUnit->pItemData->wMagicSuffix[i])));

						JS_SetElement(cx, pReturnArray, i, &nSuffix);
					}
				}

				*vp = OBJECT_TO_JSVAL(pReturnArray);
			}

			break;
		case ITEM_SUFFIXNUMS:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData)
			{
				JSObject* pReturnArray = JS_NewArrayObject(cx, 0, NULL);

				for (int i = 0; i < 3; i++)
				{
					if (pUnit->pItemData->wMagicSuffix[i])
					{
						jsval nSuffixnum = INT_TO_JSVAL(pUnit->pItemData->wMagicSuffix[i]);

						JS_SetElement(cx, pReturnArray, i, &nSuffixnum);
					}
				}

				*vp = OBJECT_TO_JSVAL(pReturnArray);
			}

			break;


		case ITEM_FNAME:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData) {
				wchar_t wszfname[256] = L"";
				D2CLIENT_GetItemName(pUnit, wszfname, sizeof(wszfname));
				if(wszfname) {
					char* tmp = UnicodeToAnsi(wszfname);
					*vp = STRING_TO_JSVAL(JS_InternString(cx, tmp));
					delete[] tmp;
					tmp = NULL;
				}
			}
			break;
		case ITEM_QUALITY:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData)
				*vp = INT_TO_JSVAL(pUnit->pItemData->dwQuality);
			break;
		case ITEM_NODE:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData)
				*vp = INT_TO_JSVAL(pUnit->pItemData->NodePage);
			break;
		case ITEM_LOC:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData)
				*vp = INT_TO_JSVAL(pUnit->pItemData->GameLocation);
			break;
		case ITEM_SIZEX:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData) {
				if(!D2COMMON_GetItemText(pUnit->dwTxtFileNo))
					break;
				*vp = INT_TO_JSVAL(D2COMMON_GetItemText(pUnit->dwTxtFileNo)->xSize);
			}
			break;
		case ITEM_SIZEY:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData) {
				if(!D2COMMON_GetItemText(pUnit->dwTxtFileNo))
					break;
				*vp = INT_TO_JSVAL(D2COMMON_GetItemText(pUnit->dwTxtFileNo)->ySize);
			}
			break;
		case ITEM_TYPE:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData) {
				if(!D2COMMON_GetItemText(pUnit->dwTxtFileNo))
					break;
				*vp = INT_TO_JSVAL(D2COMMON_GetItemText(pUnit->dwTxtFileNo)->nType);
			}
			break;
		case ITEM_DESC:
			{
				if(pUnit->dwType != UNIT_ITEM)
					break;

				wchar_t wBuffer[2048] = L"";
				wchar_t bBuffer[1] = {1};

				::WriteProcessMemory(GetCurrentProcess(), (void*)0x6FBCCB1C, bBuffer, 1, NULL);  
				::WriteProcessMemory(GetCurrentProcess(), (void*)0x6FBCCB28, &pUnit, 4, NULL);  

				//D2CLIENT_LoadItemDesc(D2CLIENT_GetPlayerUnit(), 0);				
				D2CLIENT_LoadItemDesc(pUnit->pItemData->pOwnerInventory->pOwner, 0);
				ReadProcessBYTES(GetCurrentProcess(), 0x6F9A9E68, wBuffer, 2047); 

				char *tmp = UnicodeToAnsi(wBuffer);
				if(tmp)
				{
					*vp = STRING_TO_JSVAL(JS_InternString(cx, tmp));
					delete[] tmp;
					tmp = NULL;
				}
			}

			break;
		case ITEM_GFX:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData)
				*vp = INT_TO_JSVAL(pUnit->pItemData->bInvGfxIdx);
			break;
		case UNIT_ITEMCOUNT:
			if(pUnit->pInventory)
				*vp = INT_TO_JSVAL(pUnit->pInventory->dwItemCount);
			break;
		case ITEM_BODYLOCATION:
			if(pUnit->dwType != UNIT_ITEM)
				break;
			if(pUnit->pItemData)
				*vp = INT_TO_JSVAL(pUnit->pItemData->BodyLocation);
			break;
		case UNIT_OWNER:
			JS_NewNumberValue(cx, (jsdouble)pUnit->dwOwnerId, vp);
			break;
		case UNIT_OWNERTYPE:
			*vp = INT_TO_JSVAL(pUnit->dwOwnerType);
			break;
		case ITEM_LEVEL:
			if(pUnit->dwType != UNIT_ITEM)
				break;
			if(pUnit->pItemData)
				*vp = INT_TO_JSVAL(pUnit->pItemData->dwItemLevel);
			break;
		case ITEM_LEVELREQ:
			if(pUnit->dwType != UNIT_ITEM)
				break;
			*vp = INT_TO_JSVAL(D2COMMON_GetItemLevelRequirement(pUnit, D2CLIENT_GetPlayerUnit()));
			break;
		case UNIT_DIRECTION:
			if(pUnit->pPath)
				*vp = INT_TO_JSVAL(pUnit->pPath->bDirection);
			break;
		case OBJECT_TYPE:
			if(pUnit->dwType == UNIT_OBJECT && pUnit->pObjectData)
			{
				pRoom = D2COMMON_GetRoomFromUnit(pUnit);
				if(pRoom && D2COMMON_GetLevelNoFromRoom(pRoom))
					*vp = INT_TO_JSVAL(pUnit->pObjectData->Type & 255);
				else
					*vp = INT_TO_JSVAL(pUnit->pObjectData->Type);
			}
			break;
		case OBJECT_LOCKED:
			if(pUnit->dwType == UNIT_OBJECT && pUnit->pObjectData)
				*vp = INT_TO_JSVAL( pUnit->pObjectData->ChestLocked );
			break;
		case ME_WSWITCH:
			 if(pUnit == D2CLIENT_GetPlayerUnit())
				*vp = INT_TO_JSVAL(*p_D2CLIENT_bWeapSwitch);
			break;
		default:
			break;
	}

	return JS_TRUE;
}

JSAPI_STRICT_PROP(unit_setProperty)
{
	jsval ID;
	JS_IdToValue(cx,id,&ID);
	switch(JSVAL_TO_INT(ID))
	{
		case ME_CHICKENHP:
			if(JSVAL_IS_INT(*vp))
				Vars.nChickenHP		= JSVAL_TO_INT(*vp);
			break;
		case ME_CHICKENMP:
			if(JSVAL_IS_INT(*vp))
				Vars.nChickenMP		= JSVAL_TO_INT(*vp);
			break;
		case ME_QUITONHOSTILE:
			if(JSVAL_IS_BOOLEAN(*vp))
				Vars.bQuitOnHostile = JSVAL_TO_BOOLEAN(*vp);
			break;
		case OOG_QUITONERROR:
			if(JSVAL_IS_BOOLEAN(*vp))
				Vars.bQuitOnError	= JSVAL_TO_BOOLEAN(*vp);
			break;
		case OOG_MAXGAMETIME:
			if(JSVAL_IS_INT(*vp))
				Vars.dwMaxGameTime	= JSVAL_TO_INT(*vp);
			break;
		case ME_BLOCKKEYS:
			if(JSVAL_IS_BOOLEAN(*vp))
				Vars.bBlockKeys = JSVAL_TO_BOOLEAN(*vp);
			break;
		case ME_BLOCKMOUSE:
			if(JSVAL_IS_BOOLEAN(*vp))
				Vars.bBlockMouse = JSVAL_TO_BOOLEAN(*vp);
			break;
		case ME_RUNWALK:
			{
				myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
				if(!lpUnit || (lpUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
					return JS_TRUE;

				UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);
				if(!pUnit)
					return JS_TRUE;
				if(pUnit == D2CLIENT_GetPlayerUnit())
					*p_D2CLIENT_AlwaysRun = !!JSVAL_TO_INT(*vp);
			}
			break;
		case ME_NOPICKUP:
			*p_D2CLIENT_NoPickUp = !!JSVAL_TO_INT(*vp);
			break;
	}
	return JS_TRUE;
}

JSAPI_FUNC(unit_getUnit)
{
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	if(argc < 1)
		return JS_TRUE;

	int nType = -1;
	uint32 nClassId = (uint32)-1;
	uint32 nMode = (uint32)-1;
	uint32 nUnitId = (uint32)-1;
	char szName[128] = "";

	if(argc > 0 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		nType = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);

	if(argc > 1 && JSVAL_IS_STRING(JS_ARGV(cx, vp)[1]))
		strcpy_s(szName, sizeof(szName), JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[1])));
	JS_BeginRequest(cx);
	if(argc > 1 && JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[1]) && !JSVAL_IS_NULL(JS_ARGV(cx, vp)[1]))
		JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[1], &nClassId);

	if(argc > 2 && JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[2]) && !JSVAL_IS_NULL(JS_ARGV(cx, vp)[2]))
		JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[2], &nMode);

	if(argc > 3 && JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[3]) && !JSVAL_IS_NULL(JS_ARGV(cx, vp)[3]))
		JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[3], &nUnitId);
	JS_EndRequest(cx);
	UnitAny* pUnit = NULL;
	
	if(nType == 100)
		pUnit = D2CLIENT_GetCursorItem();
	else if(nType == 101)
	{
		pUnit = D2CLIENT_GetSelectedUnit();
		if(!pUnit)
			pUnit = (*p_D2CLIENT_SelectedInvItem);
	}
	else 
		pUnit = GetUnit(szName, nClassId, nType, nMode, nUnitId);

	if(!pUnit)
		return JS_TRUE;

	myUnit* pmyUnit = new myUnit; // leaked?

	if(!pmyUnit)
		return JS_TRUE;

	pmyUnit->_dwPrivateType = PRIVATE_UNIT;
	pmyUnit->dwClassId = nClassId;
	pmyUnit->dwMode = nMode;
	pmyUnit->dwType = pUnit->dwType;
	pmyUnit->dwUnitId = pUnit->dwUnitId;
	strcpy_s(pmyUnit->szName, sizeof(pmyUnit->szName), szName);

	JSObject *jsunit = BuildObject(cx, &unit_class, unit_methods, unit_props, pmyUnit);

	if(!jsunit)
		return JS_TRUE;

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsunit));

	return JS_TRUE;
}

JSAPI_FUNC(unit_getNext)
{
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	Private* unit = (Private*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	if(!unit)
		return JS_TRUE;

	if(unit->dwPrivateType == PRIVATE_UNIT)
	{
		myUnit* lpUnit = (myUnit*)unit;
		UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

		if(!pUnit)
			return JS_TRUE;
		JS_BeginRequest(cx);
		if(argc > 0 && JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
			strcpy_s(lpUnit->szName, 128, JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0])));

		if(argc > 0 && JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[0]) && !JSVAL_IS_NULL(JS_ARGV(cx, vp)[1]))
			JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[0],(uint32*) &(lpUnit->dwClassId));

		if(argc > 1 && JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[1]) && !JSVAL_IS_NULL(JS_ARGV(cx, vp)[2]))
			JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[1],(uint32*) &(lpUnit->dwMode));
		JS_EndRequest(cx);
		pUnit = GetNextUnit(pUnit, lpUnit->szName, lpUnit->dwClassId, lpUnit->dwType, lpUnit->dwMode);

		if(!pUnit)
		{
			JSObject* obj = JS_THIS_OBJECT(cx, vp);
			JS_ClearScope(cx, obj);
			if(JS_ValueToObject(cx, JSVAL_NULL, &obj) == JS_FALSE)
				return JS_TRUE;
			JS_SET_RVAL(cx, vp, JSVAL_FALSE);
		}
		else
		{
			lpUnit->dwUnitId = pUnit->dwUnitId;
			JS_SetPrivate(cx, JS_THIS_OBJECT(cx, vp), lpUnit);
			JS_SET_RVAL(cx, vp, JSVAL_TRUE);
		}
	}
	else if(unit->dwPrivateType == PRIVATE_ITEM)
	{
		invUnit *pmyUnit = (invUnit*)unit;
		if(!pmyUnit)
			return JS_TRUE;

		UnitAny* pUnit = D2CLIENT_FindUnit(pmyUnit->dwUnitId, pmyUnit->dwType);
		UnitAny* pOwner = D2CLIENT_FindUnit(pmyUnit->dwOwnerId, pmyUnit->dwOwnerType);
		if(!pUnit || !pOwner)
			return JS_TRUE;
		JS_BeginRequest(cx);
		if(argc > 0 && JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
			strcpy_s(pmyUnit->szName, 128, JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0])));

		if(argc > 0 && JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[0]) && !JSVAL_IS_NULL(JS_ARGV(cx, vp)[1]))
			JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[0], (uint32*)&(pmyUnit->dwClassId));

		if(argc > 1 && JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[1]) && !JSVAL_IS_NULL(JS_ARGV(cx, vp)[2]))
			JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[1],(uint32*) &(pmyUnit->dwMode));
		JS_EndRequest(cx);
		UnitAny* nextItem = GetInvNextUnit(pUnit, pOwner, pmyUnit->szName, pmyUnit->dwClassId, pmyUnit->dwMode);
		if(!nextItem)
		{
			JSObject* obj = JS_THIS_OBJECT(cx, vp);
			JS_ClearScope(cx, obj);
			if(JS_ValueToObject(cx, JSVAL_NULL, &obj) == JS_FALSE)
				return JS_TRUE;
			JS_SET_RVAL(cx, vp, JSVAL_FALSE);
		}
		else
		{
			pmyUnit->dwUnitId = nextItem->dwUnitId;
			JS_SetPrivate(cx, JS_THIS_OBJECT(cx, vp), pmyUnit);
			JS_SET_RVAL(cx, vp, JSVAL_TRUE);
		}
	}

	return JS_TRUE;
}

JSAPI_FUNC(unit_cancel)
{	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");



	DWORD automapOn =*p_D2CLIENT_AutomapOn;

	if(IsScrollingText())
		D2CLIENT_ClearScreen();
	else if(D2CLIENT_GetCurrentInteractingNPC())	
		D2CLIENT_CloseNPCInteract();
	else if(D2CLIENT_GetCursorItem())
		D2CLIENT_ClickMap(0, 10, 10, 0x08);
	else
		D2CLIENT_CloseInteract();
	
	*p_D2CLIENT_AutomapOn =automapOn;
	
	return JS_TRUE;
}

JSAPI_FUNC(unit_repair)
{
	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, JSVAL_FALSE);

	if(!lpUnit || (lpUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit)
		return JS_TRUE;

	BYTE aPacket[17] = { NULL };
	aPacket[0] = 0x35;
	*(DWORD*)&aPacket[1] = *p_D2CLIENT_RecentInteractId;
	aPacket[16] = 0x80;
	D2NET_SendPacket(17,1, aPacket);

	// note: this crashes while minimized
//	D2CLIENT_PerformNpcAction(pUnit,1, NULL);
	JS_SET_RVAL(cx, vp, JSVAL_TRUE);

	return JS_TRUE;
}

JSAPI_FUNC(unit_useMenu)
{
	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, JSVAL_FALSE);

	if(argc < 1 || !JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		return JS_TRUE;

	if(!lpUnit || (lpUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit)
		return JS_TRUE;

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(ClickNPCMenu(pUnit->dwTxtFileNo, JSVAL_TO_INT(JS_ARGV(cx, vp)[0]))));

	return JS_TRUE;
}

JSAPI_FUNC(unit_interact)
{	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	JS_SET_RVAL(cx, vp, JSVAL_FALSE);

	if(!lpUnit || (lpUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit || pUnit == D2CLIENT_GetPlayerUnit())
		return JS_TRUE;

	if(pUnit->dwType == UNIT_ITEM && pUnit->dwMode != ITEM_MODE_ON_GROUND && pUnit->dwMode != ITEM_MODE_BEING_DROPPED)
	{
			int nLocation = GetItemLocation(pUnit);					
			
			BYTE aPacket[13] = {NULL};

			if(nLocation == LOCATION_INVENTORY || nLocation == LOCATION_STASH)
			{
				aPacket[0] = 0x20;
				*(DWORD*)&aPacket[1] = pUnit->dwUnitId;
				*(DWORD*)&aPacket[5] = D2CLIENT_GetPlayerUnit()->pPath->xPos;
				*(DWORD*)&aPacket[9] = D2CLIENT_GetPlayerUnit()->pPath->yPos;
				D2NET_SendPacket(13, 1, aPacket);
				return JS_TRUE;
			}
			else if(nLocation == LOCATION_BELT)
			{
				aPacket[0] = 0x26;
				*(DWORD*)&aPacket[1] = pUnit->dwUnitId;
				*(DWORD*)&aPacket[5] = 0;
				*(DWORD*)&aPacket[9] = 0;
				D2NET_SendPacket(13, 1, aPacket);
				return JS_TRUE;
			}
	}

	if(pUnit->dwType == UNIT_OBJECT && argc == 1 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
	{
		// TODO: check the range on argv[0] to make sure it won't crash the game - Done! TechnoHunter
		jsint nWaypointID;
		JS_BeginRequest(cx);
		if(!JS_ValueToECMAInt32(cx, JS_ARGV(cx, vp)[0], &nWaypointID))
		{
			JS_EndRequest(cx);
			return JS_TRUE;
		}
		JS_EndRequest(cx);
		int retVal = 0;
		if(FillBaseStat("levels", nWaypointID, "Waypoint", &retVal, sizeof(int)))
			if(retVal == 255)
				return JS_TRUE;
		
		D2CLIENT_TakeWaypoint(pUnit->dwUnitId, nWaypointID);
		if(!D2CLIENT_GetUIState(UI_GAME))
			D2CLIENT_CloseInteract();
		
		JS_SET_RVAL(cx, vp, JSVAL_TRUE);
		return JS_TRUE;
	}
//	else if(pUnit->dwType == UNIT_PLAYER && argc == 1 && JSVAL_IS_INT(argv[0]) && JSVAL_TO_INT(argv[0]) == 1)
//	{
		// Accept Trade
//	}
	else
	{
		JS_SET_RVAL(cx, vp, JSVAL_TRUE);
		ClickMap(0, D2CLIENT_GetUnitX(pUnit), D2CLIENT_GetUnitY(pUnit), FALSE, pUnit);
		//D2CLIENT_Interact(pUnit, 0x45);
	}

	return JS_TRUE;
}

void InsertStatsToGenericObject(UnitAny* pUnit, StatList* pStatList, JSContext* pJSContext, JSObject* pGenericObject);
void InsertStatsNow(Stat* pStat, int nStat, JSContext* cx, JSObject* pArray);

JSAPI_FUNC(unit_getStat)
{	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	JS_SET_RVAL(cx, vp, JSVAL_FALSE);

	if(!lpUnit || (lpUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit)
		return JS_TRUE;

	jsint nStat = 0;
	jsint nSubIndex = 0;
	JS_BeginRequest(cx);
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "i/i", &nStat, &nSubIndex))
	{
		JS_EndRequest(cx);
		return JS_TRUE;
	}
	JS_EndRequest(cx);

	if(nStat >= STAT_HP && nStat <= STAT_MAXSTAMINA)
		JS_SET_RVAL(cx, vp, INT_TO_JSVAL(D2COMMON_GetUnitStat(pUnit, nStat, nSubIndex)>>8));
	else if(nStat == STAT_EXP || nStat == STAT_LASTEXP || nStat == STAT_NEXTEXP){
		jsval rval;
		JS_NewNumberValue(cx, (unsigned int)D2COMMON_GetUnitStat(pUnit, nStat, nSubIndex), &rval);
		JS_SET_RVAL(cx,vp, rval);
	}
	else if(nStat == STAT_ITEMLEVELREQ)
		JS_SET_RVAL(cx, vp, INT_TO_JSVAL(D2COMMON_GetItemLevelRequirement(pUnit, D2CLIENT_GetPlayerUnit())));
	else if(nStat == -1)
	{
		Stat aStatList[256] = { NULL };
		StatList* pStatList = D2COMMON_GetStatList(pUnit, NULL, 0x40);

		if(pStatList)
		{
			DWORD dwStats = D2COMMON_CopyStatList(pStatList, (Stat*)aStatList, 256);

			JSObject* pReturnArray = JS_NewArrayObject(cx, 0, NULL);
			JS_SET_RVAL(cx, vp,  OBJECT_TO_JSVAL(pReturnArray));

				for (int j = 0 ; j < pUnit->pStats->StatVec.wCount; j++)
				{
					bool inListAlready = false;
					for(DWORD k = 0; k < dwStats; k++){
						if( aStatList[k].dwStatValue == pUnit->pStats->StatVec.pStats[j].dwStatValue &&
							aStatList[k].wStatIndex == pUnit->pStats->StatVec.pStats[j].wStatIndex &&
							aStatList[k].wSubIndex == pUnit->pStats->StatVec.pStats[j].wSubIndex)
							inListAlready = true;
					}
					if(!inListAlready){
						
						aStatList[dwStats].dwStatValue = pUnit->pStats->StatVec.pStats[j].dwStatValue;
						aStatList[dwStats].wStatIndex = pUnit->pStats->StatVec.pStats[j].wStatIndex;
						aStatList[dwStats].wSubIndex = pUnit->pStats->StatVec.pStats[j].wSubIndex;
						dwStats++;	
					}

				}
			for(UINT i = 0; i < dwStats; i++)
			{
				JS_BeginRequest(cx);
				JSObject* pArrayInsert = JS_NewArrayObject(cx, 0, NULL);
				JS_AddRoot(cx, &pArrayInsert);

				if(!pArrayInsert)
					continue;

				jsval nIndex	= INT_TO_JSVAL(aStatList[i].wStatIndex);
				jsval nSubIndex = INT_TO_JSVAL(aStatList[i].wSubIndex);
				jsval nValue	= INT_TO_JSVAL(aStatList[i].dwStatValue);

				JS_SetElement(cx, pArrayInsert, 0, &nIndex);
				JS_SetElement(cx, pArrayInsert, 1, &nSubIndex);	
				JS_SetElement(cx, pArrayInsert, 2, &nValue);	

				jsval aObj = OBJECT_TO_JSVAL(pArrayInsert);

				JS_SetElement(cx, pReturnArray, i, &aObj);
				JS_RemoveRoot(cx, &pArrayInsert);
				JS_EndRequest(cx);
			}
		}
	}
	else if(nStat == -2)
	{
		JSObject* pArray = JS_NewArrayObject(cx, 0, NULL);
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(pArray));

		InsertStatsToGenericObject(pUnit, pUnit->pStats, cx, pArray);
		InsertStatsToGenericObject(pUnit, D2COMMON_GetStatList(pUnit, NULL, 0x40), cx, pArray);
	//InsertStatsToGenericObject(pUnit, pUnit->pStats->pNext, cx, pArray);  // only check the current unit stats!
	//	InsertStatsToGenericObject(pUnit, pUnit->pStats->pSetList, cx, pArray);
	}
	else
	{
		long result = D2COMMON_GetUnitStat(pUnit, nStat, nSubIndex);
		if (result == 0) // if stat isnt found look up preset list
		{			
			StatList* pStatList = D2COMMON_GetStatList(pUnit, NULL, 0x40);
			Stat aStatList[256] = { NULL };
			if(pStatList)
			{
				DWORD dwStats = D2COMMON_CopyStatList(pStatList, (Stat*)aStatList, 256);
				for(UINT i = 0; i < dwStats; i++)
				{
					if (nStat == aStatList[i].wStatIndex && nSubIndex == aStatList[i].wSubIndex)	
						result = (aStatList[i].dwStatValue);
				}
			}	
		}
		jsval rval = JS_RVAL(cx,vp);
		JS_NewNumberValue(cx, result, &rval);
		JS_SET_RVAL(cx, vp, rval);
	}
	return JS_TRUE;
}

void InsertStatsToGenericObject(UnitAny* pUnit, StatList* pStatList, JSContext* cx, JSObject* pArray)
{
	Stat*	pStat = NULL;
	if(!pStatList)
		return;
	if((pStatList->dwUnitId == pUnit->dwUnitId && pStatList->dwUnitType == pUnit->dwType) || pStatList->pUnit == pUnit)
	{
		pStat = pStatList->StatVec.pStats;

		for(int nStat = 0; nStat < pStatList->StatVec.wCount; nStat++)
		{
			InsertStatsNow(pStat, nStat, cx, pArray);
		}
	}
	if((pStatList->dwFlags >> 24 & 0x80))
	{
		pStat = pStatList->SetStatVec.pStats;

		for(int nStat = 0; nStat < pStatList->SetStatVec.wCount; nStat++)
		{
			InsertStatsNow(pStat, nStat, cx, pArray);
		}
	}
}

void InsertStatsNow(Stat* pStat, int nStat, JSContext* cx, JSObject* pArray)
{
	if(pStat[nStat].wSubIndex > 0x200)
	{
		// subindex is the skill id and level
		int skill = pStat[nStat].wSubIndex >> 6,
			level = pStat[nStat].wSubIndex & 0x3F,
			charges = 0,
			maxcharges = 0;
		if(pStat[nStat].dwStatValue > 0x200)
		{
			charges = pStat[nStat].dwStatValue & 0xFF;
			maxcharges = pStat[nStat].dwStatValue >> 8;
		}
		JSObject* val = BuildObject(cx, NULL);
		jsval jsskill = INT_TO_JSVAL(skill),
			  jslevel = INT_TO_JSVAL(level),
			  jscharges = INT_TO_JSVAL(charges),
			  jsmaxcharges = INT_TO_JSVAL(maxcharges);
		// val is an anonymous object that holds properties
		if(!JS_SetProperty(cx, val, "skill", &jsskill) ||
		   !JS_SetProperty(cx, val, "level", &jslevel))
		   return;
		if(maxcharges > 0)
		{
			if(!JS_SetProperty(cx, val, "charges", &jscharges) ||
			   !JS_SetProperty(cx, val, "maxcharges", &jsmaxcharges))
			   return;
		}
		// find where we should put it
		jsval index = JSVAL_VOID,
			  obj = OBJECT_TO_JSVAL(val);
		if(!JS_GetElement(cx, pArray, pStat[nStat].wStatIndex, &index))
			return;
		if(index != JSVAL_VOID)
		{
			// modify the existing object by stuffing it into an array
			if(!JS_IsArrayObject(cx, JSVAL_TO_OBJECT(index)))
			{
				// it's not an array, build one
				JS_BeginRequest(cx);
				JSObject* arr = JS_NewArrayObject(cx, 0, NULL);
				JS_AddRoot(cx, &arr);
				JS_SetElement(cx, arr, 0, &index);
				JS_SetElement(cx, arr, 1, &obj);
				jsval arr2 = OBJECT_TO_JSVAL(arr);
				JS_SetElement(cx, pArray, pStat[nStat].wStatIndex, &arr2);
				JS_RemoveRoot(cx, &arr);
				JS_EndRequest(cx);
			}
			else
			{
				// it is an array, append the new value
				JSObject* arr = JSVAL_TO_OBJECT(index);
				jsuint len = 0;
				if(!JS_GetArrayLength(cx, arr, &len))
					return;
				len++;
				JS_BeginRequest(cx);
				JS_SetElement(cx, arr, len, &obj);
				JS_EndRequest(cx);
			}
		}
		else
			JS_BeginRequest(cx);
			JS_SetElement(cx, pArray, pStat[nStat].wStatIndex, &obj);
			JS_EndRequest(cx);
	}
	else
	{
		//Make sure to bit shift life, mana and stamina properly!
		int value = pStat[nStat].dwStatValue;
		if(pStat[nStat].wStatIndex >= 6 && pStat[nStat].wStatIndex <= 11)
			value = value >> 8;

		jsval index = JSVAL_VOID, val = INT_TO_JSVAL(value);
		if(!JS_GetElement(cx, pArray, pStat[nStat].wStatIndex, &index))
			return;
		if(index == JSVAL_VOID)
		{
			// the array index doesn't exist, make it
			index = OBJECT_TO_JSVAL(JS_NewArrayObject(cx, 0, NULL));
			JS_BeginRequest(cx);
			if(!JS_SetElement(cx, pArray, pStat[nStat].wStatIndex, &index)){
				JS_EndRequest(cx);
				return;
			}
			JS_EndRequest(cx);
		}
		// index now points to the correct array index
		JS_BeginRequest(cx);
		JS_SetElement(cx, JSVAL_TO_OBJECT(index), pStat[nStat].wSubIndex, &val);
		JS_EndRequest(cx);
	}
}

JSAPI_FUNC(unit_getState)
{	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	JS_SET_RVAL(cx, vp, JSVAL_FALSE);

	if(!lpUnit || (lpUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit || !JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		return JS_TRUE;

	jsint nState;
	JS_BeginRequest(cx);
	if(JS_ValueToInt32(cx, JS_ARGV(cx, vp)[0], &nState) == JS_FALSE)
	{
		JS_EndRequest(cx);
		return JS_TRUE;
	}
	JS_EndRequest(cx);

	// TODO: make these constants so we know what we're checking here
	if(nState > 159 || nState < 0)
		return JS_TRUE;

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(!!D2COMMON_GetUnitState(pUnit, nState)));

	return JS_TRUE;
}

JSAPI_FUNC(item_getFlags)
{	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	if(!lpUnit || (lpUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit || pUnit->dwType != UNIT_ITEM)
		return JS_TRUE;

	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(pUnit->pItemData->dwFlags));

	return JS_TRUE;
}

JSAPI_FUNC(item_getFlag)
{	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	if(argc < 1 || !JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		return JS_TRUE;

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	if(!lpUnit || (lpUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit || pUnit->dwType != UNIT_ITEM)
		return JS_TRUE;

	jsint nFlag = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(!!(nFlag & pUnit->pItemData->dwFlags)));

	return JS_TRUE;
}

//JSAPI_FUNC(item_getPrice)
//{	
//	DEPRECATED;
//
//	if(!WaitForGameReady())
//		THROW_WARNING(cx, "Game not ready");
//
//	int diff = D2CLIENT_GetDifficulty();
//	//D2COMMON_GetItemPrice(D2CLIENT_GetPlayerUnit(), pUnit, diff, *p_D2CLIENT_ItemPriceList, NPCID, buysell)
//	int buysell = 0;
//	int NPCID = 148;
//
//	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
//
//	if(!lpUnit || (lpUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
//		return JS_TRUE;
//
//	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);
//
//	if(!pUnit)
//		return JS_TRUE;
//
//	if(argc>0)
//	{
//		if(JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[0]))
//		{
//			myUnit* pmyNpc = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]));
//			
//			if(!pmyNpc || (pmyNpc->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
//				return JS_TRUE;
//
//			UnitAny* pNpc = D2CLIENT_FindUnit(pmyNpc->dwUnitId, pmyNpc->dwType);
//
//			if(!pNpc)
//				return JS_TRUE;
//
//			NPCID = pNpc->dwTxtFileNo;
//		}
//		else if(JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
//			NPCID = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
//	}
//	if(argc>1)
//		buysell = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);
//	if(argc>2)
//		diff = JSVAL_TO_INT(JS_ARGV(cx, vp)[2]);
//
//	*rval = INT_TO_JSVAL(D2COMMON_GetItemPrice(D2CLIENT_GetPlayerUnit(), pUnit, diff, *p_D2CLIENT_ItemPriceList, NPCID, buysell));
//
//	return JS_TRUE;
//}

JSAPI_FUNC(item_getItemCost)
{
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	jsint nMode;
	UnitAny* npc = D2CLIENT_GetCurrentInteractingNPC();
	jsint nNpcClassId = (npc ? npc->dwTxtFileNo : 0x9A); // defaults to Charsi's NPC id
	jsint nDifficulty = D2CLIENT_GetDifficulty();

	if(argc < 1 || !JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		return JS_TRUE;
	
	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	if(!lpUnit || (lpUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit || pUnit->dwType != UNIT_ITEM)
		return JS_TRUE;

	nMode = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);

	if(argc > 1)
	{
		if(JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[1]))
		{
			myUnit* pmyNpc = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[1]));
			
			if(!pmyNpc || (pmyNpc->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
				return JS_TRUE;

			UnitAny* pNpc = D2CLIENT_FindUnit(pmyNpc->dwUnitId, pmyNpc->dwType);

			if(!pNpc)
				return JS_TRUE;
			nNpcClassId = pNpc->dwTxtFileNo;
		}
		else if(JSVAL_IS_INT(JS_ARGV(cx, vp)[1]) && !JSVAL_IS_NULL(JS_ARGV(cx, vp)[1]))
		{
			JS_BeginRequest(cx);
			if(!JS_ValueToECMAInt32(cx, JS_ARGV(cx, vp)[1], &nNpcClassId))
			{
				JS_EndRequest(cx);
				return JS_TRUE;
			}
			JS_EndRequest(cx);
		}
		//TODO:: validate the base stat table sizes to make sure the game doesn't crash with checking values past the end of the table
		int retVal = 0;
		if(FillBaseStat("monstats", nNpcClassId, "inventory", &retVal, sizeof(int))&& retVal == 0)
			nNpcClassId = 0x9A; // invalid npcid incoming! default to charsi to allow the game to continue
	}

	if(argc > 2 && JSVAL_IS_INT(JS_ARGV(cx, vp)[2]))
		nDifficulty = JSVAL_TO_INT(JS_ARGV(cx, vp)[2]);

	switch(nMode)
	{
		case 0: // Buy
		case 1: // Sell
			JS_SET_RVAL(cx, vp, INT_TO_JSVAL(D2COMMON_GetItemPrice(D2CLIENT_GetPlayerUnit(), pUnit, nDifficulty, *p_D2CLIENT_ItemPriceList, nNpcClassId, nMode)));
			break;
		case 2: // Repair
			JS_SET_RVAL(cx, vp, INT_TO_JSVAL(D2COMMON_GetItemPrice(D2CLIENT_GetPlayerUnit(), pUnit, nDifficulty, *p_D2CLIENT_ItemPriceList, nNpcClassId, 3)));
			break;
		default:
			break;
	}

	return JS_TRUE;
}

JSAPI_FUNC(unit_getItems)
{	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	if(!lpUnit || (lpUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit || !pUnit->pInventory || !pUnit->pInventory->pFirstItem)
		return JS_TRUE;
	JS_BeginRequest(cx);
	JSObject* pReturnArray = JS_NewArrayObject(cx, 0, NULL);

	if(!pReturnArray)
	{
		JS_EndRequest(cx);
		return JS_TRUE;
	}
	JS_AddRoot(cx, &pReturnArray);

	DWORD dwArrayCount = 0;

	for(UnitAny* pItem = pUnit->pInventory->pFirstItem; pItem; pItem = D2COMMON_GetNextItemFromInventory(pItem), dwArrayCount++)
	{
		invUnit* pmyUnit = new invUnit;
		
		if(!pmyUnit)
			continue;

		pmyUnit->_dwPrivateType = PRIVATE_UNIT;
		pmyUnit->szName[0] = NULL;
		pmyUnit->dwMode = pItem->dwMode;
		pmyUnit->dwClassId = pItem->dwTxtFileNo;
		pmyUnit->dwUnitId = pItem->dwUnitId;
		pmyUnit->dwType = UNIT_ITEM;
		pmyUnit->dwOwnerId = pUnit->dwUnitId;
		pmyUnit->dwOwnerType = pUnit->dwType;

		JSObject *jsunit = BuildObject(cx, &unit_class, unit_methods, unit_props, pmyUnit);
		if(!jsunit)
		{
			JS_EndRequest(cx);
			JS_RemoveRoot(cx, &pReturnArray);
			THROW_ERROR(cx, "Failed to build item array");
		}		
		jsval a = OBJECT_TO_JSVAL(jsunit);
		JS_SetElement(cx, pReturnArray, dwArrayCount, &a);		
	}
	
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(pReturnArray));
	JS_RemoveRoot(cx, &pReturnArray);
	JS_EndRequest(cx);
	return JS_TRUE;
}


JSAPI_FUNC(unit_getSkill)
{
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	jsint nSkillId = NULL;
	jsint nExt = NULL;

	myUnit* pmyUnit = (myUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	if(!pmyUnit)
		return JS_TRUE;
	UnitAny* pUnit = D2CLIENT_FindUnit(pmyUnit->dwUnitId, pmyUnit->dwType);
	if(!pUnit)
		return JS_TRUE;

	if(argc == NULL)
		return JS_TRUE;

	if(argc == 1)
	{
		if(!JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
			return JS_TRUE;

		nSkillId = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
	}
	else if(argc == 2)
	{
		if(!JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) || !JSVAL_IS_INT(JS_ARGV(cx, vp)[1]))
			return JS_TRUE;

		nSkillId = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
		nExt = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);
	}
	if(argc == 1)
	{
		WORD wLeftSkillId = pUnit->pInfo->pLeftSkill->pSkillInfo->wSkillId;
		WORD wRightSkillId = pUnit->pInfo->pRightSkill->pSkillInfo->wSkillId;
		switch(nSkillId)
		{
			case 0:
				{
					int row = 0;
					if(FillBaseStat("skills", wRightSkillId, "skilldesc", &row, sizeof(int)))
						if(FillBaseStat("skilldesc", row, "str name", &row, sizeof(int)))
						{
							wchar_t* szName = D2LANG_GetLocaleText((WORD)row);
							char* str = UnicodeToAnsi(szName);
							JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, str)));
							delete[] str;
						}
				}
				break;
			case 1:
				{
					int row = 0;
					if(FillBaseStat("skills", wLeftSkillId, "skilldesc", &row, sizeof(int)))
						if(FillBaseStat("skilldesc", row, "str name", &row, sizeof(int)))
						{
							wchar_t* szName = D2LANG_GetLocaleText((WORD)row);
							char* str = UnicodeToAnsi(szName);
							JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, str)));
							delete[] str;
						}
				}
				break;
				
			case 2: JS_SET_RVAL(cx, vp, INT_TO_JSVAL(wRightSkillId)); break;				
			case 3: JS_SET_RVAL(cx, vp, INT_TO_JSVAL(wLeftSkillId)); break;
			case 4: {
				JSObject* pReturnArray = JS_NewArrayObject(cx, 0, NULL);
				JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(pReturnArray));
				int i = 0;
				for(Skill* pSkill = pUnit->pInfo->pFirstSkill; pSkill; pSkill = pSkill->pNextSkill) {
					JSObject* pArrayInsert = JS_NewArrayObject(cx, 0, NULL);
					JS_AddRoot(cx, &pArrayInsert);

					if(!pArrayInsert)
						continue;

					jsval nId	= INT_TO_JSVAL(pSkill->pSkillInfo->wSkillId);
					jsval nBase = INT_TO_JSVAL(pSkill->dwSkillLevel);
					jsval nTotal = INT_TO_JSVAL(D2COMMON_GetSkillLevel(pUnit, pSkill, 1));
					JS_BeginRequest(cx);
					JS_SetElement(cx, pArrayInsert, 0, &nId);
					JS_SetElement(cx, pArrayInsert, 1, &nBase);
					JS_SetElement(cx, pArrayInsert, 2, &nTotal);

					jsval aObj = OBJECT_TO_JSVAL(pArrayInsert);

					JS_SetElement(cx, pReturnArray, i, &aObj);
					JS_RemoveRoot(cx, &pArrayInsert);
					JS_EndRequest(cx);
					i++;
				}
				break;
			}
			default:
				JS_SET_RVAL(cx, vp, JSVAL_FALSE);
				break;
		}
		return JS_TRUE;
	}
	else if(argc == 2)
	{
		if(pUnit && pUnit->pInfo && pUnit->pInfo->pFirstSkill)
		{
			for(Skill* pSkill = pUnit->pInfo->pFirstSkill; pSkill; pSkill = pSkill->pNextSkill)
			{
				if(pSkill->pSkillInfo && pSkill->pSkillInfo->wSkillId == nSkillId)
				{
					JS_SET_RVAL(cx, vp, INT_TO_JSVAL(D2COMMON_GetSkillLevel(pUnit, pSkill, nExt)));
					return JS_TRUE;
				}
			}
		}

	}

	JS_SET_RVAL(cx, vp, JSVAL_FALSE);

	return JS_TRUE;
}

JSAPI_FUNC(item_shop)
{	
	CriticalMisc myMisc;
	myMisc.EnterSection();

	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	if(*p_D2CLIENT_TransactionDialog != 0 || *p_D2CLIENT_TransactionDialogs != 0 || *p_D2CLIENT_TransactionDialogs_2 != 0)
	{
		JS_SET_RVAL(cx, vp, JSVAL_FALSE);
		return JS_TRUE;
	}

	myUnit* lpItem = (myUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	if(!lpItem || (lpItem->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pItem = D2CLIENT_FindUnit(lpItem->dwUnitId, lpItem->dwType);

	if(!pItem || pItem->dwType != UNIT_ITEM)
		return JS_TRUE;

	if(!D2CLIENT_GetUIState(UI_NPCSHOP))
		return JS_TRUE;

	UnitAny* pNPC = D2CLIENT_GetCurrentInteractingNPC();
	DWORD dwMode = JSVAL_TO_INT(JS_ARGV(cx, vp)[argc - 1]);

	//Check if we are interacted.
	if(!pNPC)
		return JS_TRUE;

	//Check for proper mode.
	if ((dwMode != 1) && (dwMode != 2) && (dwMode != 6))
		return JS_TRUE;

	//Selling an Item 
	if(dwMode == 1)
	{
		//Check if we own the item!
		if (pItem->pItemData->pOwnerInventory->pOwner->dwUnitId != D2CLIENT_GetPlayerUnit()->dwUnitId)
			return JS_TRUE;

		D2CLIENT_ShopAction(pItem, pNPC, pNPC, 1, 0, 1, 1, NULL);
	}
	else
	{
		//Make sure the item is owned by the NPC interacted with.
		if (pItem->pItemData->pOwnerInventory->pOwner->dwUnitId != pNPC->dwUnitId)
			return JS_TRUE;

		D2CLIENT_ShopAction(pItem, pNPC, pNPC, 0, 0, dwMode, 1, NULL);
	}

	/*BYTE pPacket[17] = {NULL};

	if(dwMode == 2 || dwMode == 6)
		pPacket[0] = 0x32;
	else pPacket[0] = 0x33;

	*(DWORD*)&pPacket[1] = pNPC->dwUnitId;
	*(DWORD*)&pPacket[5] = pItem->dwUnitId;

   	if(dwMode == 1) // Sell
	{
		if(D2CLIENT_GetCursorItem() && D2CLIENT_GetCursorItem() == pItem)
			*(DWORD*)&pPacket[9] = 0x04;
		else 
			*(DWORD*)&pPacket[9] = 0;
	}
	else if(dwMode == 2) // Buy
	{
		if(pItem->pItemData->dwFlags & 0x10)
			*(DWORD*)&pPacket[9] = 0x00;
		else
			*(DWORD*)&pPacket[9] = 0x02;
	}
	else
		*(BYTE*)&pPacket[9+3] = 0x80;

	int nBuySell = NULL;

	if(dwMode == 2 || dwMode == 6)
		nBuySell = NULL;
	else nBuySell = 1;

	*(DWORD*)&pPacket[13] = D2COMMON_GetItemPrice(D2CLIENT_GetPlayerUnit(), pItem, D2CLIENT_GetDifficulty(), *p_D2CLIENT_ItemPriceList, pNPC->dwTxtFileNo, nBuySell);

	D2NET_SendPacket(sizeof(pPacket), 1, pPacket);*/
	
	JS_SET_RVAL(cx, vp, JSVAL_TRUE);

	return JS_TRUE;
}

JSAPI_FUNC(unit_getParent)
{	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	if(!lpUnit || (lpUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit)
		return JS_TRUE;

	if(pUnit->dwType == UNIT_MONSTER)
	{
		DWORD dwOwnerId = D2CLIENT_GetMonsterOwner(pUnit->dwUnitId);
		if(dwOwnerId == -1)
			return JS_TRUE;

		UnitAny* pMonster = GetUnit(NULL, (DWORD)-1, (DWORD)-1, (DWORD)-1, dwOwnerId);
		if (!pMonster)
			return JS_TRUE;

		myUnit* pmyUnit = new myUnit;
			if(!pmyUnit)
				return JS_TRUE;	

		pmyUnit->_dwPrivateType = PRIVATE_UNIT;
		pmyUnit->dwUnitId = pMonster->dwUnitId;
		pmyUnit->dwClassId = pMonster->dwTxtFileNo;
		pmyUnit->dwMode = pMonster->dwMode;
		pmyUnit->dwType = pMonster->dwType;
		pmyUnit->szName[0] = NULL;
						
		JSObject *jsunit = BuildObject(cx, &unit_class, unit_methods, unit_props, pmyUnit);
			if (!jsunit)
				return JS_TRUE;
			JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsunit));
			return JS_TRUE;
	}
	else if(pUnit->dwType == UNIT_OBJECT)
	{
		if(pUnit->pObjectData)
		{
			char szBuffer[128] = "";
			strcpy_s(szBuffer, sizeof(szBuffer), pUnit->pObjectData->szOwner);

			JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_InternString(cx, szBuffer)));
		}
	}
	else if(pUnit->dwType == UNIT_ITEM)
	{
		if(pUnit->pItemData && pUnit->pItemData->pOwnerInventory && pUnit->pItemData->pOwnerInventory->pOwner)
		{
			myUnit* pmyUnit = new myUnit; // leaks

			if(!pmyUnit)
				return JS_TRUE;

			pmyUnit->_dwPrivateType = PRIVATE_UNIT;
			pmyUnit->dwUnitId = pUnit->pItemData->pOwnerInventory->pOwner->dwUnitId;
			pmyUnit->dwClassId = pUnit->pItemData->pOwnerInventory->pOwner->dwTxtFileNo;
			pmyUnit->dwMode = pUnit->pItemData->pOwnerInventory->pOwner->dwMode;
			pmyUnit->dwType = pUnit->pItemData->pOwnerInventory->pOwner->dwType;
			pmyUnit->szName[0] = NULL;
			JSObject *jsunit = BuildObject(cx, &unit_class, unit_methods, unit_props, pmyUnit);

			JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsunit));
		}
	}

	return JS_TRUE;
}

// Works only on players sinces monsters _CANT_ have mercs!
JSAPI_FUNC(unit_getMerc)
{	
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, JSVAL_NULL);

	if(lpUnit && (lpUnit->_dwPrivateType & PRIVATE_UNIT) == PRIVATE_UNIT) {
		UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);
		if(pUnit && pUnit->dwType == UNIT_PLAYER) {
			UnitAny* pMerc = GetMercUnit(pUnit);
			if (pMerc) {
				myUnit* pmyUnit = new myUnit;

				pmyUnit->_dwPrivateType = PRIVATE_UNIT;
				pmyUnit->dwUnitId = pMerc->dwUnitId;
				pmyUnit->dwClassId = pMerc->dwTxtFileNo;
				pmyUnit->dwMode = NULL;
				pmyUnit->dwType = UNIT_MONSTER;
				pmyUnit->szName[0] = NULL;

				JSObject *jsunit = BuildObject(cx, &unit_class, unit_methods, unit_props, pmyUnit);
				if (jsunit)
					JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsunit));
			}
		}
	}
	return JS_TRUE;
}

JSAPI_FUNC(unit_getMercHP)
{	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");
	JSObject* test = JS_THIS_OBJECT(cx,vp);
	jsval* rest = 0;
	myUnit* lpUnit = (myUnit*) JS_GetInstancePrivate(cx, test, &unit_class, rest);
	//myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, test);

	UnitAny* pUnit = lpUnit ? D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType) : D2CLIENT_GetPlayerUnit();

	if (pUnit) {
		UnitAny* pMerc = GetMercUnit(pUnit);
		if (pMerc)
			JS_SET_RVAL(cx, vp, (pUnit->dwMode == 12 ? JSVAL_ZERO : INT_TO_JSVAL(D2CLIENT_GetUnitHPPercent(pMerc->dwUnitId))));
	}

	return JS_TRUE;
}

// unit.setSkill( int skillId OR String skillName, int hand [, int itemGlobalId] );
JSAPI_FUNC(unit_setskill)
{	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	WORD nSkillId = (WORD)-1;
	BOOL nHand = FALSE;
	DWORD itemId = (DWORD)-1;
	JS_SET_RVAL(cx, vp, JSVAL_FALSE);

	if(argc < 1)
		return JS_TRUE;

	if(JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		nSkillId = GetSkillByName(JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0])));
	else if(JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		nSkillId = (WORD)JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
	else
		return JS_TRUE;

	if(JSVAL_IS_INT(JS_ARGV(cx, vp)[1]))
		nHand = !!JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);
	else
		return JS_TRUE;

	if(argc == 3 && JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[2]))
	{
		JSObject* object = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[2]);
		if(JS_InstanceOf(cx, object, &unit_class, JS_ARGV(cx, vp)))
		{
			myUnit* unit = (myUnit*)JS_GetPrivate(cx, object);
			if(unit->dwType == UNIT_ITEM)
				itemId = unit->dwUnitId;
		}
	}
		

	if(SetSkill(nSkillId, nHand, itemId))
		JS_SET_RVAL(cx, vp, JSVAL_TRUE);

	return JS_TRUE;
}

JSAPI_FUNC(my_overhead)
{	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	myUnit *pmyUnit = (myUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	if(!pmyUnit || (pmyUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(pmyUnit->dwUnitId, pmyUnit->dwType);

	if(!pUnit)
		return JS_TRUE;

	if(!JSVAL_IS_NULL(JS_ARGV(cx, vp)[0]) && !JSVAL_IS_VOID(JS_ARGV(cx, vp)[0]))
	{
		char *lpszText = JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
		if(lpszText && lpszText[0])
		{
			OverheadMsg* pMsg = D2COMMON_GenerateOverheadMsg(NULL, lpszText, *p_D2CLIENT_OverheadTrigger);
			if(pMsg)
			{
				D2COMMON_FixOverheadMsg(pMsg, NULL);
				pUnit->pOMsg = pMsg;
			}
		}
		JS_free(cx, lpszText);
	}

	return JS_TRUE;
}

JSAPI_FUNC(my_revive)
{
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	BYTE pPacket[] = {0x41};
	D2NET_SendPacket(1, 1, pPacket);
	return JS_TRUE;
}


JSAPI_FUNC(unit_getItem)
{	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	myUnit *pmyUnit = (myUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	if(!pmyUnit || (pmyUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(pmyUnit->dwUnitId, pmyUnit->dwType);

	if(!pUnit || !pUnit->pInventory)
		return JS_TRUE;

	uint32 nClassId = (uint32)-1;
	uint32 nMode = (uint32)-1;
	uint32 nUnitId = (uint32)-1;
	char szName[128] = "";

	if(argc > 0 && JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		strcpy_s(szName, sizeof(szName), JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[0])));
	
	if(argc > 0 && JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[0]) && !JSVAL_IS_NULL(JS_ARGV(cx, vp)[0]))
		JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[0], &nClassId);

	if(argc > 1 && JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[1]) && !JSVAL_IS_NULL(JS_ARGV(cx, vp)[1]))
		JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[1], &nMode);

	if(argc > 2 && JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[2]) && !JSVAL_IS_NULL(JS_ARGV(cx, vp)[2]))
		JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[2], &nUnitId);

	UnitAny* pItem = GetInvUnit(pUnit, szName, nClassId, nMode, nUnitId);

	if(!pItem)
		return JS_TRUE;

	invUnit* pmyItem = new invUnit; // leaked?

	if(!pmyItem)
		return JS_TRUE;

	pmyItem->_dwPrivateType = PRIVATE_ITEM;
	pmyItem->dwClassId = nClassId;
	pmyItem->dwMode = nMode;
	pmyItem->dwType = pItem->dwType;
	pmyItem->dwUnitId = pItem->dwUnitId;
	pmyItem->dwOwnerId = pmyUnit->dwUnitId;
	pmyItem->dwOwnerType = pmyUnit->dwType;
	strcpy_s(pmyItem->szName, sizeof(pmyItem->szName), szName);

	JSObject *jsunit = BuildObject(cx, &unit_class, unit_methods, unit_props, pmyItem);

	if(!jsunit)
		return JS_TRUE;

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsunit));

	return JS_TRUE;
}

JSAPI_FUNC(unit_move)
{
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	myUnit *pmyUnit = (myUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	if(!pmyUnit || (pmyUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(pmyUnit->dwUnitId, pmyUnit->dwType);

	UnitAny *pPlayer = D2CLIENT_GetPlayerUnit();

	if(!pPlayer || !pUnit)
		return JS_TRUE;

	int32 x, y;

	if(pUnit == pPlayer)
	{

		if(argc < 2) 
			return JS_TRUE;

		if(JS_ValueToInt32(cx, JS_ARGV(cx, vp)[0], &x) == JS_FALSE)
			return JS_TRUE;
		if(JS_ValueToInt32(cx, JS_ARGV(cx, vp)[1], &y) == JS_FALSE)
			return JS_TRUE;
	}
	else
	{
		x = D2CLIENT_GetUnitX(pUnit);
		y = D2CLIENT_GetUnitY(pUnit);
	}

	ClickMap(0, (WORD)x, (WORD)y, FALSE, NULL);
	Sleep(50);
	ClickMap(2, (WORD)x, (WORD)y, FALSE, NULL);
//	D2CLIENT_Move((WORD)x, (WORD)y);
	return JS_TRUE;
}

JSAPI_FUNC(unit_getEnchant)
{
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");
	
	if(argc < 1 || !JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		return JS_TRUE;

	myUnit *pmyUnit = (myUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	if(!pmyUnit || (pmyUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(pmyUnit->dwUnitId, pmyUnit->dwType);

	if(!pUnit || pUnit->dwType != UNIT_MONSTER)
		return JS_TRUE;

	int nEnchant = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(0));

	for(int i = 0; i < 9; i++)
		if(pUnit->pMonsterData->anEnchants[i] == nEnchant)
		{
			JS_SET_RVAL(cx, vp, JSVAL_TRUE);
			break;
		}

	return JS_TRUE;
}

JSAPI_FUNC(unit_getQuest)
{
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");
	
	if(argc < 2 || !JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) || !JSVAL_IS_INT(JS_ARGV(cx, vp)[1]))
		return JS_TRUE;

	jsint nAct = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
	jsint nQuest = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);

	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(D2COMMON_GetQuestFlag(D2CLIENT_GetQuestInfo(), nAct, nQuest)));

	return JS_TRUE;
}

JSAPI_FUNC(unit_getMinionCount)
{
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");
	
	if(argc < 1 || !JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		return JS_TRUE;

	jsint nType = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);

	myUnit *pmyUnit = (myUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	if(!pmyUnit || (pmyUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(pmyUnit->dwUnitId, pmyUnit->dwType);

	if(!pUnit || (pUnit->dwType != UNIT_MONSTER && pUnit->dwType != UNIT_PLAYER))
		return JS_TRUE;

	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(D2CLIENT_GetMinionCount(pUnit, (DWORD)nType)));
	
	return JS_TRUE;
}



JSAPI_FUNC(me_getRepairCost)
{
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	UnitAny* npc = D2CLIENT_GetCurrentInteractingNPC();
	jsint nNpcClassId = (npc ? npc->dwTxtFileNo : 0x9A);

	if(argc > 0 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		nNpcClassId = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);

	JS_SET_RVAL(cx, vp,INT_TO_JSVAL(D2COMMON_GetRepairCost(NULL, D2CLIENT_GetPlayerUnit(), nNpcClassId, D2CLIENT_GetDifficulty(), *p_D2CLIENT_ItemPriceList, 0)));
 
	return JS_TRUE;
}
