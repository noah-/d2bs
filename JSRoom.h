#ifndef ROOM_H
#define ROOM_H

#include "Room.h"

#include "js32.h"

CLASS_CTOR(room);

JSAPI_PROP(room_getProperty);

JSAPI_FUNC(room_getNext);
JSAPI_FUNC(room_getPresetUnits);
JSAPI_FUNC(room_getCollision);
JSAPI_FUNC(room_getNearby);
JSAPI_FUNC(room_getStat);
JSAPI_FUNC(room_getFirst);
JSAPI_FUNC(room_unitInRoom);
JSAPI_FUNC(room_reveal);

JSAPI_FUNC(my_getRoom);

enum room_tinyid {
	ROOM_NUM, 
	ROOM_XPOS, 
	ROOM_YPOS, 
	ROOM_XSIZE, 
	ROOM_YSIZE, 
	ROOM_SUBNUMBER, 
	ROOM_AREA, 
	ROOM_LEVEL, 
	ROOM_CORRECTTOMB,
};

static JSPropertySpec room_props[] = { 
	{"number",		ROOM_NUM,			JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(room_getProperty), JSOP_NULLWRAPPER },
	{"x",			ROOM_XPOS,			JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(room_getProperty), JSOP_NULLWRAPPER },
	{"y",			ROOM_YPOS,			JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(room_getProperty), JSOP_NULLWRAPPER },
	{"xsize",		ROOM_XSIZE,			JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(room_getProperty), JSOP_NULLWRAPPER },
	{"ysize",		ROOM_YSIZE,			JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(room_getProperty), JSOP_NULLWRAPPER },
	{"subnumber",	ROOM_SUBNUMBER,		JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(room_getProperty), JSOP_NULLWRAPPER },
	{"area",		ROOM_AREA,			JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(room_getProperty), JSOP_NULLWRAPPER },
	{"level",		ROOM_LEVEL,			JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(room_getProperty), JSOP_NULLWRAPPER },
	{"correcttomb",	ROOM_CORRECTTOMB,	JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(room_getProperty), JSOP_NULLWRAPPER },
	{ 0, 0, 0, JSOP_NULLWRAPPER, JSOP_NULLWRAPPER }
};

static JSFunctionSpec room_methods[] = {
	JS_FS("getNext",		room_getNext,			0, FUNCTION_FLAGS),
	JS_FS("reveal",			room_reveal,			1, FUNCTION_FLAGS),
	JS_FS("getPresetUnits",	room_getPresetUnits,	0, FUNCTION_FLAGS),
	JS_FS("getCollision",	room_getCollision,		0, FUNCTION_FLAGS),
	JS_FS("getNearby",		room_getNearby,			0, FUNCTION_FLAGS),
	JS_FS("getStat",		room_getStat,			0, FUNCTION_FLAGS),
	JS_FS("getFirst",		room_getFirst,			0, FUNCTION_FLAGS),
	JS_FS("unitInRoom",		room_unitInRoom,		1, FUNCTION_FLAGS),
	JS_FS_END
};

#endif
