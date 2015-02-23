#ifndef __JSSOCKET_H__
#define __JSSOCKET_H__

#include "js32.h"

//////////////////////////////////////////////////////////////////
// socket stuff
//////////////////////////////////////////////////////////////////

CLASS_CTOR(socket);
JSBool socket_equality(JSContext *cx, JSObject *obj, jsval v, JSBool *bp);

JSAPI_PROP(socket_getProperty);
JSAPI_STRICT_PROP(socket_setProperty);

JSAPI_FUNC(socket_open);
JSAPI_FUNC(socket_close);
JSAPI_FUNC(socket_send);
JSAPI_FUNC(socket_read);

void socket_finalize(JSFreeOp *fop, JSObject *obj);

enum {
	SOCKET_READABLE,
};

static JSFunctionSpec socket_methods[] = {
	JS_FN("close",		socket_close,			0, FUNCTION_FLAGS),
	JS_FN("send",		socket_send,			1, FUNCTION_FLAGS),
	JS_FN("read",	socket_read,		0, FUNCTION_FLAGS),
	JS_FS_END
};

static JSFunctionSpec socket_s_methods[] = {
	JS_FN("open",		socket_open,		2, FUNCTION_FLAGS),
	JS_FS_END
};



static JSPropertySpec socket_props[] = {
	{"readable",	SOCKET_READABLE,	JSPROP_PERMANENT_VAR,	JSOP_WRAPPER(socket_getProperty), JSOP_NULLWRAPPER },
	{ 0, 0, 0, JSOP_NULLWRAPPER, JSOP_NULLWRAPPER }
};

#endif
