#include "JSPresetUnit.h"

#include "D2Ptrs.h"
#include "CriticalSections.h"
#include "D2Helpers.h"

EMPTY_CTOR(presetunit)

void presetunit_finalize(JSFreeOp *fop, JSObject *obj)
{
	myPresetUnit *pUnit = (myPresetUnit*)JS_GetPrivate(obj);

	if(pUnit)
	{
		JS_SetPrivate(obj, NULL);
		delete pUnit;
	}
}

JSAPI_PROP(presetunit_getProperty)
{
	myPresetUnit* pUnit = (myPresetUnit*)JS_GetPrivate(cx, obj);

	if(!pUnit)
		return JS_TRUE;

	jsval ID;
	JS_IdToValue(cx,id,&ID);
	switch(JSVAL_TO_INT(ID))
	{
		case PUNIT_TYPE:
			vp.setInt32(pUnit->dwType);
			break;
		case PUNIT_ROOMX:
			vp.setInt32(pUnit->dwRoomX);
			break;
		case PUNIT_ROOMY:
			vp.setInt32(pUnit->dwRoomY);
			break;
		case PUNIT_X:
			vp.setInt32(pUnit->dwPosX);
			break;
		case PUNIT_Y:
			vp.setInt32(pUnit->dwPosY);
			break;
		case PUNIT_ID:
			vp.setInt32(pUnit->dwId);
			break;
		case PUINT_LEVEL:
			vp.setInt32(pUnit->dwLevel);
		default:
			break;
	}
	return JS_TRUE;
}

JSAPI_FUNC(my_getPresetUnits)
{
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	if(argc < 1)
	{
		JS_SET_RVAL(cx, vp, JSVAL_FALSE);
		return JS_TRUE;
	}

	uint32 levelId;
	JS_BeginRequest(cx);
	JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[0], &levelId);
	JS_EndRequest(cx);
	Level* pLevel = GetLevel(levelId);

	if(!pLevel)
		THROW_ERROR(cx, "getPresetUnits failed, couldn't access the level!");

	uint nClassId = NULL;
	uint nType = NULL;

	if(argc >= 2)
		nType = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);
	if(argc >= 3)
		nClassId = JSVAL_TO_INT(JS_ARGV(cx, vp)[2]);

	CriticalRoom cRoom;
	cRoom.EnterSection();

	bool bAddedRoom = FALSE;
	DWORD dwArrayCount = NULL;

	JSObject* pReturnArray = JS_NewArrayObject(cx, 0, NULL);
	JS_BeginRequest(cx);
	JS_AddRoot(cx, &pReturnArray);
	for(Room2 *pRoom = pLevel->pRoom2First; pRoom; pRoom = pRoom->pRoom2Next)
	{
		bAddedRoom = FALSE;

		if(!pRoom->pPreset)
		{
			D2COMMON_AddRoomData(D2CLIENT_GetPlayerUnit()->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
			bAddedRoom = TRUE;
		}
		
		for(PresetUnit* pUnit = pRoom->pPreset; pUnit; pUnit = pUnit->pPresetNext)
		{
			// Does it fit?
			if((nType == NULL || pUnit->dwType == nType) && (nClassId == NULL || pUnit->dwTxtFileNo == nClassId))
			{
				myPresetUnit* mypUnit = new myPresetUnit;

				mypUnit->dwPosX = pUnit->dwPosX;
				mypUnit->dwPosY = pUnit->dwPosY;
				mypUnit->dwRoomX = pRoom->dwPosX;
				mypUnit->dwRoomY = pRoom->dwPosY;
				mypUnit->dwType = pUnit->dwType;
				mypUnit->dwId = pUnit->dwTxtFileNo;
				mypUnit->dwLevel = levelId;

				JSObject* unit = BuildObject(cx, &presetunit_class, NULL, presetunit_props, mypUnit);
				if(!unit)
				{
					delete mypUnit;
					JS_EndRequest(cx);
					THROW_ERROR(cx, "Failed to build object?");
				}

				jsval a = OBJECT_TO_JSVAL(unit);
				JS_SetElement(cx, pReturnArray, dwArrayCount, &a);

				dwArrayCount++;
			}
		}

		if(bAddedRoom)
		{
			D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
			bAddedRoom = FALSE;			
		}
	}
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(pReturnArray));
	
	JS_RemoveRoot(cx, &pReturnArray);
	JS_EndRequest(cx);
	cRoom.LeaveSection();
	return JS_TRUE;
}

JSAPI_FUNC(my_getPresetUnit)
{
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	if(argc < 1)
	{
		JS_SET_RVAL(cx, vp, JSVAL_FALSE);
		return JS_TRUE;
	}

	uint32 levelId;
	JS_BeginRequest(cx);
	JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[0], &levelId);
	JS_EndRequest(cx);
	Level* pLevel = GetLevel(levelId);

	if(!pLevel)
		THROW_ERROR(cx, "getPresetUnits failed, couldn't access the level!");

	DWORD nClassId = NULL;
	DWORD nType = NULL;

	if(argc >= 2)
		nType = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);
	if(argc >= 3)
		nClassId = JSVAL_TO_INT(JS_ARGV(cx, vp)[2]);

	CriticalRoom cRoom;
	cRoom.EnterSection();

	bool bAddedRoom = FALSE;

	for(Room2 *pRoom = pLevel->pRoom2First; pRoom; pRoom = pRoom->pRoom2Next) {

		bAddedRoom = FALSE;

		if(!pRoom->pRoom1)
		{
			D2COMMON_AddRoomData(D2CLIENT_GetPlayerUnit()->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
			bAddedRoom = TRUE;
		}

		for(PresetUnit* pUnit = pRoom->pPreset; pUnit; pUnit = pUnit->pPresetNext)
		{
			// Does it fit?
			if((nType == NULL || pUnit->dwType == nType) && (nClassId == NULL || pUnit->dwTxtFileNo == nClassId))
			{
				// Yes it fits! Return it
				myPresetUnit* mypUnit = new myPresetUnit;

				mypUnit->dwPosX = pUnit->dwPosX;
				mypUnit->dwPosY = pUnit->dwPosY;
				mypUnit->dwRoomX = pRoom->dwPosX;
				mypUnit->dwRoomY = pRoom->dwPosY;
				mypUnit->dwType = pUnit->dwType;
				mypUnit->dwId = pUnit->dwTxtFileNo;
				mypUnit->dwLevel = levelId;

				JSObject* obj = BuildObject(cx, &presetunit_class, NULL, presetunit_props, mypUnit);
				if(!obj)
				{
					delete mypUnit;
					THROW_ERROR(cx, "Failed to create presetunit object");
				}
				JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
				cRoom.LeaveSection();
				return JS_TRUE;
			}
		}

		if(bAddedRoom)
		{
			D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
			bAddedRoom = FALSE;			
		}
	}
	JS_SET_RVAL(cx, vp, JSVAL_FALSE);
	cRoom.LeaveSection();
	return JS_TRUE;
}
