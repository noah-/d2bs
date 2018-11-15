#ifndef JSSANDBOX_H
#define JSSANDBOX_H

#include "Script.h"

JSAPI_FUNC(sandbox_ctor);

JSAPI_PROP(sandbox_addProperty);
JSAPI_PROP(sandbox_delProperty);
JSAPI_PROP(sandbox_getProperty);
JSAPI_STRICT_PROP(sandbox_setProperty);

JSAPI_FUNC(sandbox_eval);
JSAPI_FUNC(sandbox_include);
JSAPI_FUNC(sandbox_isIncluded);
JSAPI_FUNC(sandbox_clear);

void sandbox_finalize(JSFreeOp *fop, JSObject *obj);

struct sandbox {
	JSContext* context;
	JSObject* innerObj;
	IncludeList list;
};

static JSFunctionSpec sandbox_methods[] = {
	JS_FS("evaluate",	sandbox_eval,		1, FUNCTION_FLAGS),
	JS_FS("include",		sandbox_include,	1, FUNCTION_FLAGS),
	JS_FS("isIncluded",	sandbox_isIncluded,	1, FUNCTION_FLAGS),
	JS_FS("clearScope",	sandbox_clear,		0, FUNCTION_FLAGS),
	JS_FS_END
};

#endif
