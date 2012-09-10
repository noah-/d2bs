#pragma once

#define XP_WIN

#include "dependencies\include\jsapi.h"
//#include "Exports.hpp"
#include <vector>
#include <io.h>
#include <string>
#include <fstream>
#include <streambuf>

typedef unsigned int uint;
typedef uint32_t uint32;
typedef int32_t int32;
typedef unsigned int uintN;
typedef unsigned int jsrefcount;
typedef double jsdouble;
typedef int32_t jsint;
typedef uint32_t jsuint;
typedef uint16_t uint16;

#if defined(EXPORTING)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif
// this should be included, but can't be due to compiler include cycles
// however, every file that includes this one includes ScriptEngine.h first anyway
//#include "ScriptEngine.h"
#define NUM(x) #x
#define NAME(line, v) (__FILE__ ":" NUM(line) " -> " #v)
#define JS_AddValueRoot(cx,vp) JS_AddNamedValueRoot((cx), (vp), NAME(__LINE__, vp))
#define JS_AddStringRoot(cx,vp) JS_AddNamedStringRoot((cx), (vp), NAME(__LINE__, vp))
#define JS_AddObjectRoot(cx,vp) JS_AddNamedObjectRoot((cx), (vp), NAME(__LINE__, vp))
#define JS_AddGCThingRoot(cx,vp) JS_AddNamedGCThingRoot((cx), (vp), NAME(__LINE__, vp))

void * JS_GetPrivate(JSContext *cx, JSObject *obj);
void JS_SetPrivate(JSContext *cx, JSObject *obj, void *data);
JSBool JSVAL_IS_OBJECT(jsval v);
// IMPORTANT: Ordering is critical here! If your object has a
// defined prototype, _THAT PROTOTYPE MUST BE LISTED ABOVE IT_
struct JSClassSpec {
	JSClass* classp;
	JSClass* proto;
	JSNative ctor;
	uintN argc;
	JSFunctionSpec* methods;
	JSPropertySpec* properties;
	JSFunctionSpec* static_methods;
	JSPropertySpec* static_properties;
};

struct JSModuleSpec {
	const wchar_t* name;
	JSClassSpec* classes;
	JSFunctionSpec* static_methods;
	JSPropertySpec* static_properties;
};

class JSAutoRoot {
private:
	JSAutoRoot(const JSAutoRoot&);
	JSAutoRoot& operator=(const JSAutoRoot&);

	JSContext* cx;
	jsval* ref;
public:
	JSAutoRoot(JSContext* cx, jsval value) : cx(cx) { ref = new jsval(value); JS_AddValueRoot(cx, ref); }
	~JSAutoRoot() { JS_RemoveValueRoot(cx, ref); delete ref; }
	jsval* get() { return ref; }                   
};

#define JS_AddRoot(cx, vp) JS_AddObjectRoot(cx, (JSObject**)(vp), NAME(__LINE__, vp))
#define JS_RemoveRoot(cx, vp) JS_RemoveObjectRoot(cx, (JSObject**) (vp));
#define JSVAL_IS_FUNCTION(cx, var) (!JSVAL_IS_PRIMITIVE(var) && JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(var)))

#define JSPROP_PERMANENT_VAR (JSPROP_READONLY | JSPROP_ENUMERATE | JSPROP_PERMANENT)
#define JSPROP_STATIC_VAR (JSPROP_ENUMERATE | JSPROP_PERMANENT)

#define CLASS_CTOR(name) JSBool name##_ctor (JSContext* cx, uintN argc, jsval* vp) 

//#define EMPTY_CTOR(name) JSBool name##_ctor (JSContext* cx, uintN argc, jsval* vp) { return THROW_ERROR(cx, #name " is not constructable."); }
//#define EMPTY_CTOR(name) \
//JSBool name##_ctor (JSContext *cx, JSObject* obj, uintN argc, jsval *argv, jsval *rval) { \
//	THROW_ERROR(cx, "Invalid Operation"); }

JSObject* BuildObject(JSContext* cx, JSClass* classp = NULL, JSFunctionSpec* funcs = NULL, JSPropertySpec* props = NULL, void* priv = NULL, JSObject* proto = NULL, JSObject* parent = NULL);
JSScript* JS_CompileFile(JSContext* cx, JSObject* globalObject, std::string fileName);
#define THROW_ERROR(cx, msg) { JS_ReportError(cx, msg); return JS_FALSE; }
#define THROW_WARNING(cx, msg) { JS_ReportWarning(cx, msg); }

//EXPORT JSBool JS_ThrowError(JSContext* cx, const char* message, ...);
EXPORT void JS_DefineClasses(JSContext* cx, JSObject* obj, JSClassSpec* classes);
EXPORT JSObject* JS_GetProtoForClass(JSContext* cx, JSClass* classp);
EXPORT JSObject* JS_NewObjectWithProto(JSContext* cx, JSObject* obj, JSClassSpec* classp, JSClassSpec* proto, JSObject* parent);

#define JSPROP_DEFAULT	JSPROP_ENUMERATE|JSPROP_PERMANENT
#define JSPROP_STATIC	JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY

#define JSAPI_FUNC(name) JSBool name##(JSContext *cx, uintN argc, jsval *vp)
//#define JSAPI_EMPTY_CTOR(name) JSBool name##(JSContext* cx, uintN argc, jsval* vp) { return THROW_ERROR(cx, "Invalid Operation");}
#define EMPTY_CTOR(name) \
JSBool name##_ctor (JSContext* cx, uintN argc, jsval* vp) { \
	THROW_ERROR(cx, "Invalid Operation"); }


#define JSAPI_PROP(name) JSBool name##(JSContext *cx, JSHandleObject obj, JSHandleId id, jsval *vp)
#define JSAPI_STRICT_PROP(name) JSBool name##(JSContext* cx, JSHandleObject obj, JSHandleId id, JSBool strict, jsval* vp)
//#define JSAPI_FUNC(fName) JSBool fName (JSContext *cx, JSObject *obj, uintN argc, jsval *rval, jsval *rval)
//#define JSAPI_PROP(fName) JSBool fName (JSContext *cx, JSObject *obj, jsval id, jsval *vp)

#define JS_PS(name, id, flags, getter, setter) \
	{name, id, flags, getter, setter}

#define JS_PS_END JS_PS(0, 0, 0, 0, 0)

#define JS_CS(classp, proto, ctor, argc, methods, props, static_methods, static_props) \
	{classp, proto, ctor, argc, methods, props, static_methods, static_props}

#define JS_CS_END {0}

#define JS_MS(name, classes, static_methods, static_properties) \
	{L##name, classes, static_methods, static_properties}

#define JS_MS_END {0, 0, 0, 0}

#define JSCLASS_SPEC(add, del, get, set, enumerate, resolve, convert, finalize, ctor) \
	add, del, get, set, enumerate, resolve, convert, finalize, \
    NULL,  NULL, NULL,ctor, NULL

#define JSCLASS_DEFAULT_WITH_CTOR(ctor) \
	JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub, \
				 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub, ctor)

#define JSCLASS_DEFAULT_STANDARD_MEMBERS \
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub, \
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub, \
    JSCLASS_NO_OPTIONAL_MEMBERS


template <class T> struct Converter {
	jsval operator()(JSContext* cx, const jschar* c) { return STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, c)); }
	jsval operator()(JSContext* cx, unsigned char c) { return UINT_TO_JSVAL(c); }
	jsval operator()(JSContext* cx, char* c) { return STRING_TO_JSVAL(JS_NewStringCopyZ(cx, c)); }
	jsval operator()(JSContext* cx, int i) { return INT_TO_JSVAL(i); }
	jsval operator()(JSContext* cx, double d) { return DOUBLE_TO_JSVAL(d); }
	jsval operator()(JSContext* cx, uint32 ui) { return UINT_TO_JSVAL(ui); }
	jsval operator()(JSContext* cx, bool b) { return BOOLEAN_TO_JSVAL(b); }

	const jschar* operator()(JSContext* cx, jsval v, const jschar*) { return JS_GetStringCharsZ(cx, JSVAL_TO_STRING(v)); }
	unsigned char operator()(JSContext* cx, jsval v, unsigned char) { return (unsigned char)JSVAL_TO_INT(v); }
	int operator()(JSContext* cx, jsval v, int) { return JSVAL_TO_INT(v); }
	double operator()(JSContext* cx, jsval v, double) { return JSVAL_TO_DOUBLE(v); }
	uint32 operator()(JSContext* cx, jsval v, uint32) { return (uint32)JSVAL_TO_INT(v); }
	bool operator()(JSContext* cx, jsval v, bool) { return !!JSVAL_TO_BOOLEAN(v); }
};

template<typename T>
inline JSBool __fastcall JS_ArrayToVector(JSContext* cx, JSObject* arr, std::vector<T>& vec)
{
	Converter<T> converter;
	if(!JS_IsArrayObject(cx, arr))
		return JS_ThrowError(cx, "Object must be an Array");

	jsuint len = 0;
	JS_GetArrayLength(cx, arr, &len);
	for(jsuint i = 0; i < len; i++)
	{
		jsval el = JSVAL_VOID;
		JS_GetElement(cx, arr, i, &el);
		vec.push_back(converter(cx, el, (T)0));
	}
	return JS_TRUE;
}

template<typename T>
inline JSObject* __fastcall JS_VectorToArray(JSContext* cx, std::vector<T>& vec)
{
	Converter<T> converter;
	size_t len = vec.size();
	jsval* jsarr = new jsval[len];

	auto end = vec.end();
	int i = 0;
	for(auto it = vec.begin(); it != end; it++, i++) jsarr[i] = converter(cx, vec[i]);

	JSObject* arr = JS_NewArrayObject(cx, len, jsarr);
	return arr;
}
