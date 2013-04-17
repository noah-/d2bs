#include "JSParty.h"
#include "D2Structs.h"
#include "D2Helpers.h"
#include "D2Ptrs.h"
#include "JSUnit.h"
#include "JSGlobalClasses.h"

EMPTY_CTOR(party)

JSAPI_PROP(party_getProperty)
{
	RosterUnit* pUnit = (RosterUnit*)JS_GetPrivate(cx, obj);

	if(!pUnit)
		return JS_TRUE;

	jsval ID;
	JS_IdToValue(cx,id,&ID);
	JS_BeginRequest(cx);
	switch(JSVAL_TO_INT(ID))
	{
		case PARTY_NAME:
			vp.setString(JS_NewStringCopyZ(cx, pUnit->szName));
			break;
		case PARTY_X:
			vp.setInt32(pUnit->Xpos);
			break;
		case PARTY_Y:
			vp.setInt32(pUnit->Ypos);
			break;
		case PARTY_AREA:
			vp.setInt32(pUnit->dwLevelId);
			break;
		case PARTY_GID:
			vp.setNumber((jsdouble)pUnit->dwUnitId);
			break;
		case PARTY_LIFE:
			vp.setInt32(pUnit->dwPartyLife);
			break;
		case PARTY_CLASSID:
			vp.setInt32(pUnit->dwClassId);
			break;
		case PARTY_LEVEL:
			vp.setInt32(pUnit->wLevel);
			break;
		case PARTY_FLAG:
			vp.setInt32(pUnit->dwPartyFlags);
			break;
		case PARTY_ID:
			vp.setInt32(pUnit->wPartyId);
			break;
		default:
			break;
	}
	JS_EndRequest(cx);
	return JS_TRUE;
}

JSAPI_FUNC(party_getNext)
{
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	RosterUnit *pUnit = (RosterUnit*)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	if(!pUnit)
	{
		JS_SET_RVAL(cx, vp, INT_TO_JSVAL(0));
		return JS_TRUE;
	}

	pUnit = pUnit->pNext;

	if(pUnit)
	{
		JS_SetPrivate(cx, JS_THIS_OBJECT(cx, vp), pUnit);
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(JS_THIS_OBJECT(cx, vp)));
	}
	else
	{
		JSObject* obj = JS_THIS_OBJECT(cx, vp);
//		JS_ClearScope(cx, obj);
		if(JS_ValueToObject(cx, JSVAL_NULL, &obj))
			JS_SET_RVAL(cx, vp, INT_TO_JSVAL(0));
	}
	
	return JS_TRUE;
}

JSAPI_FUNC(my_getParty)
{	
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	JSObject* jUnit = BuildObject(cx, &party_class, party_methods, party_props, 0);
	if(!jUnit)
		return JS_TRUE;
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jUnit));
//	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsUnit));



	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	RosterUnit* pUnit = *p_D2CLIENT_PlayerUnitList;

	if(!pUnit)
		return JS_TRUE;

	if(argc == 1)
	{
		UnitAny* inUnit = NULL;
		char* nPlayerName = "";
		uint32 nPlayerId = NULL;

		if(JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
		{
			nPlayerName  = JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
		}
		else if(JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		{
			JS_BeginRequest(cx);
			if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "u", &nPlayerId))
			{
				JS_EndRequest(cx);
				THROW_ERROR(cx, "Unable to get ID");
				
			}
			JS_EndRequest(cx);
		}
		else if(JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[0]))
		{
			myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]));

			if(!lpUnit)
				return JS_TRUE;

			inUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);
			if(!inUnit)
				THROW_ERROR(cx, "Unable to get Unit");

			nPlayerId = inUnit->dwUnitId;
		}

		if(!nPlayerName && !nPlayerId)
			return JS_TRUE;

		BOOL bFound = FALSE;

		for(RosterUnit* pScan = pUnit; pScan; pScan = pScan->pNext)
		{
			if(nPlayerId && pScan->dwUnitId == nPlayerId)
			{
				bFound = TRUE;
				pUnit = pScan;
				break;
			}
			if(nPlayerName && _stricmp( pScan->szName, nPlayerName) == 0)
			{
				bFound = TRUE;
				pUnit = pScan;
				break;
			}
		}

		if(!bFound)
			return JS_TRUE;

	}
	JSObject* jsUnit = BuildObject(cx, &party_class, party_methods, party_props, pUnit);
	if(!jsUnit)
		return JS_TRUE;

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsUnit));

	return JS_TRUE;
}
