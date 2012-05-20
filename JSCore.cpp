#include <io.h>
#include <windows.h>
#include <ddeml.h>

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

JSAPI_FUNC(my_print)
{
	for(uintN i = 0; i < argc; i++)
	{
		if(!JSVAL_IS_NULL(JS_ARGV(cx, vp)[i]))
		{
			if(!JS_ConvertValue(cx, JS_ARGV(cx, vp)[i], JSTYPE_STRING, &(JS_ARGV(cx, vp)[i])))
			{
				JS_ReportError(cx, "Converting to string failed");
				return JS_FALSE;
			}

			char* Text = JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[i]));
			if(Text == NULL)
			{
				JS_ReportError(cx, "Could not get string for value");
				return JS_FALSE;
			}

			jsrefcount depth = JS_SuspendRequest(cx);
			if(!Text)
				Print("undefined");
			else {
				std::replace(Text, Text + strlen(Text), '%', (char)(unsigned char)0xFE);
				Print(Text);
			}
			JS_ResumeRequest(cx, depth);
		}
	}
	return JS_TRUE;
}

JSAPI_FUNC(my_delay)
{
	uint32 nDelay = 0;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "u", &nDelay))
		return JS_FALSE;

	if(nDelay)
	{
		jsrefcount depth = JS_SuspendRequest(cx);
		Sleep(nDelay);
		JS_ResumeRequest(cx, depth);
	}
	else
		JS_ReportWarning(cx, "delay(0) called, argument must be >= 1");

	return JS_TRUE;
}

JSAPI_FUNC(my_load)
{
	JS_SET_RVAL(cx, vp, JSVAL_FALSE);

	Script* script = (Script*)JS_GetContextPrivate(cx);
	if(!script)
	{
		JS_ReportError(cx, "Failed to get script object");
		return JS_FALSE;
	}

	char* file = NULL;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "s", &file))
		return JS_FALSE;

	if(strlen(file) > (_MAX_FNAME + _MAX_PATH - strlen(Vars.szScriptPath)))
	{
		JS_ReportError(cx, "File name too long!");
		return JS_FALSE;
	}

	char buf[_MAX_PATH+_MAX_FNAME];
	ScriptState scriptState = script->GetState();
	if(scriptState == Command)
		scriptState = (ClientState() == ClientStateInGame ? InGame : OutOfGame);

	sprintf_s(buf, sizeof(buf), "%s\\%s", Vars.szScriptPath, file);
	StringReplace(buf, '/', '\\', _MAX_PATH+_MAX_FNAME);
	Script* newScript = ScriptEngine::CompileFile(buf, scriptState, argc-1, JS_ARGV(cx, vp)+1);
	if(newScript)
	{
		newScript->BeginThread(ScriptThread);
		JSObject* res = BuildObject(cx, &script_class, script_methods, script_props, newScript->GetContext());
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(res));
	}
	else
	{
		// TODO: Should this actually be there? No notification is bad, but do we want this? maybe throw an exception?
		Print("File \"%s\" not found.", file);
		JS_SET_RVAL(cx, vp,JSVAL_NULL );		
	}

	return JS_TRUE;
}

JSAPI_FUNC(my_include)
{
	JS_SET_RVAL(cx, vp, JSVAL_FALSE);
	
	Script* script = (Script*)JS_GetContextPrivate(cx);
	if(!script)
	{
		JS_ReportError(cx, "Failed to get script object");
		return JS_FALSE;
	}

	char* file = NULL;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "s", &file))
		return JS_FALSE;

	if(strlen(file) > (_MAX_FNAME + _MAX_PATH - strlen(Vars.szScriptPath) - 6))
	{
		JS_ReportError(cx, "File name too long!");
		return JS_FALSE;
	}

	char buf[_MAX_PATH+_MAX_FNAME];
	sprintf_s(buf, sizeof(buf), "%s\\libs\\%s", Vars.szScriptPath, file);
	if(_access(buf, 0) == 0)
		JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(script->Include(buf)) );
		

	return JS_TRUE;
}

JSAPI_FUNC(my_stop)
{
	if(argc > 0 && (JSVAL_IS_INT(JS_ARGV(cx, vp)[0]) && JSVAL_TO_INT(JS_ARGV(cx, vp)[0]) == 1) ||
			(JSVAL_IS_BOOLEAN(JS_ARGV(cx, vp)[0]) && JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[0]) == TRUE))
	{
		Script* script = (Script*)JS_GetContextPrivate(cx);
		if(script)
			script->Stop();
	}
	else
		ScriptEngine::StopAll();

	return JS_FALSE;
}

JSAPI_FUNC(my_beep)
{
	jsint nBeepId = NULL;

	if(argc > 0 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		nBeepId = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);

	MessageBeep(nBeepId);

	return JS_TRUE;
}

JSAPI_FUNC(my_getTickCount)
{
	jsval rval;
	JS_NewNumberValue(cx, (jsdouble)GetTickCount(), &rval);
	JS_SET_RVAL(cx, vp, rval);
	return JS_TRUE;
}

JSAPI_FUNC(my_getThreadPriority)
{
	//JS_SET_RVAL(cx, vp, *(jsval)GetThreadPriority(GetCurrentThread()));
	return JS_TRUE;
}

JSAPI_FUNC(my_isIncluded)
{
	char* file = NULL;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "s", &file))
		return JS_FALSE;

	if(strlen(file) > (_MAX_FNAME+_MAX_PATH-strlen(Vars.szScriptPath)-6))
	{
		JS_ReportError(cx, "File name too long");
		return JS_FALSE;
	}

	char path[_MAX_FNAME+_MAX_PATH];
	sprintf_s(path, _MAX_FNAME+_MAX_PATH, "%s\\libs\\%s", Vars.szScriptPath, file);
	Script* script = (Script*)JS_GetContextPrivate(cx);
	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(script->IsIncluded(path)));
	
	return JS_TRUE;
}

JSAPI_FUNC(my_version)
{
	if(argc < 1)
	{
		JS_SET_RVAL(cx, vp,  STRING_TO_JSVAL(JS_InternString(cx, D2BS_VERSION)));
		return JS_TRUE;
	}

	Print("ÿc4D2BSÿc1 ÿc3%s for Diablo II 1.13c.", D2BS_VERSION); 

	return JS_TRUE;
}

JSAPI_FUNC(my_debugLog)
{
	for(uintN i = 0; i < argc; i++)
	{
		if(!JSVAL_IS_NULL(JS_ARGV(cx, vp)[i]))
		{
			if(!JS_ConvertValue(cx, JS_ARGV(cx, vp)[i], JSTYPE_STRING, &(JS_ARGV(cx, vp)[i])))
			{
				JS_ReportError(cx, "Converting to string failed");
				return JS_FALSE;
			}

			char* Text = JS_EncodeString(cx,JS_ValueToString(cx, JS_ARGV(cx, vp)[i]));
			if(Text == NULL)
			{
				JS_ReportError(cx, "Could not get string for value");
				return JS_FALSE;
			}

			jsrefcount depth = JS_SuspendRequest(cx);
			if(!Text)
				Log("undefined");
			else {
				StringReplace(Text, '%', (unsigned char)0xFE, strlen(Text));
				Log(Text);
			}
			JS_ResumeRequest(cx, depth);
		}
	}

	return JS_TRUE;
}
JSAPI_FUNC(my_copy)
{
	char  *data = NULL;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "s", &data))
		return JS_FALSE;

	HGLOBAL hText;
	char *pText;
	hText = GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE, strlen(data)+1);
	pText = (char*)GlobalLock(hText);
	strcpy_s(pText,strlen(data)+1, data);
	GlobalUnlock(hText);

	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hText);
	CloseClipboard();
	return JS_TRUE;
}
JSAPI_FUNC(my_paste)
{
	OpenClipboard(NULL);
	HANDLE foo = GetClipboardData(CF_TEXT);
	CloseClipboard();
	LPVOID lptstr = GlobalLock(foo);
	(char *)lptstr;
	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, (char *)lptstr)));
	return JS_TRUE;
}
JSAPI_FUNC(my_sendCopyData)
{
	if(argc < 4)
	{
		JS_SET_RVAL(cx, vp, JSVAL_FALSE);
		return JS_TRUE;
	}
	char *windowClassName = NULL, *windowName = NULL, *data = NULL;
	jsint nModeId = NULL;
	HWND hWnd = NULL;
		
	if(argc > 1 && JSVAL_IS_NUMBER(JS_ARGV(cx, vp)[1]) && !JSVAL_IS_NULL(JS_ARGV(cx, vp)[1]))
		JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[1], (uint32*)&hWnd);

	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "ssis", &windowClassName, &windowName, &nModeId, &data))
		return JS_FALSE;

	if(_strcmpi(windowClassName, "null") == 0)
		windowClassName = NULL;
	if(_strcmpi(windowName, "null") == 0)
		windowName = NULL;
	if(hWnd == NULL)
	{
		hWnd = FindWindow(windowClassName, windowName);
		if(!hWnd)
		{
			JS_SET_RVAL(cx, vp, JSVAL_FALSE);
			return JS_TRUE;
		}
	}
	// if data is NULL, strlen crashes
	if(data == NULL)
		data = "";

	COPYDATASTRUCT aCopy = { nModeId, strlen(data)+1, data };
	JS_SET_RVAL(cx, vp, INT_TO_JSVAL(SendMessage(hWnd, WM_COPYDATA, (WPARAM)D2GFX_GetHwnd(), (LPARAM)&aCopy)));

	return JS_TRUE;
}

JSAPI_FUNC(my_sendDDE)
{
	jsint mode;
	char *pszDDEServer = "\"\"", *pszTopic = "\"\"", *pszItem = "\"\"", *pszData = "\"\"";

	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "issss", &mode, &pszDDEServer, &pszTopic, &pszItem, &pszData))
		return JS_FALSE;

	char buffer[255] = "";
	if(SendDDE(mode, pszDDEServer, pszTopic, pszItem, pszData, (char**)&buffer, 255))
	{
		if(mode == 0)
			JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, buffer)));
	}
	else
		THROW_ERROR(cx, "DDE Failed! Check the log for the error message.");

	return JS_TRUE;
}

JSAPI_FUNC(my_keystate)
{
	if(argc < 1 || !JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
		return JS_TRUE;

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(!!GetAsyncKeyState(JSVAL_TO_INT(JS_ARGV(cx, vp)[0]))));
	
	return JS_TRUE;
}

JSAPI_FUNC(my_addEventListener)
{
	if(JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]) && JSVAL_IS_FUNCTION(cx, JS_ARGV(cx, vp)[1]))
	{
		char* evtName = JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
		if(evtName && strlen(evtName))
		{
			Script* self = (Script*)JS_GetContextPrivate(cx);
			self->RegisterEvent(JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0])), JS_ARGV(cx, vp)[1]);
		}
		else
			THROW_ERROR(cx, "Event name is invalid!");
	}
	return JS_TRUE;
}

JSAPI_FUNC(my_removeEventListener)
{
	if(JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]) && JSVAL_IS_FUNCTION(cx, JS_ARGV(cx, vp)[1]))
	{
		char* evtName = JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
		if(evtName && strlen(evtName))
		{
			Script* self = (Script*)JS_GetContextPrivate(cx);
			self->UnregisterEvent(evtName, JS_ARGV(cx, vp)[1]);
		}
		else
			THROW_ERROR(cx, "Event name is invalid!");
	}
	return JS_TRUE;
}

JSAPI_FUNC(my_clearEvent)
{
	if(JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
	{
		Script* self = (Script*)JS_GetContextPrivate(cx);
		self->ClearEvent(JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0])));
	}
	return JS_TRUE;
}

JSAPI_FUNC(my_clearAllEvents)
{
	Script* self = (Script*)JS_GetContextPrivate(cx);
	self->ClearAllEvents();
	return JS_TRUE;
}

JSAPI_FUNC(my_js_strict)
{
	if(argc == NULL)
	{
		JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(((JS_GetOptions(cx) & JSOPTION_STRICT) == JSOPTION_STRICT)));
		return JS_TRUE;
	}

	if(argc == 1)
	{
		bool bFlag = ((JS_GetOptions(cx) & JSOPTION_STRICT) == JSOPTION_STRICT);
		if(JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[0]))
		{
			if(!bFlag)
				JS_ToggleOptions(cx, JSOPTION_STRICT);
		}
		else
		{
			if(bFlag)
				JS_ToggleOptions(cx, JSOPTION_STRICT);
		}
	}

	return JS_TRUE;
}

JSAPI_FUNC(my_scriptBroadcast)
{
	if(argc < 1)
		THROW_ERROR(cx, "You must specify something to broadcast");

	ScriptBroadcastEvent(argc, JS_ARGV(cx, vp));
	return JS_TRUE;
}

JSAPI_FUNC(my_showConsole)
{
	Console::Show();
	return JS_TRUE;
}

JSAPI_FUNC(my_hideConsole)
{
	Console::Hide();
	return JS_TRUE;
}

JSAPI_FUNC(my_loadMpq)
{
	char* path = NULL;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "s", &path))
		return JS_FALSE;

	if(isValidPath(path))
		LoadMPQ(path);

	return JS_TRUE;
}

JSAPI_FUNC(my_sendPacket)
{

	if(!Vars.bEnableUnsupported)
	{
		THROW_WARNING(cx, "sendPacket requires EnableUnsupported = true in d2bs.ini");
		return JS_TRUE;
	}
	
	BYTE* aPacket = new BYTE[20];
	BYTE* pPacket = aPacket;
	uint type = 1;
	for(uint i = 0; i < argc; i++){
		if(i%2 == 0){ 
			JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[i], (uint32*)&type); ++i;
		}
		JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[i], (uint32*)aPacket);
		aPacket += type; 
	}
	D2NET_SendPacket(aPacket - pPacket, 1, pPacket);
	delete[] aPacket;
	JS_SET_RVAL(cx, vp, JSVAL_TRUE);
	return JS_TRUE;
}

JSAPI_FUNC(my_getPacket)
{

	if(!Vars.bEnableUnsupported)
	{
		THROW_WARNING(cx, "getPacket requires EnableUnsupported = true in d2bs.ini");
		return JS_TRUE;
	}
	
	BYTE* aPacket = new BYTE[20];
	BYTE* pPacket = aPacket;
	uint type = 1;
	for(uint i = 0; i < argc; i++){
		if(i%2 == 0){ 
			JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[i], (uint32*)&type); ++i;
		}
		JS_ValueToECMAUint32(cx, JS_ARGV(cx, vp)[i], (uint32*)aPacket);
		aPacket += type; 
	}
	D2NET_ReceivePacket(pPacket, aPacket - pPacket);
	delete[] aPacket;
	JS_SET_RVAL(cx, vp, JSVAL_TRUE);
	return JS_TRUE;
}