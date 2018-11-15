#pragma once

#include "Control.h"

#include "js32.h"
#include <windows.h>
#include "D2Ptrs.h"

CLASS_CTOR(control);

void control_finalize(JSFreeOp *fop, JSObject *obj);

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
	CONTROL_TEXT		,
	CONTROL_X			,
	CONTROL_Y			,
	CONTROL_XSIZE		,
	CONTROL_YSIZE		,
	CONTROL_STATE		,
	CONTROL_MAXLENGTH	,
	CONTROL_TYPE		,
	CONTROL_VISIBLE		,
	CONTROL_CURSORPOS	,
	CONTROL_SELECTSTART	,
	CONTROL_SELECTEND	,
	CONTROL_PASSWORD	,
	CONTROL_DISABLED	
};


static JSPropertySpec control_props[] = {
	{"text",		CONTROL_TEXT,			JSPROP_STATIC_VAR,		JSOP_WRAPPER(control_getProperty), JSOP_WRAPPER(control_setProperty)},
	{"x",			CONTROL_X,				JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(control_getProperty), JSOP_NULLWRAPPER},
	{"y",			CONTROL_Y,				JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(control_getProperty), JSOP_NULLWRAPPER},
	{"xsize",		CONTROL_XSIZE,			JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(control_getProperty), JSOP_NULLWRAPPER},
	{"ysize",		CONTROL_YSIZE,			JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(control_getProperty), JSOP_NULLWRAPPER},
	{"state",		CONTROL_STATE,			JSPROP_STATIC_VAR,		JSOP_WRAPPER(control_getProperty),	JSOP_WRAPPER(control_setProperty)},
	{"password",	CONTROL_PASSWORD,		JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(control_getProperty), JSOP_NULLWRAPPER},
//	{"maxlength",	CONTROL_MAXLENGTH,		JSPROP_PERMANENT_VAR,	control_getProperty},
	{"type",		CONTROL_TYPE,			JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(control_getProperty), JSOP_NULLWRAPPER},
//	{"visible",		CONTROL_VISIBLE,		JSPROP_PERMANENT_VAR,	control_getProperty},
	{"cursorpos",	CONTROL_CURSORPOS,		JSPROP_STATIC_VAR,		JSOP_WRAPPER(control_getProperty),	JSOP_WRAPPER(control_setProperty)},
	{"selectstart",	CONTROL_SELECTSTART,	JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(control_getProperty), JSOP_NULLWRAPPER},
	{"selectend",	CONTROL_SELECTEND,		JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(control_getProperty), JSOP_NULLWRAPPER},
	{"disabled",	CONTROL_DISABLED,		JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(control_getProperty),	JSOP_WRAPPER(control_setProperty)},
	{ 0, 0, 0, JSOP_NULLWRAPPER, JSOP_NULLWRAPPER }
};

static JSFunctionSpec control_funcs[] = {
	JS_FS("getNext",		control_getNext,		0, FUNCTION_FLAGS),
	JS_FS("click",			control_click,			0, FUNCTION_FLAGS),
	JS_FS("setText",		control_setText,		1, FUNCTION_FLAGS),
	JS_FS("getText",		control_getText,		0, FUNCTION_FLAGS),
	JS_FS_END
};