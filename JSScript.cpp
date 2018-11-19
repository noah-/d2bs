#include "JSScript.h"
#include "Script.h"
#include "ScriptEngine.h"
#include "D2BS.h"
#include "Helpers.h"

EMPTY_CTOR(script)

struct FindHelper {
    DWORD tid;
    char* name;
    Script* script;
};

bool __fastcall FindScriptByTid(Script* script, void* argv, uint argc);
bool __fastcall FindScriptByName(Script* script, void* argv, uint argc);

JSAPI_PROP(script_getProperty) {
    Script* script = (Script*)JS_GetInstancePrivate(cx, obj, &script_class, NULL);

    // TODO: make this check stronger
    if (!script)
        return JS_TRUE;
    jsval ID;
    JS_IdToValue(cx, id, &ID);
    switch (JSVAL_TO_INT(ID)) {
    case SCRIPT_FILENAME:
        vp.setString(JS_InternString(cx, script->GetShortFilename()));
        break;
    case SCRIPT_GAMETYPE:
        vp.setBoolean(script->GetState() == InGame ? false : true);
        break;
    case SCRIPT_RUNNING:
        vp.setBoolean(script->IsRunning());
        break;
    case SCRIPT_THREADID:
        vp.setInt32(script->GetThreadId());
        break;
    case SCRIPT_MEMORY:
        vp.setInt32(JS_GetGCParameter(script->GetRuntime(), JSGC_BYTES));
        break;
    default:
        break;
    }

    return JS_TRUE;
}

JSAPI_FUNC(script_getNext) {

    Script* iterp = (Script*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &script_class, NULL);
    ScriptEngine::LockScriptList("scrip.getNext");
    // EnterCriticalSection(&ScriptEngine::lock);

    for (ScriptMap::iterator it = ScriptEngine::scripts.begin(); it != ScriptEngine::scripts.end(); it++) {
        if (it->second == iterp) {
            it++;
            if (it == ScriptEngine::scripts.end())
                break;
            iterp = it->second;
            JS_SetPrivate(cx, JS_THIS_OBJECT(cx, vp), iterp);
            JS_SET_RVAL(cx, vp, JSVAL_TRUE);
            ScriptEngine::UnLockScriptList("scrip.getNext");
            // LeaveCriticalSection(&ScriptEngine::lock);
            return JS_TRUE;
        }
    }

    // LeaveCriticalSection(&ScriptEngine::lock);
    ScriptEngine::UnLockScriptList("scrip.getNext");

    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JSAPI_FUNC(script_stop) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    Script* script = (Script*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &script_class, NULL);
    if (script->IsRunning())
        script->Stop();

    return JS_TRUE;
}

JSAPI_FUNC(script_pause) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    Script* script = (Script*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &script_class, NULL);

    if (script->IsRunning())
        script->Pause();

    return JS_TRUE;
}

JSAPI_FUNC(script_resume) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    Script* script = (Script*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &script_class, NULL);

    if (script->IsPaused())
        script->Resume();

    return JS_TRUE;
}

JSAPI_FUNC(script_send) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    Script* script = (Script*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &script_class, NULL);
    Event* evt = new Event;
    if (!script || !script->IsRunning())
        return JS_TRUE;
    ScriptEngine::LockScriptList("script.send");
    evt->owner = script;
    evt->argc = argc;
    evt->name = strdup("scriptmsg");
    evt->arg1 = new DWORD(argc);
    evt->argv = new JSAutoStructuredCloneBuffer*;
    for (uint i = 0; i < argc; i++) {
        evt->argv[i] = new JSAutoStructuredCloneBuffer;
        evt->argv[i]->write(cx, JS_ARGV(cx, vp)[i]);
    }

    EnterCriticalSection(&Vars.cEventSection);
    evt->owner->EventList.push_front(evt);
    LeaveCriticalSection(&Vars.cEventSection);
    evt->owner->TriggerOperationCallback();
    ScriptEngine::UnLockScriptList("script.send");

    return JS_TRUE;
}

JSAPI_FUNC(script_join) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    Script* script = (Script*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &script_class, NULL);

    script->Join();

    return JS_TRUE;
}

JSAPI_FUNC(my_getScript) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    Script* iterp = NULL;
    if (argc == 1 && JSVAL_IS_BOOLEAN(JS_ARGV(cx, vp)[0]) && JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[0]) == JS_TRUE)
        iterp = (Script*)JS_GetContextPrivate(cx);
    else if (argc == 1 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0])) {
        // loop over the Scripts in ScriptEngine and find the one with the right threadid
        DWORD tid = (DWORD)JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
        FindHelper args = {tid, NULL, NULL};
        ScriptEngine::ForEachScript(FindScriptByTid, &args, 1);
        if (args.script != NULL)
            iterp = args.script;
        else
            return JS_TRUE;
    } else if (argc == 1 && JSVAL_IS_STRING(JS_ARGV(cx, vp)[0])) {
        char* name = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
        if (name)
            StringReplace(name, '/', '\\', strlen(name));
        FindHelper args = {0, name, NULL};
        ScriptEngine::ForEachScript(FindScriptByName, &args, 1);
        JS_free(cx, name);
        if (args.script != NULL)
            iterp = args.script;
        else
            return JS_TRUE;
    } else {
        if (ScriptEngine::scripts.size() > 0) {
            //	EnterCriticalSection(&ScriptEngine::lock);
            ScriptEngine::LockScriptList("getScript");
            iterp = ScriptEngine::scripts.begin()->second;
            ScriptEngine::UnLockScriptList("getScript");
            //	LeaveCriticalSection(&ScriptEngine::lock);
        }

        if (iterp == NULL)
            return JS_TRUE;
    }

    JSObject* res = BuildObject(cx, &script_class, script_methods, script_props, iterp);

    if (!res)
        THROW_ERROR(cx, "Failed to build the script object");
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(res));

    return JS_TRUE;
}
JSAPI_FUNC(my_getScripts) {

    DWORD dwArrayCount = NULL;

    JSObject* pReturnArray = JS_NewArrayObject(cx, 0, NULL);
    JS_BeginRequest(cx);
    JS_AddRoot(cx, &pReturnArray);
    ScriptEngine::LockScriptList("getScripts");

    for (ScriptMap::iterator it = ScriptEngine::scripts.begin(); it != ScriptEngine::scripts.end(); it++) {
        JSObject* res = BuildObject(cx, &script_class, script_methods, script_props, it->second);
        jsval a = OBJECT_TO_JSVAL(res);
        JS_SetElement(cx, pReturnArray, dwArrayCount, &a);
        dwArrayCount++;
    }

    ScriptEngine::UnLockScriptList("getScripts");
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(pReturnArray));
    JS_RemoveRoot(cx, &pReturnArray);
    JS_EndRequest(cx);
    return JS_TRUE;
}
bool __fastcall FindScriptByName(Script* script, void* argv, uint argc) {
    FindHelper* helper = (FindHelper*)argv;
    static uint pathlen = strlen(Vars.szScriptPath) + 1;
    const char* fname = script->GetShortFilename();
    if (_strcmpi(fname, helper->name) == 0) {
        helper->script = script;
        return false;
    }
    return true;
}

bool __fastcall FindScriptByTid(Script* script, void* argv, uint argc) {
    FindHelper* helper = (FindHelper*)argv;
    if (script->GetThreadId() == helper->tid) {
        helper->script = script;
        return false;
    }
    return true;
}
