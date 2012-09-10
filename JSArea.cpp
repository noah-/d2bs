#include "JSArea.h"
#include "D2Ptrs.h"
#include "JSExits.h"
#include "CriticalSections.h"
#include "MapHeader.h"

EMPTY_CTOR(area)

void area_finalize(JSFreeOp *fop, JSObject *obj)
{
	myArea* pArea = (myArea*)JS_GetPrivate(obj);

	if(pArea)
	{
		
		JS_SetPrivate(obj, NULL);
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

	jsval ID;
	JS_IdToValue(cx,id,&ID);
	switch(JSVAL_TO_INT(ID))
	{
		case AUNIT_EXITS:
			{
			JS_BeginRequest(cx);
				if(pArea->ExitArray == NULL)
				{
					pArea->ExitArray = JS_NewArrayObject(cx, 0, NULL);
					JS_AddRoot(cx, &pArea->ExitArray);

					CriticalRoom cRoom;
					cRoom.EnterSection();

					ActMap* map = ActMap::GetMap(pLevel);
					
					ExitArray exits;
					map->GetExits(exits);
					map->CleanUp();
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
							JS_EndRequest(cx);
							THROW_ERROR(cx, "Failed to create exit object!");
						}

						jsval a = OBJECT_TO_JSVAL(pExit);
						JS_SetElement(cx, pArea->ExitArray, i, &a);
					}
				}
				*vp = OBJECT_TO_JSVAL(pArea->ExitArray);
				if(pArea->ExitArray)
					JS_RemoveRoot(cx, &pArea->ExitArray);
			}
			JS_EndRequest(cx);
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
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	if(!WaitForGameReady())
		THROW_ERROR(cx, "Get Area: Game not ready");

	int32 nArea = GetPlayerArea();

	if(argc == 1)
	{
	
		if(JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		{
			JS_BeginRequest(cx);
			JS_ValueToECMAInt32(cx, JS_ARGV(cx, vp)[0], &nArea);
			JS_EndRequest(cx);
		}
		else
			THROW_ERROR(cx, "Invalid parameter passed to getArea!");
	}

	if(nArea < 0)
		THROW_ERROR(cx, "Invalid parameter passed to getArea!");
	
	Level* pLevel = GetLevel(nArea);

	if(!pLevel)
	{
		JS_SET_RVAL(cx, vp, JSVAL_FALSE);
		return JS_TRUE;
	}

	myArea* pArea = new myArea;
	if(!pArea)
	{
		JS_SET_RVAL(cx, vp, JSVAL_FALSE);
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
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(unit));
	
	return JS_TRUE;
}
