#include "ScriptEngine.h"
#include "js32.h"
//#include <cstdarg>

JSObject* BuildObject(JSContext* cx, JSClass* classp, JSFunctionSpec* funcs, JSPropertySpec* props, void* priv, JSObject* proto, JSObject* parent)
{
	JS_BeginRequest(cx);
	
	JSObject* obj = JS_NewObject(cx, classp, proto, parent);

	if(obj)
	{
		// add root to avoid newborn root problem
		JS_AddObjectRoot(cx, &obj);
		//if(JS_AddRoot(&obj) == JS_FALSE)
			//return NULL;
		if(obj && funcs && !JS_DefineFunctions(cx, obj, funcs))
			obj = NULL;
		if(obj && props && !JS_DefineProperties(cx, obj, props))
			obj = NULL;
		if(obj && priv)
			JS_SetPrivate(cx, obj, priv);
		JS_RemoveObjectRoot(cx, &obj);
	}
	JS_EndRequest(cx);
	return obj;
}
JSScript* JS_CompileFile(JSContext* cx, JSObject* globalObject, std::string fileName)

{
	std::ifstream t(fileName.c_str());
	std::string str;

	t.seekg(0, std::ios::end);   
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(t)),
            std::istreambuf_iterator<char>());

	JSScript* rval = JS_CompileScript(cx, globalObject,str.c_str(), str.size() , fileName.c_str(),1);
	JS_AddNamedScriptRoot (cx, &rval, fileName.c_str());
	JS_RemoveScriptRoot(cx, &rval);
	
return rval;
}
JSBool JSVAL_IS_OBJECT(jsval v){return !JSVAL_IS_PRIMITIVE(v);}
void * JS_GetPrivate(JSContext *cx, JSObject *obj){ return JS_GetPrivate(obj);}
void JS_SetPrivate(JSContext *cx, JSObject *obj, void *data){ return JS_SetPrivate(obj ,data);}

