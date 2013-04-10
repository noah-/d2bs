#ifndef AREA_H
#define AREA_H

#include <windows.h>
#include "js32.h"

CLASS_CTOR(area);

void area_finalize(JSFreeOp *fop, JSObject *obj);

JSAPI_PROP(area_getProperty);

JSAPI_FUNC(my_getArea);

enum area_tinyid {
	AUNIT_EXITS,
	AUNIT_NAME,
	AUNIT_X,
	AUNIT_XSIZE,
	AUNIT_Y,
	AUNIT_YSIZE,
	AUNIT_ID
};


static JSPropertySpec area_props[] = {
	{"exits",		AUNIT_EXITS,		JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(area_getProperty), JSOP_NULLWRAPPER},
	{"name",		AUNIT_NAME,			JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(area_getProperty), JSOP_NULLWRAPPER},
	{"x",			AUNIT_X,			JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(area_getProperty), JSOP_NULLWRAPPER},
	{"xsize",		AUNIT_XSIZE,		JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(area_getProperty), JSOP_NULLWRAPPER},
	{"y",			AUNIT_Y,			JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(area_getProperty), JSOP_NULLWRAPPER},
	{"ysize",		AUNIT_YSIZE,		JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(area_getProperty), JSOP_NULLWRAPPER},
	{"id",			AUNIT_ID,			JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(area_getProperty), JSOP_NULLWRAPPER},
	{ 0, 0, 0, JSOP_NULLWRAPPER, JSOP_NULLWRAPPER }
};

struct myArea {
	DWORD AreaId;
	DWORD Exits;
	JSObject* ExitArray;
};

#endif