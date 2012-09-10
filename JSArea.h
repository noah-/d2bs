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
	{"exits",		AUNIT_EXITS,		JSPROP_PERMANENT_VAR,	area_getProperty},
	{"name",		AUNIT_NAME,			JSPROP_PERMANENT_VAR,	area_getProperty},
	{"x",			AUNIT_X,			JSPROP_PERMANENT_VAR,	area_getProperty},
	{"xsize",		AUNIT_XSIZE,		JSPROP_PERMANENT_VAR,	area_getProperty},
	{"y",			AUNIT_Y,			JSPROP_PERMANENT_VAR,	area_getProperty},
	{"ysize",		AUNIT_YSIZE,		JSPROP_PERMANENT_VAR,	area_getProperty},
	{"id",			AUNIT_ID,			JSPROP_PERMANENT_VAR,	area_getProperty},
	{0},
};

struct myArea {
	DWORD AreaId;
	DWORD Exits;
	JSObject* ExitArray;
};

#endif