#include "ScriptEngine.h"
#include "js32.h"
#include "D2Helpers.h"
#include "Helpers.h"
#include "File.h"
//#include <cstdarg>

JSObject* BuildObject(JSContext* cx, JSClass* classp, JSFunctionSpec* funcs, JSPropertySpec* props, void* priv, JSObject* proto, JSObject* parent) {
    JS_BeginRequest(cx);

    JSObject* obj = JS_NewObject(cx, classp, proto, parent);

    if (obj) {
        // add root to avoid newborn root problem
        JS_AddObjectRoot(cx, &obj);
        // if(JS_AddRoot(&obj) == JS_FALSE)
        // return NULL;
        if (obj && funcs && !JS_DefineFunctions(cx, obj, funcs))
            obj = NULL;
        if (obj && props && !JS_DefineProperties(cx, obj, props))
            obj = NULL;
        if (obj && priv)
            JS_SetPrivate(cx, obj, priv);
        JS_RemoveObjectRoot(cx, &obj);
    }
    JS_EndRequest(cx);
    return obj;
}
JSScript* JS_CompileFile(JSContext* cx, JSObject* globalObject, std::wstring fileName)
{
    std::ifstream t(fileName.c_str(), std::ios::binary);
    std::string str;

    int ch1 = t.get();
    int ch2 = t.get();
    int ch3 = t.get();

    t.seekg(0, std::ios::end);
    str.resize(t.tellg());
    t.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

    if (ch1 == 0xEF && ch2 == 0xBB && ch3 == 0xBF) { // replace utf8-bom with space
        str[0] = ' ';
        str[1] = ' ';
        str[2] = ' ';
    }

    char* nFileName = UnicodeToAnsi(fileName.c_str());
    //TODO: FIX THIS PROPERLY SO IT WORKS WITH REAL UNICODE PATHS
    JSScript* rval = JS_CompileScript(cx, globalObject, str.c_str(), str.size(), nFileName, 1);
    //wchar_t* wStr = AnsiToUnicode(str.c_str());
    //JSScript* rval = JS_CompileUCScript(cx, globalObject, wStr, wcslen(wStr), nFileName, 1);
    JS_AddNamedScriptRoot(cx, &rval, nFileName);
    JS_RemoveScriptRoot(cx, &rval);
    delete[] nFileName;

    return rval;
}
JSBool JSVAL_IS_OBJECT(jsval v) {
    return !JSVAL_IS_PRIMITIVE(v);
}
void* JS_GetPrivate(JSContext* cx, JSObject* obj) {
    return JS_GetPrivate(obj);
}
void JS_SetPrivate(JSContext* cx, JSObject* obj, void* data) {
    return JS_SetPrivate(obj, data);
}
