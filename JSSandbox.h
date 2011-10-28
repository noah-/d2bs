#ifndef JSSANDBOX_H
#define JSSANDBOX_H

#include "Script.h"

JSAPI_FUNC(sandbox_ctor);

JSAPI_PROP(sandbox_addProperty);
JSAPI_PROP(sandbox_delProperty);
JSAPI_PROP(sandbox_getProperty);
JSAPI_PROP(sandbox_setProperty);

JSAPI_FUNC(sandbox_eval);
JSAPI_FUNC(sandbox_include);
JSAPI_FUNC(sandbox_isIncluded);
JSAPI_FUNC(sandbox_clear);

void sandbox_finalize(JSContext *cx, JSObject *obj);

struct sandbox {
	JSContext* context;
	JSObject* innerObj;
	IncludeList list;
};

static JSFunctionSpec sandbox_methods[] = {
	{"evaluate",	sandbox_eval,		1},
	{"include",		sandbox_include,	1},
	{"isIncluded",	sandbox_isIncluded,	1},
	{"clearScope",	sandbox_clear,		0},
	{0}
};

#endif
