#ifndef EXITS_H
#define EXITS_H

#include "js32.h"
#include <windows.h>

CLASS_CTOR(exit);

void exit_finalize(JSContext *cx, JSObject *obj);
JSAPI_PROP(exit_getProperty);

enum exit_tinyid {
	EXIT_X,
	EXIT_Y,
	EXIT_TARGET,
	EXIT_TYPE,
	EXIT_TILEID,
	EXIT_LEVELID
};


static JSPropertySpec exit_props[] = {
	{"x",			EXIT_X,			JSPROP_PERMANENT_VAR,	exit_getProperty},
	{"y",			EXIT_Y,			JSPROP_PERMANENT_VAR,	exit_getProperty},
	{"target",		EXIT_TARGET,	JSPROP_PERMANENT_VAR,	exit_getProperty},
	{"type",		EXIT_TYPE,		JSPROP_PERMANENT_VAR,	exit_getProperty},
	{"tileid",		EXIT_TILEID,	JSPROP_PERMANENT_VAR,	exit_getProperty},
	{"level",		EXIT_LEVELID,	JSPROP_PERMANENT_VAR,	exit_getProperty},
	{0},
};

struct myExit {
	DWORD x;
	DWORD y;
	DWORD id;
	DWORD type;
	DWORD tileid;
	DWORD level;
};

#endif