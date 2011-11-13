#include "JSArea.h"
#include "D2Ptrs.h"
#include "JSExits.h"
#include "CriticalSections.h"
#include "MapHeader.h"

EMPTY_CTOR(area)

void area_finalize(JSContext *cx, JSObject *obj)
{
	myArea* pArea = (myArea*)JS_GetPrivate(cx, obj);

	if(pArea)
	{
		if(pArea->ExitArray)
			JS_RemoveRoot(&pArea->ExitArray);
		JS_SetPrivate(cx, obj, NULL);
		delete pArea;
	}
}

JSAPI_PROP(area_getProperty)
{
	myArea* pArea = (myArea*)JS_GetPrivate(cx, obj);;
	if(!pArea)
		return JS_FALSE;

	Level* pLevel = GetLevel(pArea->AreaId);
	if(!pLevel)
		return JS_FALSE;

	switch(JSVAL_TO_INT(id))
	{
		case AUNIT_EXITS:
			{
				if(pArea->ExitArray == NULL)
				{
					pArea->ExitArray = JS_NewArrayObject(cx, 0, NULL);
					JS_AddRoot(&pArea->ExitArray);

					CriticalRoom cRoom;
					cRoom.EnterSection();

					LevelMap* map = LevelMap::GetMap(pLevel);
					ExitArray exits;
					map->GetExits(exits);

					int count = exits.size();
					for(int i = 0; i < count; i++)
					{
						myExit* exit = new myExit;
						exit->id = exits[i].Target;
						exit->x = exits[i].Position.first;
						exit->y = exits[i].Position.second;
						exit->type = exits[i].Type;
						exit->tileid = exits[i].TileId;
						exit->level = pArea->AreaId;

						JSObject* pExit = BuildObject(cx, &exit_class, NULL, exit_props, exit);
						if(!pExit)
						{
							delete exit;
							THROW_ERROR(cx, "Failed to create exit object!");
						}

						jsval a = OBJECT_TO_JSVAL(pExit);
						JS_SetElement(cx, pArea->ExitArray, i, &a);
					}
				}
				*vp = OBJECT_TO_JSVAL(pArea->ExitArray);
			}
			break;
		case AUNIT_NAME:
			{
				LevelTxt* pTxt = D2COMMON_GetLevelText(pArea->AreaId);
				if(pTxt)
					*vp = STRING_TO_JSVAL(JS_InternString(cx, pTxt->szName));
			}
			break;
		case AUNIT_X:
			*vp = INT_TO_JSVAL(pLevel->dwPosX);
			break;
		case AUNIT_Y:
			*vp = INT_TO_JSVAL(pLevel->dwPosY);
			break;
		case AUNIT_XSIZE:
			*vp = INT_TO_JSVAL(pLevel->dwSizeX);
			break;
		case AUNIT_YSIZE:
			*vp = INT_TO_JSVAL(pLevel->dwSizeY);
			break;
		case AUNIT_ID:
			*vp = INT_TO_JSVAL(pLevel->dwLevelNo);
			break;
	}

	return JS_TRUE;
}

JSAPI_FUNC(my_getArea)
{
	if(!WaitForGameReady())
		THROW_WARNING(cx, "Game not ready");

	int32 nArea = GetPlayerArea();

	if(argc == 1)
	{
		if(JSVAL_IS_INT(argv[0]))
			JS_ValueToECMAInt32(cx, argv[0], &nArea);
		else
			THROW_ERROR(cx, "Invalid parameter passed to getArea!");
	}

	if(nArea < 0)
		THROW_ERROR(cx, "Invalid parameter passed to getArea!");
	
	Level* pLevel = GetLevel(nArea);

	if(!pLevel)
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	myArea* pArea = new myArea;
	if(!pArea)
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	pArea->AreaId = nArea;
	pArea->ExitArray = NULL;
	
	JSObject* unit = BuildObject(cx, &area_class, NULL, area_props, pArea);
	if(!unit)
	{
		delete pArea;
		pArea = NULL;
		THROW_ERROR(cx, "Failed to build area unit!");
	}

	*rval = OBJECT_TO_JSVAL(unit);

	return JS_TRUE;
}
