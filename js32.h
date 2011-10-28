#pragma once

#define JS_THREADSAFE
#define XP_WIN

#include "js32/jsapi.h"
#include "js32/jsdbgapi.h"
// this should be included, but can't be due to compiler include cycles
// however, every file that includes this one includes ScriptEngine.h first anyway
//#include "ScriptEngine.h"

#define JSAPI_FUNC(fName) JSBool fName (JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
#define JSAPI_PROP(fName) JSBool fName (JSContext *cx, JSObject *obj, jsval id, jsval *vp)

#define JSVAL_IS_FUNCTION(cx, var) (JSVAL_IS_OBJECT(var) && JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(var)))

#define JSPROP_PERMANENT_VAR (JSPROP_READONLY | JSPROP_ENUMERATE | JSPROP_PERMANENT)
#define JSPROP_STATIC_VAR (JSPROP_ENUMERATE | JSPROP_PERMANENT)

#define NUM(x) #x
#define NAME(line, v) (__FILE__ "@" NUM(line) ": " #v)
#define JS_AddRoot(vp) JS_AddNamedRootRT(ScriptEngine::GetRuntime(), (vp), NAME(__LINE__, vp))
#define JS_RemoveRoot(vp) JS_RemoveRootRT(ScriptEngine::GetRuntime(), (vp));

#define DEPRECATED JS_ReportWarning(cx, "This function has been deprecated, and will be removed from future releases.")

JSObject* BuildObject(JSContext* cx, JSClass* classp = NULL, JSFunctionSpec* funcs = NULL, JSPropertySpec* props = NULL, void* priv = NULL, JSObject* proto = NULL, JSObject* parent = NULL);
#define THROW_ERROR(cx, msg) { JS_ReportError(cx, msg); return JS_FALSE; }
#define THROW_WARNING(cx, msg) { JS_ReportWarning(cx, msg); }

#define CLASS_CTOR(name) JSBool name##_ctor (JSContext *cx, JSObject* obj, uintN argc, jsval *argv, jsval *rval)
#define EMPTY_CTOR(name) \
JSBool name##_ctor (JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) { \
	THROW_ERROR(cx, "Invalid Operation"); }

