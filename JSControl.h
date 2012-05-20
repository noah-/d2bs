#pragma once

#include "Control.h"

#include "js32.h"
#include <windows.h>
#include "D2Ptrs.h"

CLASS_CTOR(control);

void control_finalize(JSContext *cx, JSObject *obj);

JSAPI_FUNC(control_getNext);
JSAPI_FUNC(control_click);
JSAPI_FUNC(control_setText);
JSAPI_FUNC(control_getText);

JSAPI_PROP(control_getProperty);
JSAPI_STRICT_PROP(control_setProperty);

struct ControlData {
	DWORD _dwPrivate;
	Control* pControl;

	DWORD	dwType;
	DWORD	dwX;
	DWORD	dwY;
	DWORD	dwSizeX;
	DWORD	dwSizeY;
};

enum control_tinyid {
	CONTROL_TEXT		= -1,
	CONTROL_X			= -2,
	CONTROL_Y			= -3,
	CONTROL_XSIZE		= -4,
	CONTROL_YSIZE		= -5,
	CONTROL_STATE		= -6,
	CONTROL_MAXLENGTH	= -7,
	CONTROL_TYPE		= -8,
	CONTROL_VISIBLE		= -9,
	CONTROL_CURSORPOS	= -10,
	CONTROL_SELECTSTART	= -11,
	CONTROL_SELECTEND	= -12,
	CONTROL_PASSWORD	= -13,
	CONTROL_DISABLED	= -14
};


static JSPropertySpec control_props[] = {
	{"text",		CONTROL_TEXT,			JSPROP_STATIC_VAR,		control_getProperty,	control_setProperty},
	{"x",			CONTROL_X,				JSPROP_PERMANENT_VAR,	control_getProperty},
	{"y",			CONTROL_Y,				JSPROP_PERMANENT_VAR,	control_getProperty},
	{"xsize",		CONTROL_XSIZE,			JSPROP_PERMANENT_VAR,	control_getProperty},
	{"ysize",		CONTROL_YSIZE,			JSPROP_PERMANENT_VAR,	control_getProperty},
	{"state",		CONTROL_STATE,			JSPROP_STATIC_VAR,		control_getProperty,	control_setProperty},
	{"password",	CONTROL_PASSWORD,		JSPROP_PERMANENT_VAR,	control_getProperty},
//	{"maxlength",	CONTROL_MAXLENGTH,		JSPROP_PERMANENT_VAR,	control_getProperty},
	{"type",		CONTROL_TYPE,			JSPROP_PERMANENT_VAR,	control_getProperty},
//	{"visible",		CONTROL_VISIBLE,		JSPROP_PERMANENT_VAR,	control_getProperty},
	{"cursorpos",	CONTROL_CURSORPOS,		JSPROP_STATIC_VAR,		control_getProperty,	control_setProperty},
	{"selectstart",	CONTROL_SELECTSTART,	JSPROP_PERMANENT_VAR,	control_getProperty},
	{"selectend",	CONTROL_SELECTEND,		JSPROP_PERMANENT_VAR,	control_getProperty},
	{"disabled",	CONTROL_DISABLED,		JSPROP_PERMANENT_VAR,	control_getProperty,	control_setProperty},
	{0},
};

static JSFunctionSpec control_funcs[] = {
	{"getNext",			control_getNext,		0},
	{"click",			control_click,			0},
	{"setText",			control_setText,		1},
	{"getText",			control_getText,		0},
	{0},
};