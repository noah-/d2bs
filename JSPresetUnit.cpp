#include "JSPresetUnit.h"

#include "D2Ptrs.h"
#include "CriticalSections.h"
#include "D2Helpers.h"

EMPTY_CTOR(presetunit)

void presetunit_finalize(JSContext *cx, JSObject *obj)
{
	myPresetUnit *pUnit = (myPresetUnit*)JS_GetPrivate(cx, obj);

	if(pUnit)
	{
		JS_SetPrivate(cx, obj, NULL);
		delete pUnit;
	}
}

JSAPI_PROP(presetunit_getProperty)
{
	myPresetUnit* pUnit = (myPresetUnit*)JS_GetPrivate(cx, obj);

	if(!pUnit)
		return JS_TRUE;

	switch(JSVAL_TO_INT(id))
	{
		case PUNIT_TYPE:
			*vp = INT_TO_JSVAL(pUnit->dwType);
			break;
		case PUNIT_ROOMX:
			*vp = INT_TO_JSVAL(pUnit->dwRoomX);
			break;
		case PUNIT_ROOMY:
			*vp = INT_TO_JSVAL(pUnit->dwRoomY);
			break;
		case PUNIT_X:
			*vp = INT_TO_JSVAL(pUnit->dwPosX);
			break;
		case PUNIT_Y:
			*vp = INT_TO_JSVAL(pUnit->dwPosY);
			break;
		case PUNIT_ID:
			*vp = INT_TO_JSVAL(pUnit->dwId);
			break;
		case PUINT_LEVEL:
			*vp = INT_TO_JSVAL(pUnit->dwLevel);
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
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	uint32 levelId;
	JS_ValueToECMAUint32(cx, argv[0], &levelId);
	Level* pLevel = GetLevel(levelId);

	if(!pLevel)
		THROW_ERROR(cx, "getPresetUnits failed, couldn't access the level!");

	uint nClassId = NULL;
	uint nType = NULL;

	if(argc >= 2)
		nType = JSVAL_TO_INT(argv[1]);
	if(argc >= 3)
		nClassId = JSVAL_TO_INT(argv[2]);

	CriticalRoom cRoom;
	cRoom.EnterSection();

	bool bAddedRoom = FALSE;
	DWORD dwArrayCount = NULL;

	JSObject* pReturnArray = JS_NewArrayObject(cx, 0, NULL);
	JS_AddRoot(&pReturnArray);
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

	*rval = OBJECT_TO_JSVAL(pReturnArray);
	JS_RemoveRoot(&pReturnArray);

	return JS_TRUE;
}

JSAPI_FUNC(my_getPresetUnit)
{
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	if(argc < 1)
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	uint32 levelId;
	JS_ValueToECMAUint32(cx, argv[0], &levelId);
	Level* pLevel = GetLevel(levelId);

	if(!pLevel)
		THROW_ERROR(cx, "getPresetUnits failed, couldn't access the level!");

	DWORD nClassId = NULL;
	DWORD nType = NULL;

	if(argc >= 2)
		nType = JSVAL_TO_INT(argv[1]);
	if(argc >= 3)
		nClassId = JSVAL_TO_INT(argv[2]);

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

				*rval = OBJECT_TO_JSVAL(obj);
				return JS_TRUE;
			}
		}

		if(bAddedRoom)
		{
			D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
			bAddedRoom = FALSE;			
		}
	}

	*rval = JSVAL_FALSE;

	return JS_TRUE;
}
