#ifndef PRESETUNIT_H
#define PRESETUNIT_H

#include <windows.h>
#include "js32.h"

CLASS_CTOR(presetunit);

void presetunit_finalize(JSFreeOp *fop, JSObject *obj);
JSAPI_PROP(presetunit_getProperty);

JSAPI_FUNC(my_getPresetUnit);
JSAPI_FUNC(my_getPresetUnits);

enum presetunit_tinyid {
	PUNIT_TYPE,			// 0
	PUNIT_ROOMX,		// 1
	PUNIT_ROOMY,		// 2
	PUNIT_X,			// 3
	PUNIT_Y,			// 4
	PUNIT_ID,			// 5
	PUINT_LEVEL			// 6
};

static JSPropertySpec presetunit_props[] = {
	{"type",		PUNIT_TYPE,			JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(presetunit_getProperty), JSOP_NULLWRAPPER },
	{"roomx",		PUNIT_ROOMX,		JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(presetunit_getProperty), JSOP_NULLWRAPPER },
	{"roomy",		PUNIT_ROOMY,		JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(presetunit_getProperty), JSOP_NULLWRAPPER },
	{"x",			PUNIT_X,			JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(presetunit_getProperty), JSOP_NULLWRAPPER },
	{"y",			PUNIT_Y,			JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(presetunit_getProperty), JSOP_NULLWRAPPER },
	{"id",			PUNIT_ID,			JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(presetunit_getProperty), JSOP_NULLWRAPPER },
	{"level",		PUINT_LEVEL,		JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(presetunit_getProperty), JSOP_NULLWRAPPER },
	{ 0, 0, 0, JSOP_NULLWRAPPER, JSOP_NULLWRAPPER }
};

struct myPresetUnit
{
	DWORD dwType;
	DWORD dwRoomX;
	DWORD dwRoomY;
	DWORD dwPosX;
	DWORD dwPosY;
	DWORD dwId;
	DWORD dwLevel;
};

#endif