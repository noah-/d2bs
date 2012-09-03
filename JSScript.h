#pragma once

#include <windows.h>

#include "Script.h"

#include "js32.h"

//CLASS_CTOR(script);
CLASS_CTOR(script);
JSAPI_PROP(script_getProperty);

JSAPI_FUNC(script_getNext);
JSAPI_FUNC(script_stop);
JSAPI_FUNC(script_send);
JSAPI_FUNC(script_pause);
JSAPI_FUNC(script_resume);
JSAPI_FUNC(script_join);
JSAPI_FUNC(my_getScript);

enum script_tinyid {
	SCRIPT_FILENAME,
	SCRIPT_GAMETYPE,
	SCRIPT_RUNNING,
	SCRIPT_THREADID,
	SCRIPT_MEMORY,
};

static JSPropertySpec script_props[] = {
	{"name",			SCRIPT_FILENAME,		JSPROP_PERMANENT_VAR,	script_getProperty},
	{"type",			SCRIPT_GAMETYPE,		JSPROP_PERMANENT_VAR,	script_getProperty},
	{"running",			SCRIPT_RUNNING,			JSPROP_PERMANENT_VAR,	script_getProperty},
	{"threadid",		SCRIPT_THREADID,		JSPROP_PERMANENT_VAR,	script_getProperty},
	{"memory",  		SCRIPT_THREADID,		JSPROP_PERMANENT_VAR,	script_getProperty},
	{0},
};

static JSFunctionSpec script_methods[] = {
	{"getNext",			script_getNext,		0},
	{"pause",			script_pause,		0},
	{"resume",			script_resume,		0},
	{"stop",			script_stop,		0},
	{"join",			script_join,		0},
	{"send",			script_send,		1},
	{0},
};
