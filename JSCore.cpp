#include <io.h>
#include <windows.h>
#include <ddeml.h>
#include <wininet.h>
#include <iostream>

//#include "js32.h"
//#include "Script.h"
#include "JSCore.h"
#include "Core.h"
#include "Helpers.h"
#include "dde.h"
//#include "ScriptEngine.h"
//#include "D2BS.h"
#include "Events.h"
#include "Console.h"
#include "D2Ptrs.h"
#include "File.h"

#include "JSScript.h"

#pragma warning (disable : 4702)

JSAPI_FUNC(my_utf8ToEuc) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);

    if (argc == 0 || JSVAL_IS_NULL(JS_ARGV(cx, vp)[0])) {
        return JS_TRUE;
    }

    JS_BeginRequest(cx);
    if (!JS_ConvertValue(cx, JS_ARGV(cx, vp)[0], JSTYPE_STRING, &(JS_ARGV(cx, vp)[0]))) {
        JS_EndRequest(cx);
        JS_ReportError(cx, "Converting to string failed");
        return JS_FALSE;
    }
    JS_EndRequest(cx);
    char *Text = JS_EncodeString(cx, JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
    if (Text == NULL) {
        JS_ReportError(cx, "Could not get string for value");
        JS_free(cx, Text);
        return JS_FALSE;
    }

    wchar_t *szText = AnsiToUnicode(Text);
    char *euc = UnicodeToAnsi(szText, CP_ACP);
    JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, euc)));
    JS_free(cx, Text);
    delete[] szText;
    delete[] euc;

	return JS_TRUE;
}

JSAPI_FUNC(my_print) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    for (uintN i = 0; i < argc; i++) {
        if (!JSVAL_IS_NULL(JS_ARGV(cx, vp)[i])) {
            JS_BeginRequest(cx);
            if (!JS_ConvertValue(cx, JS_ARGV(cx, vp)[i], JSTYPE_STRING, &(JS_ARGV(cx, vp)[i]))) {
                JS_EndRequest(cx);
                JS_ReportError(cx, "Converting to string failed");
                return JS_FALSE;
            }
            JS_EndRequest(cx);
            char *Text = JS_EncodeString(cx, JS_ValueToString(cx, JS_ARGV(cx, vp)[i]));
            if (Text == NULL) {
                JS_ReportError(cx, "Could not get string for value");
                JS_free(cx, Text);
                return JS_FALSE;
            }

            // bob20				jsrefcount depth = JS_SuspendRequest(cx);
            if (!Text)
                Print("undefined");
            else {
                Print("%s", Text);
            }
            // bob20				JS_ResumeRequest(cx, depth);
            JS_free(cx, Text);
        }
    }
    return JS_TRUE;
}

JSAPI_FUNC(my_setTimeout) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);

    if (argc < 2 || !JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[1]))
        JS_ReportError(cx, "invalid params passed to setTimeout");

    if (JSVAL_IS_FUNCTION(cx, JS_ARGV(cx, vp)[0]) && JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[1])) {
        Script *self = (Script *)JS_GetContextPrivate(cx);
        int freq = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);
        self->RegisterEvent("setTimeout", JS_ARGV(cx, vp)[0]);
        Event *evt = new Event;
        evt->owner = self;
        evt->name = _strdup("setTimeout");
        evt->arg3 = new jsval(JS_ARGV(cx, vp)[0]);
        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(ScriptEngine::AddDelayedEvent(evt, freq)));
    }

    return JS_TRUE;
}

JSAPI_FUNC(my_setInterval) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);

    if (argc < 2 || !JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[1]))
        JS_ReportError(cx, "invalid params passed to setInterval");

    if (JSVAL_IS_FUNCTION(cx, JS_ARGV(cx, vp)[0]) && JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[1])) {
        Script *self = (Script *)JS_GetContextPrivate(cx);
        int freq = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);
        self->RegisterEvent("setInterval", JS_ARGV(cx, vp)[0]);
        Event *evt = new Event;
        evt->owner = self;
        evt->name = _strdup("setInterval");
        evt->arg3 = new jsval(JS_ARGV(cx, vp)[0]);
        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(ScriptEngine::AddDelayedEvent(evt, freq)));
    }

    return JS_TRUE;
}
JSAPI_FUNC(my_clearInterval) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    if (argc != 1 || !JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[0]))
        JS_ReportError(cx, "invalid params passed to clearInterval");

    ScriptEngine::RemoveDelayedEvent(JSVAL_TO_INT(JS_ARGV(cx, vp)[0]));
    return JS_TRUE;
}
JSAPI_FUNC(my_delay) {
    uint32 nDelay = 0;
    JS_BeginRequest(cx);
    if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "u", &nDelay)) {
        JS_EndRequest(cx);
        return JS_FALSE;
    }
    JS_EndRequest(cx);
    Script *script = (Script *)JS_GetContextPrivate(cx);
    DWORD start = GetTickCount();

    int amt = nDelay - (GetTickCount() - start);

    if (nDelay) {         // loop so we can exec events while in delay
        while (amt > 0) { // had a script deadlock here, make sure were positve with amt
            WaitForSingleObjectEx(script->eventSignal, amt, true);
            ResetEvent(script->eventSignal);
            if (script->IsAborted())
                break;

            while (script->EventList.size() > 0 && !!!(JSBool)(script->IsAborted() || ((script->GetState() == InGame) && ClientState() == ClientStateMenu))) {
                EnterCriticalSection(&Vars.cEventSection);
                Event *evt = script->EventList.back();
                script->EventList.pop_back();
                LeaveCriticalSection(&Vars.cEventSection);
                ExecScriptEvent(evt, false);
            }
            if (JS_GetGCParameter(script->GetRuntime(), JSGC_BYTES) - script->LastGC > 524288) // gc every .5 mb
            {
                JS_GC(JS_GetRuntime(cx));
                script->LastGC = JS_GetGCParameter(script->GetRuntime(), JSGC_BYTES);
            }
            /*else
                    JS_MaybeGC(cx);*/
            amt = nDelay - (GetTickCount() - start);
            // SleepEx(10,true);	// ex for delayed setTimer
        }

    } else
        JS_ReportWarning(cx, "delay(0) called, argument must be >= 1");
    return JS_TRUE;
}

JSAPI_FUNC(my_load) {
    JS_SET_RVAL(cx, vp, JSVAL_FALSE);

    Script *script = (Script *)JS_GetContextPrivate(cx);
    if (!script) {
        JS_ReportError(cx, "Failed to get script object");
        return JS_FALSE;
    }

    char *file = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));

    if (strlen(file) > (_MAX_FNAME + _MAX_PATH - strlen(Vars.szScriptPath))) {
        JS_ReportError(cx, "File name too long!");
        return JS_FALSE;
    }

    char buf[_MAX_PATH + _MAX_FNAME];
    ScriptState scriptState = script->GetState();
    if (scriptState == Command)
        scriptState = (ClientState() == ClientStateInGame ? InGame : OutOfGame);

    sprintf_s(buf, sizeof(buf), "%s\\%s", Vars.szScriptPath, file);
    StringReplace(buf, '/', '\\', _MAX_PATH + _MAX_FNAME);

    JSAutoStructuredCloneBuffer **autoBuffer = new JSAutoStructuredCloneBuffer *;
    for (uintN i = 1; i < argc; i++) {
        autoBuffer[i - 1] = new JSAutoStructuredCloneBuffer;
        autoBuffer[i - 1]->write(cx, JS_ARGV(cx, vp)[i]);
    }

    Script *newScript = ScriptEngine::CompileFile(buf, scriptState, argc - 1, autoBuffer);
    JS_free(cx, file);
    if (newScript) {
        newScript->BeginThread(ScriptThread);
        // JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(BuildObject(cx, &script_class, script_methods, script_props, newScript->GetContext())));
        JS_SET_RVAL(cx, vp, JSVAL_NULL);
    } else {
        // TODO: Should this actually be there? No notification is bad, but do we want this? maybe throw an exception?
        Print("File \"%s\" not found.", file);
        JS_SET_RVAL(cx, vp, JSVAL_NULL);
    }
    return JS_TRUE;
}

JSAPI_FUNC(my_include) {
    JS_SET_RVAL(cx, vp, JSVAL_FALSE);

    Script *script = (Script *)JS_GetContextPrivate(cx);
    if (!script) {
        JS_ReportError(cx, "Failed to get script object");
        return JS_FALSE;
    }

    char *file = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));

    if (strlen(file) > (_MAX_FNAME + _MAX_PATH - strlen(Vars.szScriptPath) - 6)) {
        JS_ReportError(cx, "File name too long!");
        return JS_FALSE;
    }

    char buf[_MAX_PATH + _MAX_FNAME];
    sprintf_s(buf, sizeof(buf), "%s\\libs\\%s", Vars.szScriptPath, file);
    if (_access(buf, 0) == 0)
        JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(script->Include(buf)));

    JS_free(cx, file);
    return JS_TRUE;
}

JSAPI_FUNC(my_stop) {
    if (argc > 0 && (JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) && JSVAL_TO_INT(JS_ARGV(cx, vp)[0]) == 1) ||
        (JSVAL_IS_BOOLEAN(JS_ARGV(cx, vp)[0]) && JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[0]) == TRUE)) {
        Script *script = (Script *)JS_GetContextPrivate(cx);
        if (script)
            script->Stop();
    } else
        ScriptEngine::StopAll();

    return JS_FALSE;
}

JSAPI_FUNC(my_beep) {
    jsint nBeepId = NULL;

    if (argc > 0 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
        nBeepId = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);

    MessageBeep(nBeepId);

    return JS_TRUE;
}

JSAPI_FUNC(my_getTickCount) {
    JS_BeginRequest(cx);
    jsval rval;
    rval = JS_NumberValue((jsdouble)GetTickCount());
    JS_SET_RVAL(cx, vp, rval);
    JS_EndRequest(cx);
    return JS_TRUE;
}

JSAPI_FUNC(my_getThreadPriority) {
    JS_SET_RVAL(cx, vp, INT_TO_JSVAL((uint)GetCurrentThread()));
    return JS_TRUE;
}

JSAPI_FUNC(my_isIncluded) {
    char *file = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));

    if (strlen(file) > (_MAX_FNAME + _MAX_PATH - strlen(Vars.szScriptPath) - 6)) {
        JS_ReportError(cx, "File name too long");
        return JS_FALSE;
    }

    char path[_MAX_FNAME + _MAX_PATH];
    sprintf_s(path, _MAX_FNAME + _MAX_PATH, "%s\\libs\\%s", Vars.szScriptPath, file);
    Script *script = (Script *)JS_GetContextPrivate(cx);
    JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(script->IsIncluded(path)));
    JS_free(cx, file);
    return JS_TRUE;
}

JSAPI_FUNC(my_version) {
    if (argc < 1) {
        JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_InternString(cx, D2BS_VERSION)));
        return JS_TRUE;
    }

    Print("ÿc4D2BSÿc1 ÿc3%s for Diablo II 1.14d.", D2BS_VERSION);

    return JS_TRUE;
}

JSAPI_FUNC(my_debugLog) {
    for (uintN i = 0; i < argc; i++) {
        if (!JSVAL_IS_NULL(JS_ARGV(cx, vp)[i])) {
            JS_BeginRequest(cx);
            if (!JS_ConvertValue(cx, JS_ARGV(cx, vp)[i], JSTYPE_STRING, &(JS_ARGV(cx, vp)[i]))) {
                JS_EndRequest(cx);
                JS_ReportError(cx, "Converting to string failed");
                return JS_FALSE;
            }
            JS_EndRequest(cx);
            char *Text = JS_EncodeString(cx, JS_ValueToString(cx, JS_ARGV(cx, vp)[i]));
            if (Text == NULL) {
                JS_ReportError(cx, "Could not get string for value");
                return JS_FALSE;
            }

            // bob20			jsrefcount depth = JS_SuspendRequest(cx);
            if (!Text)
                Log("undefined");
            else {
                Log("%s", Text);
            }
            // bob20				JS_ResumeRequest(cx, depth);
            JS_free(cx, Text);
        }
    }

    return JS_TRUE;
}
JSAPI_FUNC(my_copy) {
    char *data = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));

    JS_ValueToString(cx, JS_ARGV(cx, vp)[0]);
    HGLOBAL hText;
    char *pText;
    hText = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, strlen(data) + 1);
    pText = (char *)GlobalLock(hText);
    //char *tempData;

    strcpy_s(pText, strlen(data) + 1, data);
    GlobalUnlock(hText);

    OpenClipboard(NULL);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hText);
    CloseClipboard();
    JS_free(cx, data);
    return JS_TRUE;
}
JSAPI_FUNC(my_paste) {
    OpenClipboard(NULL);
    HANDLE foo = GetClipboardData(CF_TEXT);
    CloseClipboard();
    LPVOID lptstr = GlobalLock(foo);
    (char *)lptstr;
    JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, (char *)lptstr)));
    return JS_TRUE;
}
JSAPI_FUNC(my_sendCopyData) {
    JS_SET_RVAL(cx, vp, JSVAL_FALSE);
    if (argc < 4) {
        return JS_TRUE;
    }

    char *windowClassName = NULL, *windowName = NULL, *data = NULL;
    jsint nModeId = NULL;
    HWND hWnd = NULL;

    JS_BeginRequest(cx);
    if (JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]) && !JSVAL_IS_NULL(JS_ARGV(cx, vp)[0])) {
        windowClassName = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
    }

    if (!JSVAL_IS_NULL(JS_ARGV(cx, vp)[1])) {
        if (JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[1])) {
            JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[1], (uint32 *)&hWnd);
        } else if (JSVAL_IS_STRING(JS_ARGV(cx, vp)[1])) {
            windowName = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[1]));
        }
    }

    if (JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[2]) && !JSVAL_IS_NULL(JS_ARGV(cx, vp)[2]))
        JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[2], (uint32 *)&nModeId);

    if (JSVAL_IS_STRING(JS_ARGV(cx, vp)[3]) && !JSVAL_IS_NULL(JS_ARGV(cx, vp)[3])) {
        data = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[3]));
    }
    JS_EndRequest(cx);

    if (hWnd == NULL && windowName != NULL) {
        hWnd = FindWindow(windowClassName, windowName);
        if (!hWnd) {
            return JS_TRUE;
        }
    }

    // if data is NULL, strlen crashes
    if (data == NULL)
        data = "";

    COPYDATASTRUCT aCopy = {nModeId, strlen(data) + 1, data};

    // bob20	 jsrefcount depth = JS_SuspendRequest(cx);
    JS_SET_RVAL(cx, vp, INT_TO_JSVAL(SendMessage(hWnd, WM_COPYDATA, (WPARAM)D2GFX_GetHwnd(), (LPARAM)&aCopy)));
    // bob20	 JS_ResumeRequest(cx, depth);

    JS_free(cx, data);
    JS_free(cx, windowName);
    JS_free(cx, windowClassName);
    return JS_TRUE;
}

JSAPI_FUNC(my_sendDDE) {
    jsint mode;
    char *pszDDEServer = "\"\"", *pszTopic = "\"\"", *pszItem = "\"\"", *pszData = "\"\"";
    JS_BeginRequest(cx);

    if (JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
        JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[1], (uint32 *)&mode);

    if (JSVAL_IS_STRING(JS_ARGV(cx, vp)[1]))
        pszDDEServer = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[1]));

    if (JSVAL_IS_STRING(JS_ARGV(cx, vp)[2]))
        pszTopic = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[2]));

    if (JSVAL_IS_STRING(JS_ARGV(cx, vp)[3]))
        pszItem = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[3]));

    if (JSVAL_IS_STRING(JS_ARGV(cx, vp)[4]))
        pszData = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[4]));

    JS_EndRequest(cx);
    char buffer[255] = "";
    if (SendDDE(mode, pszDDEServer, pszTopic, pszItem, pszData, (char **)&buffer, 255)) {
        if (mode == 0)
            JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, buffer)));
    } else
        THROW_ERROR(cx, "DDE Failed! Check the log for the error message.");

    return JS_TRUE;
}

JSAPI_FUNC(my_keystate) {
    if (argc < 1 || !JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
        return JS_TRUE;

    JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(!!GetAsyncKeyState(JSVAL_TO_INT(JS_ARGV(cx, vp)[0]))));

    return JS_TRUE;
}

JSAPI_FUNC(my_addEventListener) {
    if (JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]) && JSVAL_IS_FUNCTION(cx, JS_ARGV(cx, vp)[1])) {
        char *evtName = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
        if (evtName && strlen(evtName)) {
            Script *self = (Script *)JS_GetContextPrivate(cx);
            self->RegisterEvent(JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0])), JS_ARGV(cx, vp)[1]);
        } else
            THROW_ERROR(cx, "Event name is invalid!");
        JS_free(cx, evtName);
    }
    return JS_TRUE;
}

JSAPI_FUNC(my_removeEventListener) {
    if (JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]) && JSVAL_IS_FUNCTION(cx, JS_ARGV(cx, vp)[1])) {
        char *evtName = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
        if (evtName && strlen(evtName)) {
            Script *self = (Script *)JS_GetContextPrivate(cx);
            self->UnregisterEvent(evtName, JS_ARGV(cx, vp)[1]);
        } else
            THROW_ERROR(cx, "Event name is invalid!");
        JS_free(cx, evtName);
    }
    return JS_TRUE;
}

JSAPI_FUNC(my_clearEvent) {
    if (JSVAL_IS_STRING(JS_ARGV(cx, vp)[0])) {
        Script *self = (Script *)JS_GetContextPrivate(cx);
        char *evt = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
        self->ClearEvent(evt);
        JS_free(cx, evt);
    }
    return JS_TRUE;
}

JSAPI_FUNC(my_clearAllEvents) {
    Script *self = (Script *)JS_GetContextPrivate(cx);
    self->ClearAllEvents();
    return JS_TRUE;
}

JSAPI_FUNC(my_js_strict) {
    if (argc == NULL) {
        JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(((JS_GetOptions(cx) & JSOPTION_STRICT) == JSOPTION_STRICT)));
        return JS_TRUE;
    }

    if (argc == 1) {
        bool bFlag = ((JS_GetOptions(cx) & JSOPTION_STRICT) == JSOPTION_STRICT);
        if (JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[0])) {
            if (!bFlag)
                JS_ToggleOptions(cx, JSOPTION_STRICT);
        } else {
            if (bFlag)
                JS_ToggleOptions(cx, JSOPTION_STRICT);
        }
    }

    return JS_TRUE;
}

JSAPI_FUNC(my_scriptBroadcast) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    if (argc < 1)
        THROW_ERROR(cx, "You must specify something to broadcast");

    ScriptBroadcastEvent(cx, argc, JS_ARGV(cx, vp));
    return JS_TRUE;
}

JSAPI_FUNC(my_showConsole) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    Console::Show();
    return JS_TRUE;
}

JSAPI_FUNC(my_hideConsole) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    Console::Hide();
    return JS_TRUE;
}

JSAPI_FUNC(my_handler) {
    JS_SET_RVAL(cx, vp, UINT_TO_JSVAL((uint32_t)Vars.hHandle));
    return JS_TRUE;
}

JSAPI_FUNC(my_loadMpq) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    char *path = JS_EncodeString(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));

    if (isValidPath(path))
        LoadMPQ(path);
    JS_free(cx, path);
    return JS_TRUE;
}

JSAPI_FUNC(my_sendPacket) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);

    if (!Vars.bEnableUnsupported) {
        THROW_WARNING(cx, vp, "sendPacket requires EnableUnsupported = true in d2bs.ini");
        return JS_TRUE;
    }

    JS_BeginRequest(cx);
    BYTE *aPacket;
    uint32 len = 0;

    if (JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[0])) {
        JSObject *obj;
        JS_ValueToObject(cx, JS_ARGV(cx, vp)[0], &obj);

        if (!JS_IsArrayBufferObject(obj)) {
            THROW_WARNING(cx, vp, "invalid ArrayBuffer parameter");
            JS_EndRequest(cx);
            return JS_TRUE;
        }

        len = JS_GetArrayBufferByteLength(obj);
        aPacket = new BYTE[len];
        memcpy(aPacket, JS_GetArrayBufferData(obj), len);
    } else {
        if (argc % 2 != 0) {
            THROW_WARNING(cx, vp, "invalid packet format");
            JS_EndRequest(cx);
            return JS_TRUE;
        }

        aPacket = new BYTE[2 * argc];
        uint32 size = 0;

        for (uint i = 0; i < argc; i += 2, len += size) {
            JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[i], &size);
            JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[i + 1], (uint32_t *)&aPacket[len]);
        }
    }

    JS_EndRequest(cx);
    D2NET_SendPacket(len, 1, aPacket);
    delete[] aPacket;
    JS_SET_RVAL(cx, vp, JSVAL_TRUE);
    return JS_TRUE;
}

JSAPI_FUNC(my_getPacket) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);

    if (!Vars.bEnableUnsupported) {
        THROW_WARNING(cx, vp, "getPacket requires EnableUnsupported = true in d2bs.ini");
        return JS_TRUE;
    }

    JS_BeginRequest(cx);
    BYTE *aPacket;
    uint32 len = 0;

    if (JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[0])) {
        JSObject *obj;
        JS_ValueToObject(cx, JS_ARGV(cx, vp)[0], &obj);

        if (!JS_IsArrayBufferObject(obj)) {
            THROW_WARNING(cx, vp, "invalid ArrayBuffer parameter");
            JS_EndRequest(cx);
            return JS_TRUE;
        }

        len = JS_GetArrayBufferByteLength(obj);
        aPacket = new BYTE[len];
        memcpy(aPacket, JS_GetArrayBufferData(obj), len);
    } else {
        if (argc % 2 != 0) {
            THROW_WARNING(cx, vp, "invalid packet format");
            JS_EndRequest(cx);
            return JS_TRUE;
        }

        aPacket = new BYTE[2 * argc];
        uint32 size = 0;

        for (uint i = 0; i < argc; i += 2, len += size) {
            JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[i], &size);
            JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[i + 1], (uint32_t *)&aPacket[len]);
        }
    }

    JS_EndRequest(cx);
    D2NET_ReceivePacket(aPacket, len);
    delete[] aPacket;
    JS_SET_RVAL(cx, vp, JSVAL_TRUE);
    return JS_TRUE;
}

#pragma comment(lib, "wininet")
JSAPI_FUNC(my_getIP) {
    HINTERNET hInternet, hFile;
    DWORD rSize;
    char buffer[32];

    hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    hFile = InternetOpenUrl(hInternet, "http://ipv4bot.whatismyipaddress.com", NULL, 0, INTERNET_FLAG_RELOAD, 0);
    InternetReadFile(hFile, &buffer, sizeof(buffer), &rSize);
    buffer[min(rSize, 31)] = '\0';
    InternetCloseHandle(hFile);
    InternetCloseHandle(hInternet);
    JS_BeginRequest(cx);
    JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, (char *)buffer)));
    JS_EndRequest(cx);
    return JS_TRUE;
}

JSAPI_FUNC(my_sendClick) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    uint32 x = 0;
    uint32 y = 0;
    JS_BeginRequest(cx);
    if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "uu", &x, &y)) {
        JS_EndRequest(cx);
        return JS_FALSE;
    }
    JS_EndRequest(cx);
    Sleep(100);
    SendMouseClick(x, y, 0);
    Sleep(100);
    SendMouseClick(x, y, 1);
    Sleep(100);
    return JS_TRUE;
}

JSAPI_FUNC(my_sendKey) {
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    uint32 key;
    JS_BeginRequest(cx);
    if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "u", &key)) {
        JS_EndRequest(cx);
        return JS_FALSE;
    }
    JS_EndRequest(cx);
    BOOL prompt = Console::IsEnabled();
    if (prompt) {
        Console::HidePrompt();
    }
    Sleep(100);
    SendKeyPress(WM_KEYDOWN, key, 0);
    Sleep(100);
    SendKeyPress(WM_KEYUP, key, 0);
    Sleep(100);
    if (prompt) {
        Console::ShowPrompt();
    }
    return JS_TRUE;
}