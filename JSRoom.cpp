#include "JSRoom.h"
#include "CriticalSections.h"
#include "JSPresetUnit.h"
#include "D2Structs.h"
#include "D2Ptrs.h"

#pragma warning (disable : 4002)

EMPTY_CTOR(room)

JSAPI_PROP(room_getProperty) {
    Room2 *pRoom2 = (Room2 *)JS_GetPrivate(cx, obj);

    if (!pRoom2)
        return JS_TRUE;
    jsval ID;
    JS_IdToValue(cx, id, &ID);
    switch (JSVAL_TO_INT(ID)) {
    case ROOM_NUM:
        if (pRoom2->dwPresetType != 2)
            vp.setInt32(-1);
        else if (pRoom2->pType2Info)
            vp.setInt32(pRoom2->pType2Info->dwRoomNumber);
        break;
    case ROOM_XPOS:
        vp.setInt32(pRoom2->dwPosX);
        break;
    case ROOM_YPOS:
        vp.setInt32(pRoom2->dwPosY);
        break;
    case ROOM_XSIZE:
        vp.setInt32(pRoom2->dwSizeX * 5);
        break;
    case ROOM_YSIZE:
        vp.setInt32(pRoom2->dwSizeY * 5);
        break;
    case ROOM_AREA:
    case ROOM_LEVEL:
        if (pRoom2->pLevel)
            vp.setInt32(pRoom2->pLevel->dwLevelNo);
        break;

    case ROOM_CORRECTTOMB:
        if (pRoom2->pLevel && pRoom2->pLevel->pMisc && pRoom2->pLevel->pMisc->dwStaffTombLevel)
            vp.setInt32(pRoom2->pLevel->pMisc->dwStaffTombLevel);
        break;

    default:
        break;
    }

    return JS_TRUE;
}

JSAPI_FUNC(room_getNext) {
    Room2 *pRoom2 = (Room2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
    if (!pRoom2) {
        JS_SET_RVAL(cx, vp, JSVAL_FALSE);
        return JS_TRUE;
    }

    pRoom2 = pRoom2->pRoom2Next;

    if (!pRoom2) {
        JSObject *obj = JS_THIS_OBJECT(cx, vp);
        //		JS_ClearScope(cx, obj);
        if (JS_ValueToObject(cx, JSVAL_NULL, &obj))
            JS_SET_RVAL(cx, vp, JSVAL_FALSE);
    } else {
        JS_SetPrivate(cx, JS_THIS_OBJECT(cx, vp), pRoom2);
        JS_SET_RVAL(cx, vp, JSVAL_TRUE);
    }

    return JS_TRUE;
}

JSAPI_FUNC(room_getPresetUnits) {
    Room2 *pRoom2 = (Room2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

    DWORD nType = NULL;
    DWORD nClass = NULL;

    if (argc > 0 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
        nType = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
    if (argc > 1 && JSVAL_IS_INT(JS_ARGV(cx, vp)[1]))
        nClass = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);

    bool bAdded = FALSE;
    DWORD dwArrayCount = NULL;

    AutoCriticalRoom *cRoom = new AutoCriticalRoom;
    if (!pRoom2 || !GameReady()) {
        delete cRoom;
        return JS_TRUE;
    }

    if (!pRoom2->pRoom1) {
        bAdded = TRUE;
        D2COMMON_AddRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY,
                             D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
    }

    JSObject *pReturnArray = JS_NewArrayObject(cx, 0, NULL);
    JS_AddRoot(cx, &pReturnArray);
    for (PresetUnit *pUnit = pRoom2->pPreset; pUnit; pUnit = pUnit->pPresetNext) {
        if ((pUnit->dwType == nType || nType == NULL) && (pUnit->dwTxtFileNo == nClass || nClass == NULL)) {
            myPresetUnit *mypUnit = new myPresetUnit;

            mypUnit->dwPosX = pUnit->dwPosX;
            mypUnit->dwPosY = pUnit->dwPosY;
            mypUnit->dwRoomX = pRoom2->dwPosX;
            mypUnit->dwRoomY = pRoom2->dwPosY;
            mypUnit->dwType = pUnit->dwType;
            mypUnit->dwId = pUnit->dwTxtFileNo;

            JSObject *jsUnit = BuildObject(cx, &presetunit_class, NULL, presetunit_props, mypUnit);
            if (!jsUnit) {
                delete cRoom;
                JS_SET_RVAL(cx, vp, JSVAL_FALSE);
                return JS_TRUE;
            }

            jsval a = OBJECT_TO_JSVAL(jsUnit);
            JS_BeginRequest(cx);
            JS_SetElement(cx, pReturnArray, dwArrayCount, &a);
            JS_EndRequest(cx);
            dwArrayCount++;
        }
    }

    if (bAdded)
        D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY,
                                D2CLIENT_GetPlayerUnit()->pPath->pRoom1);

    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(pReturnArray));
    JS_RemoveRoot(cx, &pReturnArray);
    delete cRoom;
    return JS_TRUE;
}
JSAPI_FUNC(room_getCollisionTypeArray) {
    Room2 *pRoom2 = (Room2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

    bool bAdded = FALSE;
    CollMap *pCol = NULL;

    AutoCriticalRoom *cRoom = new AutoCriticalRoom;
    if (!pRoom2 || !GameReady()) {
        delete cRoom;
        return JS_TRUE;
    }

    if (!pRoom2->pRoom1) {
        bAdded = TRUE;
        D2COMMON_AddRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY,
                             D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
    }

    JSObject *jsobjy = JS_NewUint16Array(cx, (pRoom2->dwSizeX * 5) * (pRoom2->dwSizeY * 5));
    JS_AddRoot(cx, &jsobjy);

    if (!jsobjy)
        return JS_TRUE;

    if (pRoom2->pRoom1)
        pCol = pRoom2->pRoom1->Coll;

    if (!pCol) {
        JS_RemoveRoot(cx, &jsobjy);
        if (bAdded)
            D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY,
                                    D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
        delete cRoom;
        return JS_TRUE;
    }

    int x = pCol->dwPosGameX - pRoom2->pLevel->dwPosX * 5;
    int y = pCol->dwPosGameY - pRoom2->pLevel->dwPosY * 5;
    int nCx = pCol->dwSizeGameX;
    int nCy = pCol->dwSizeGameY;

    int nLimitX = x + nCx;
    int nLimitY = y + nCy;

    int nCurrentArrayY = NULL;

    WORD *p = pCol->pMapStart;
    JS_BeginRequest(cx);
    for (int j = y; j < nLimitY; j++) {

        int nCurrentArrayX = 0;
        for (int i = x; i < nLimitX; i++) {
            jsval nNode = INT_TO_JSVAL(*p);

            if (!JS_SetElement(cx, jsobjy, nCurrentArrayY * nCx + nCurrentArrayX, &nNode)) {
                if (bAdded)
                    D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY,
                                            D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
                JS_RemoveRoot(cx, &jsobjy);
                JS_EndRequest(cx);
                delete cRoom;
                return JS_TRUE;
            }

            nCurrentArrayX++;
            p++;
        }
        nCurrentArrayY++;
    }

    if (bAdded)
        D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY,
                                D2CLIENT_GetPlayerUnit()->pPath->pRoom1);

    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsobjy));
    JS_RemoveRoot(cx, &jsobjy);
    JS_EndRequest(cx);
    delete cRoom;
    return JS_TRUE;
}
JSAPI_FUNC(room_getCollision) {
    Room2 *pRoom2 = (Room2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

    bool bAdded = FALSE;
    CollMap *pCol = NULL;

    AutoCriticalRoom *cRoom = new AutoCriticalRoom;
    if (!pRoom2 || !GameReady()) {
        delete cRoom;
        return JS_TRUE;
    }

    JSObject *jsobjy = JS_NewArrayObject(cx, NULL, NULL);
    JS_AddRoot(cx, &jsobjy);
    if (!jsobjy)
        return JS_TRUE;

    if (!pRoom2->pRoom1) {
        bAdded = TRUE;
        D2COMMON_AddRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY,
                             D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
    }

    if (pRoom2->pRoom1)
        pCol = pRoom2->pRoom1->Coll;

    if (!pCol) {
        JS_RemoveRoot(cx, &jsobjy);
        if (bAdded)
            D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY,
                                    D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
        delete cRoom;
        return JS_TRUE;
    }

    int x = pCol->dwPosGameX - pRoom2->pLevel->dwPosX * 5;
    int y = pCol->dwPosGameY - pRoom2->pLevel->dwPosY * 5;
    int nCx = pCol->dwSizeGameX;
    int nCy = pCol->dwSizeGameY;

    int nLimitX = x + nCx;
    int nLimitY = y + nCy;

    int nCurrentArrayY = NULL;

    WORD *p = pCol->pMapStart;
    JS_BeginRequest(cx);
    for (int j = y; j < nLimitY; j++) {
        JSObject *jsobjx = JS_NewArrayObject(cx, NULL, NULL);

        int nCurrentArrayX = 0;
        for (int i = x; i < nLimitX; i++) {
            jsval nNode = INT_TO_JSVAL(*p);

            if (!JS_SetElement(cx, jsobjx, nCurrentArrayX, &nNode)) {
                if (bAdded)
                    D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY,
                                            D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
                JS_RemoveRoot(cx, &jsobjy);
                JS_EndRequest(cx);
                delete cRoom;
                return JS_TRUE;
            }

            nCurrentArrayX++;
            p++;
        }

        jsval jsu = OBJECT_TO_JSVAL(jsobjx);

        if (!JS_SetElement(cx, jsobjy, nCurrentArrayY, &jsu)) {
            if (bAdded)
                D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY,
                                        D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
            JS_RemoveRoot(cx, &jsobjy);
            JS_EndRequest(cx);
            delete cRoom;
            return JS_TRUE;
        }
        nCurrentArrayY++;
    }

    if (bAdded)
        D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY,
                                D2CLIENT_GetPlayerUnit()->pPath->pRoom1);

    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsobjy));
    JS_RemoveRoot(cx, &jsobjy);
    JS_EndRequest(cx);
    delete cRoom;
    return JS_TRUE;
}

JSAPI_FUNC(room_getNearby) {
    Room2 *pRoom2 = (Room2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
    JSObject *jsobj = JS_NewArrayObject(cx, NULL, NULL);

    if (!jsobj)
        return JS_TRUE;
    JS_BeginRequest(cx);
    for (DWORD i = 0; i < pRoom2->dwRoomsNear; ++i) {
        JSObject *jsroom = BuildObject(cx, &room_class, room_methods, room_props, pRoom2->pRoom2Near[i]);
        if (!jsroom) {
            JS_EndRequest(cx);
            return JS_TRUE;
        }
        jsval jsu = OBJECT_TO_JSVAL(jsroom);

        if (!JS_SetElement(cx, jsobj, i, &jsu)) {
            JS_EndRequest(cx);
            return JS_TRUE;
        }
    }
    JS_EndRequest(cx);
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsobj));
    return JS_TRUE;
}

// Don't know whether it works or not
JSAPI_FUNC(room_getStat) {
    Room2 *pRoom2 = (Room2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    if (argc < 1 || !JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
        return JS_TRUE;

    jsint nStat = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);

    bool bAdded = false;

    AutoCriticalRoom *cRoom = new AutoCriticalRoom;
    if (!pRoom2 || !GameReady()) {
        delete cRoom;
        return JS_TRUE;
    }
    if (!pRoom2->pRoom1) {
        bAdded = true;
        D2COMMON_AddRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY,
                             D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
    }

    if (!pRoom2->pRoom1) {
        delete cRoom;
        return JS_TRUE;
    }

    if (nStat == 0) // xStart
        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(pRoom2->pRoom1->dwXStart));
    else if (nStat == 1)
        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(pRoom2->pRoom1->dwYStart));
    else if (nStat == 2)
        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(pRoom2->pRoom1->dwXSize));
    else if (nStat == 3)
        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(pRoom2->pRoom1->dwYSize));
    else if (nStat == 4)
        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(pRoom2->dwPosX));
    else if (nStat == 5)
        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(pRoom2->dwPosY));
    else if (nStat == 6)
        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(pRoom2->dwSizeX));
    else if (nStat == 7)
        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(pRoom2->dwSizeY));
    //	else if(nStat == 8)
    //		*rval = INT_TO_JSVAL(pRoom2->pRoom1->dwYStart); // God knows??!!??!?!?!?!
    else if (nStat == 9)
        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(pRoom2->pRoom1->Coll->dwPosGameX));
    else if (nStat == 10)
        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(pRoom2->pRoom1->Coll->dwPosGameY));
    else if (nStat == 11)
        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(pRoom2->pRoom1->Coll->dwSizeGameX));
    else if (nStat == 12)
        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(pRoom2->pRoom1->Coll->dwSizeGameY));
    else if (nStat == 13)
        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(pRoom2->pRoom1->Coll->dwPosRoomX));
    else if (nStat == 14)
        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(pRoom2->pRoom1->Coll->dwPosGameY));
    else if (nStat == 15)
        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(pRoom2->pRoom1->Coll->dwSizeRoomX));
    else if (nStat == 16)
        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(pRoom2->pRoom1->Coll->dwSizeRoomY));

    if (bAdded)
        D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY,
                                D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
    delete cRoom;
    return JS_TRUE;
}

JSAPI_FUNC(room_getFirst) {
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    Room2 *pRoom2 = (Room2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
    if (!pRoom2 || !pRoom2->pLevel || !pRoom2->pLevel->pRoom2First)
        return JS_TRUE;

    JSObject *jsroom = BuildObject(cx, &room_class, room_methods, room_props, pRoom2->pLevel->pRoom2First);
    if (!jsroom)
        return JS_TRUE;

    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsroom));

    return JS_TRUE;
}

JSAPI_FUNC(room_unitInRoom) {
    Room2 *pRoom2 = (Room2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
    if (!pRoom2 || argc < 1 || !JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[0]))
        return JS_TRUE;

    myUnit *pmyUnit = (myUnit *)JS_GetPrivate(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]));

    if (!pmyUnit || (pmyUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
        return JS_TRUE;

    UnitAny *pUnit = D2CLIENT_FindUnit(pmyUnit->dwUnitId, pmyUnit->dwType);

    if (!pUnit)
        return JS_TRUE;

    Room1 *pRoom1 = D2COMMON_GetRoomFromUnit(pUnit);

    if (!pRoom1 || !pRoom1->pRoom2)
        return JS_TRUE;

    JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(!!(pRoom1->pRoom2 == pRoom2)));

    return JS_TRUE;
}

JSAPI_FUNC(room_reveal) {
    Room2 *pRoom2 = (Room2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

    BOOL bDrawPresets = false;
    if (argc == 1 && JSVAL_IS_BOOLEAN(JS_ARGV(cx, vp)[0]))
        bDrawPresets = !!JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[0]);

    AutoCriticalRoom *cRoom = new AutoCriticalRoom;
    if (!pRoom2 || !GameReady()) {
        delete cRoom;
        return JS_TRUE;
    }

    JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(RevealRoom(pRoom2, bDrawPresets)));
    delete cRoom;
    return JS_TRUE;
}

JSAPI_FUNC(my_getRoom) {
    JS_SET_RVAL(cx, vp, JSVAL_VOID);

    if (!WaitForGameReady())
        THROW_ERROR(cx, "Get Room Game not ready");

    AutoCriticalRoom *cRoom = new AutoCriticalRoom;

    if (argc == 1 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0])) {
        uint32 levelId;
        JS_BeginRequest(cx);
        JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[0], &levelId);
        JS_EndRequest(cx);
        if (levelId != 0) // 1 Parameter, AreaId
        {
            Level *pLevel = GetLevel(levelId);

            if (!pLevel || !pLevel->pRoom2First) {
                delete cRoom;
                return JS_TRUE;
            }

            JSObject *jsroom = BuildObject(cx, &room_class, room_methods, room_props, pLevel->pRoom2First);
            if (!jsroom) {
                delete cRoom;
                return JS_TRUE;
            }
            delete cRoom;
            JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsroom));
            return JS_TRUE;
        } else if (levelId == 0) {
            Room1 *pRoom1 = D2COMMON_GetRoomFromUnit(D2CLIENT_GetPlayerUnit());

            if (!pRoom1 || !pRoom1->pRoom2) {
                delete cRoom;
                return JS_TRUE;
            }

            JSObject *jsroom = BuildObject(cx, &room_class, room_methods, room_props, pRoom1->pRoom2);
            delete cRoom;
            if (!jsroom)
                return JS_TRUE;

            JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsroom));
            return JS_TRUE;
        }
    } else if (argc == 3 || argc == 2) // area ,x and y
    {
        Level *pLevel = NULL;

        uint32 levelId;
        JS_BeginRequest(cx);
        JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[0], &levelId);
        JS_EndRequest(cx);
        if (argc == 3)
            pLevel = GetLevel(levelId);
        else if (D2CLIENT_GetPlayerUnit() && D2CLIENT_GetPlayerUnit()->pPath && D2CLIENT_GetPlayerUnit()->pPath->pRoom1 &&
                 D2CLIENT_GetPlayerUnit()->pPath->pRoom1->pRoom2)
            pLevel = D2CLIENT_GetPlayerUnit()->pPath->pRoom1->pRoom2->pLevel;

        if (!pLevel || !pLevel->pRoom2First) {
            delete cRoom;
            return JS_TRUE;
        }

        uint32 nX = NULL;
        uint32 nY = NULL;
        JS_BeginRequest(cx);
        if (argc == 2) {
            JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[0], &nX);
            JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[1], &nY);
        } else if (argc == 3) {
            JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[1], &nX);
            JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[2], &nY);
        }
        JS_EndRequest(cx);
        if (!nX || !nY) {
            delete cRoom;
            return JS_TRUE;
        }

        // Scan for the room with the matching x,y coordinates.
        for (Room2 *pRoom = pLevel->pRoom2First; pRoom; pRoom = pRoom->pRoom2Next) {
            bool bAdded = FALSE;
            if (!pRoom->pRoom1) {
                D2COMMON_AddRoomData(D2CLIENT_GetPlayerUnit()->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
                bAdded = TRUE;
            }

            CollMap *map = pRoom->pRoom1->Coll;
            if (nX >= map->dwPosGameX && nY >= map->dwPosGameY && nX < (map->dwPosGameX + map->dwSizeGameX) && nY < (map->dwPosGameY + map->dwSizeGameY)) {
                if (bAdded)
                    D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY,
                                            D2CLIENT_GetPlayerUnit()->pPath->pRoom1);

                JSObject *jsroom = BuildObject(cx, &room_class, room_methods, room_props, pRoom);
                delete cRoom;
                if (!jsroom)
                    return JS_TRUE;

                JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsroom));
                return JS_TRUE;
            }

            if (bAdded)
                D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY,
                                        D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
        }

        JSObject *jsroom = BuildObject(cx, &room_class, room_methods, room_props, pLevel->pRoom2First);
        delete cRoom;
        if (!jsroom)
            return JS_TRUE;

        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsroom));
        return JS_TRUE;
    } else {
        JSObject *jsroom = BuildObject(cx, &room_class, room_methods, room_props, D2CLIENT_GetPlayerUnit()->pPath->pRoom1->pRoom2->pLevel->pRoom2First);
        delete cRoom;
        if (!jsroom)
            return JS_TRUE;

        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsroom));
        return JS_TRUE;
    }
    delete cRoom;
    return JS_TRUE;
}
