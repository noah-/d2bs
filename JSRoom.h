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
	{"number",		ROOM_NUM,			JSPROP_PERMANENT_VAR,	room_getProperty},
	{"x",			ROOM_XPOS,			JSPROP_PERMANENT_VAR,	room_getProperty},
	{"y",			ROOM_YPOS,			JSPROP_PERMANENT_VAR,	room_getProperty},
	{"xsize",		ROOM_XSIZE,			JSPROP_PERMANENT_VAR,	room_getProperty},
	{"ysize",		ROOM_YSIZE,			JSPROP_PERMANENT_VAR,	room_getProperty},
	{"subnumber",	ROOM_SUBNUMBER,		JSPROP_PERMANENT_VAR,	room_getProperty},
	{"area",		ROOM_AREA,			JSPROP_PERMANENT_VAR,	room_getProperty},
	{"level",		ROOM_LEVEL,			JSPROP_PERMANENT_VAR,	room_getProperty},
	{"correcttomb",	ROOM_CORRECTTOMB,	JSPROP_PERMANENT_VAR,	room_getProperty},
	{0}
};

static JSFunctionSpec room_methods[] = {
	{"getNext",			room_getNext,			0},
	{"reveal",			room_reveal,			1},
	{"getPresetUnits",	room_getPresetUnits,	0},
	{"getCollision",	room_getCollision,		0},
	{"getNearby",		room_getNearby,			0},
	{"getStat",			room_getStat,			0},
	{"getFirst",		room_getFirst,			0},
	{"unitInRoom",		room_unitInRoom,		1},
	{0}
};

#endif
