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
	{"name",			SCRIPT_FILENAME,		JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(script_getProperty), JSOP_NULLWRAPPER },
	{"type",			SCRIPT_GAMETYPE,		JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(script_getProperty), JSOP_NULLWRAPPER },
	{"running",			SCRIPT_RUNNING,			JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(script_getProperty), JSOP_NULLWRAPPER },
	{"threadid",		SCRIPT_THREADID,		JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(script_getProperty), JSOP_NULLWRAPPER },
	{"memory",  		SCRIPT_THREADID,		JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(script_getProperty), JSOP_NULLWRAPPER },
	{ 0, 0, 0, JSOP_NULLWRAPPER, JSOP_NULLWRAPPER }
};

static JSFunctionSpec script_methods[] = {
	JS_FS("getNext",		script_getNext,		0, FUNCTION_FLAGS),
	JS_FS("pause",			script_pause,		0, FUNCTION_FLAGS),
	JS_FS("resume",			script_resume,		0, FUNCTION_FLAGS),
	JS_FS("stop",			script_stop,		0, FUNCTION_FLAGS),
	JS_FS("join",			script_join,		0, FUNCTION_FLAGS),
	JS_FS("send",			script_send,		1, FUNCTION_FLAGS),
	JS_FS_END
};
