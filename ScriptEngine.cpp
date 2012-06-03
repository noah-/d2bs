#include <algorithm>
#include <vector>
#include <assert.h>

#include "ScriptEngine.h"
#include "Core.h"
#include "JSGlobalFuncs.h"
#include "JSGlobalClasses.h"
#include "JSUnit.h"
#include "Constants.h"
#include "D2BS.h"
#include "Console.h"
#include "D2Ptrs.h"
#include "Events.h"

using namespace std;
Script* ScriptEngine::console = NULL;
JSRuntime* ScriptEngine::runtime = NULL;
ScriptMap ScriptEngine::scripts = ScriptMap();
EngineState ScriptEngine::state = Stopped;
CRITICAL_SECTION ScriptEngine::lock = {0};
JSContext* ScriptEngine::context = NULL;
// internal ForEachScript helpers
bool __fastcall DisposeScript(Script* script, void*, uint);
bool __fastcall StopScript(Script* script, void* argv, uint argc);
bool __fastcall GCPauseScript(Script* script, void* argv, uint argc);

Script* ScriptEngine::CompileFile(const char* file, ScriptState state, uintN argc, jsval* argv, bool recompile)
{
	if(GetState() != Running)
		return NULL;
	char* fileName = _strdup(file);
	_strlwr_s(fileName, strlen(file)+1);
	try
	{
		EnterCriticalSection(&lock);
		if(!Vars.bDisableCache)
		{
			if(recompile && scripts.count(fileName) > 0)
			{
				scripts[fileName]->Stop(true, true);
				DisposeScript(scripts[fileName]);
			}
			else if(scripts.count(fileName) > 0)
			{
				Script* ret = scripts[fileName];
				ret->Stop(true, true);
				delete[] fileName;
				LeaveCriticalSection(&lock);
				return ret;
			}
		}
		Script* script = new Script(fileName, state, argc, argv);
		scripts[fileName] = script;
		LeaveCriticalSection(&lock);
		delete[] fileName;
		return script;
	}
	catch(std::exception e)
	{
		LeaveCriticalSection(&lock);
		Print(const_cast<char*>(e.what()));
		delete[] fileName;
		return NULL;
	}
}

void ScriptEngine::RunCommand(const char* command)
{
	if(GetState() != Running)
		return;
	try
	{
		EnterCriticalSection(&lock);
		console->RunCommand(command);
		LeaveCriticalSection(&lock);
	}
	catch(std::exception e)
	{
		LeaveCriticalSection(&lock);
		Print(const_cast<char*>(e.what()));
	}
}

void ScriptEngine::DisposeScript(Script* script)
{
	EnterCriticalSection(&lock);

	if(scripts.count(script->GetFilename()))
		scripts.erase(script->GetFilename());
	delete script;

	LeaveCriticalSection(&lock);
}

unsigned int ScriptEngine::GetCount(bool active, bool unexecuted)
{
	if(GetState() != Running)
		return 0;

	EnterCriticalSection(&lock);
	int count = scripts.size();
	for(ScriptMap::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!active && it->second->IsRunning() && !it->second->IsAborted())
			count--;
		if(!unexecuted && it->second->GetExecutionCount() == 0 && !it->second->IsRunning())
			count--;
	}
	assert(count >= 0);
	LeaveCriticalSection(&lock);
	return count;
}

BOOL ScriptEngine::Startup(void)
{
	if(GetState() == Stopped)
	{
		state = Starting;
		InitializeCriticalSection(&lock);
		EnterCriticalSection(&lock);
		// set UTF-8 to enabled--currently not supported, need to recompile spidermonkey
		//JS_SetCStringsAreUTF8();
		// create the runtime with the requested memory limit
		runtime = JS_NewRuntime(Vars.dwMemUsage*10);   //1.8 hack!!
		if(!runtime)
		{
			LeaveCriticalSection(&lock);
			return FALSE;
		}
		JS_SetContextCallback(runtime, contextCallback);
		// create a context for internal use before we set the callback
		context = JS_NewContext(runtime, 8192);
		JS_SetErrorReporter(context, reportError);
		JS_SetOperationCallback(context, branchCallback);
		JS_SetOptions(context, JSOPTION_STRICT|JSOPTION_VAROBJFIX|JSOPTION_XML);
		JS_SetVersion(context, JSVERSION_LATEST);

		
		JS_SetGCCallbackRT(runtime, gcCallback);

		console = new Script("", Command);
		state = Running;
		LeaveCriticalSection(&lock);
	}
	return TRUE;
}

void ScriptEngine::Shutdown(void)
{
	if(GetState() == Running)
	{
		// bring the engine down properly
		EnterCriticalSection(&lock);
		state = Stopping;
		StopAll(true);
		console->Stop(true, true);

		// clear all scripts now that they're stopped
		ForEachScript(::DisposeScript, NULL, 0);

		if(!scripts.empty())
			scripts.clear();

		if(runtime)
		{
			JS_DestroyContextNoGC(context);
			JS_DestroyRuntime(runtime);
			JS_ShutDown();
			runtime = NULL;
		}

		LeaveCriticalSection(&lock);
		DeleteCriticalSection(&lock);
		state = Stopped;
	}
}

void ScriptEngine::StopAll(bool forceStop)
{
	if(GetState() != Running)
		return;

	EnterCriticalSection(&lock);

	ForEachScript(StopScript, &forceStop, 1);

	LeaveCriticalSection(&lock);
}

void ScriptEngine::UpdateConsole()
{
	console->UpdatePlayerGid();
}
void ScriptEngine::FlushCache(void)
{
	if(GetState() != Running)
		return;

	static bool isFlushing = false;

	if(isFlushing || Vars.bDisableCache)
		return;

	EnterCriticalSection(&lock);
	// TODO: examine if this lock is necessary any more
	EnterCriticalSection(&Vars.cFlushCacheSection);

	isFlushing = true;

	ForEachScript(::DisposeScript, NULL, 0);

	isFlushing = false;

	LeaveCriticalSection(&Vars.cFlushCacheSection);
	LeaveCriticalSection(&lock);
}

bool ScriptEngine::ForEachScript(ScriptCallback callback, void* argv, uint argc)
{
	if(callback == NULL || scripts.size() < 1)
		return false;
	bool block = false;
	EnterCriticalSection(&lock);

	// damn std::list not supporting operator[]...
	std::vector<Script*> list;
	for(ScriptMap::iterator it = scripts.begin(); it != scripts.end(); it++)
		list.push_back(it->second);
		LeaveCriticalSection(&lock);
	int count = list.size();
	// damn std::iterator not supporting manipulating the list...
	for(int i = 0; i < count; i++)
	{
		if(callback(list[i], argv, argc))
			block = true;
	}
	
	
	return block;
}

//void ScriptEngine::ExecEventAsync(char* evtName, AutoRoot** argv, uintN argc)
//{
//	if(GetState() != Running)
//		return;
//
//	EventHelper helper = {evtName, argv, argc};
//	ForEachScript(ExecEventOnScript, &helper, 1);
//}

void ScriptEngine::InitClass(JSContext* context, JSObject* globalObject, JSClass* classp,
							 JSFunctionSpec* methods, JSPropertySpec* props,
							 JSFunctionSpec* s_methods, JSPropertySpec* s_props)
{
	if(!JS_InitClass(context, globalObject, NULL, classp, classp->construct, 0, props, methods, s_props, s_methods))
		throw std::exception("Couldn't initialize the class");
}

void ScriptEngine::DefineConstant(JSContext* context, JSObject* globalObject, const char* name, int value)
{
	if(!JS_DefineProperty(context, globalObject, name, INT_TO_JSVAL(value), NULL, NULL, JSPROP_PERMANENT_VAR))
		throw std::exception("Couldn't initialize the constant");
}

// internal ForEachScript helper functions
bool __fastcall DisposeScript(Script* script, void*, uint)
{
	ScriptEngine::DisposeScript(script);
	return true;
}

bool __fastcall StopScript(Script* script, void* argv, uint argc)
{
	JS_TriggerOperationCallback(script->GetContext());
	script->Stop(*(bool*)(argv), ScriptEngine::GetState() == Stopping);
	return true;
}

bool __fastcall StopIngameScript(Script* script, void*, uint)
{
	if(script->GetState() == InGame)
		script->Stop(true);
	return true;
}

//bool __fastcall ExecEventOnScript(Script* script, void* argv, uint argc)
//{
//	EventHelper* helper = (EventHelper*)argv;
//	
//	if(script->IsListenerRegistered(helper->evtName))
//		script->ExecEventAsync(helper->evtName, helper->argc, helper->argv);
//	return true;
//}

JSBool branchCallback(JSContext* cx)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);
	static int callBackCount = 0;
	callBackCount ++;
	if (callBackCount % 3000 == 0){
		
		
		callBackCount = 0;
	}
	JS_MaybeGC(cx);
	jsrefcount depth = JS_SuspendRequest(cx);
	
	bool pause = script->IsPaused();

	if(pause)
		script->SetPauseState(true);
	while(script->IsPaused())
	{
		Sleep(50);
		JS_MaybeGC(cx);
	}
	if(pause)
		script->SetPauseState(false);

	JS_ResumeRequest(cx, depth);
	
	if(!!!(JSBool)(script->IsAborted() || ((script->GetState() == InGame) && ClientState() == ClientStateMenu)))
	{
		while(script->EventList.size() > 0 && !!!(JSBool)(script->IsAborted() || ((script->GetState() == InGame) && ClientState() == ClientStateMenu)))
		{
			EnterCriticalSection(&Vars.cEventSection);
				Event* evt = script->EventList.back();
				script->EventList.pop_back();
			LeaveCriticalSection(&Vars.cEventSection);
			ExecScriptEvent(evt,false);
		}
		return !!!(JSBool)(script->IsAborted() || ((script->GetState() == InGame) && ClientState() == ClientStateMenu));
	} else {
		while(script->EventList.size() > 0)
		{
			EnterCriticalSection(&Vars.cEventSection);
				Event* evt = script->EventList.back();
				script->EventList.pop_back();
			LeaveCriticalSection(&Vars.cEventSection);
			ExecScriptEvent(evt,true); //clean list and pop events
		}
		script->ClearAllEvents();
		Genhook::Clean(script);
		//JS_DestroyContext(cx);
		return false;
	}
	
}

JSBool contextCallback(JSContext* cx, uintN contextOp)
{
	if(contextOp == JSCONTEXT_NEW)
	{
		JS_SetContextThread(cx);
		JS_BeginRequest(cx);

		JS_SetErrorReporter(cx, reportError);
		JS_SetOperationCallback(cx, branchCallback);
		
		JS_SetVersion(cx, JSVERSION_LATEST);
		JS_SetOptions(cx, JSOPTION_JIT|
					  JSOPTION_METHODJIT|
					  JSOPTION_RELIMIT|
					  JSOPTION_VAROBJFIX|
					  JSOPTION_XML|
					  JSOPTION_STRICT);

		
	JSObject* globalObject = JS_NewCompartmentAndGlobalObject(cx, &global_obj, NULL);
	JS_AddObjectRoot(cx, &globalObject);
		
		
		if(JS_InitStandardClasses(cx, globalObject) == JS_FALSE)
			return JS_FALSE;
		if(JS_DefineFunctions(cx, globalObject, global_funcs) == JS_FALSE)
			return JS_FALSE;

		myUnit* lpUnit = new myUnit;
		memset(lpUnit, NULL, sizeof(myUnit));

		UnitAny* player = D2CLIENT_GetPlayerUnit();
		lpUnit->dwMode = (DWORD)-1;
		lpUnit->dwClassId = (DWORD)-1;
		lpUnit->dwType = UNIT_PLAYER;
		lpUnit->dwUnitId = player ? player->dwUnitId : NULL;
		lpUnit->_dwPrivateType = PRIVATE_UNIT;

		for(JSClassSpec* entry = global_classes; entry->classp != NULL; entry++)
			ScriptEngine::InitClass(cx, globalObject, entry->classp, entry->methods, entry->properties,
										entry->static_methods, entry->static_properties);

		JSObject* meObject = BuildObject(cx, &unit_class, unit_methods, me_props, lpUnit);
		if(!meObject)
			return JS_FALSE;

		if(JS_DefineProperty(cx, globalObject, "me", OBJECT_TO_JSVAL(meObject), NULL, NULL, JSPROP_PERMANENT_VAR) == JS_FALSE)
			return JS_FALSE;

#define DEFCONST(vp) ScriptEngine::DefineConstant(cx, globalObject, #vp, vp)
		DEFCONST(FILE_READ);
		DEFCONST(FILE_WRITE);
		DEFCONST(FILE_APPEND);
#undef DEFCONST

		JS_EndRequest(cx);		
		
	}
	if(contextOp == JSCONTEXT_DESTROY)
	{
		Script* script = (Script*)JS_GetContextPrivate(cx);
		
		//ScriptEngine::DisposeScript(script);
		JS_GC(cx);
	}

	return JS_TRUE;
}

JSBool gcCallback(JSContext *cx, JSGCStatus status)
{
	if(status == JSGC_BEGIN)
	{
//		EnterCriticalSection(&ScriptEngine::lock);
		Print("*** ENTERING GC ***");
#ifdef DEBUG
		Log("*** ENTERING GC ***");
#ifdef lord2800_INFO
		Print("*** ENTERING GC ***");
#endif
#endif
	}
	else if(status == JSGC_END)
	{
		Print("*** LEAVING GC ***");
#ifdef DEBUG
		Log("*** LEAVING GC ***");
#ifdef lord2800_INFO
		Print("*** LEAVING GC ***");
#endif
#endif
//		LeaveCriticalSection(&ScriptEngine::lock);
	}
	return JS_TRUE;
}

void reportError(JSContext *cx, const char *message, JSErrorReport *report)
{
	bool warn = JSREPORT_IS_WARNING(report->flags);
	bool isStrict = JSREPORT_IS_STRICT(report->flags);
	const char* type = (warn ? "Warning" : "Error");
	const char* strict = (isStrict ? "Strict " : "");
	char* filename = report->filename ? _strdup(report->filename) : _strdup("<unknown>");
	char* displayName = filename;
	if(_stricmp("Command Line", filename) != 0 && _stricmp("<unknown>", filename) != 0)
		displayName = filename + strlen(Vars.szPath);

	Log("[%s%s] Code(%d) File(%s:%d) %s\nLine: %s", 
			strict, type, report->errorNumber, filename, report->lineno, message, report->linebuf);

	Print("[ÿc%d%s%sÿc0 (%d)] File(%s:%d) %s", 
			(warn ? 9 : 1), strict, type, report->errorNumber, displayName, report->lineno, message);

	free(filename);

	if(Vars.bQuitOnError && !JSREPORT_IS_WARNING(report->flags) && ClientState() == ClientStateInGame)
		D2CLIENT_ExitGame();
	else
		Console::ShowBuffer();
}
bool ExecScriptEvent(Event* evt, bool clearList)
{
	JSContext* cx;
	if (!clearList)
		cx = evt->owner->GetContext();
	char* evtName = (char*) evt->name;
	if (strcmp(evtName,"itemaction") == 0){
		if(!clearList)
		{
			DWORD* gid = (DWORD*) evt->arg1;
			char* code =  (char*) evt->arg2;		
			DWORD* mode = (DWORD*) evt->arg3;
			bool* global = (bool*) evt->arg4;
		
			jsval* argv = new jsval[evt->argc];
			JS_BeginRequest(cx);
		
			JS_NewNumberValue(cx, *gid, &argv[0]);
			JS_NewNumberValue(cx, *mode, &argv[1]);
			argv[2] = (STRING_TO_JSVAL(JS_NewStringCopyZ(cx, code)));
			argv[3] = (BOOLEAN_TO_JSVAL(*global));
			for(int j = 0 ; j < evt->argc; j++)
				JS_AddRoot(cx, &argv[j]);
		
			jsval rval;
			bool block;
			for(FunctionList::iterator it = evt->owner->functions[evtName].begin(); it != evt->owner->functions[evtName].end(); it++)
			{
				JS_CallFunctionValue(cx, JS_GetGlobalObject(cx), *(*it)->value(), 4, argv, &rval);
				block |= (JSVAL_IS_BOOLEAN(rval) && JSVAL_TO_BOOLEAN(rval));
			}
			JS_EndRequest(cx);
			for(int j = 0 ; j < evt->argc; j++)
				JS_RemoveRoot(cx, &argv[j]);
		}
		delete evt->arg1;
		delete evt->arg2;
		delete evt->arg3;
		delete evt->arg4;
		delete evt;
		return true;		
	}
	if (strcmp(evtName, "gameevent") == 0){
		if(!clearList)
		{
			jsval* argv = new jsval[evt->argc];
			JS_BeginRequest(cx);		
				JS_NewNumberValue(cx, *(BYTE*) evt->arg1, &argv[0]);
				JS_NewNumberValue(cx, *(DWORD*) evt->arg2, &argv[1]);
				JS_NewNumberValue(cx, *(DWORD*) evt->arg3, &argv[2]);
				argv[3] = (STRING_TO_JSVAL(JS_NewStringCopyZ(cx, (char*) evt->arg4)));
				argv[4] = (STRING_TO_JSVAL(JS_NewStringCopyZ(cx, (char*) evt->arg5)));
		
				for(int j = 0 ; j < evt->argc; j++)
					JS_AddRoot(cx, &argv[j]);
		
				jsval rval;			
				for(FunctionList::iterator it = evt->owner->functions[evtName].begin(); it != evt->owner->functions[evtName].end(); it++)
				{
					JS_CallFunctionValue(cx, JS_GetGlobalObject(cx), *(*it)->value(), 4, argv, &rval);				
				}
			JS_EndRequest(cx);
			for(int j = 0 ; j < evt->argc; j++)
				JS_RemoveRoot(cx, &argv[j]);
		}
		delete evt->arg1;
		delete evt->arg2;
		delete evt->arg3;
		delete evt->arg4;
		delete evt->arg5;
		delete evt;
		return true;	
	}
	if (strcmp(evtName, "copydata") == 0){
		if(!clearList)
		{
			jsval* argv = new jsval[evt->argc];
			JS_BeginRequest(cx);		
				JS_NewNumberValue(cx, *(DWORD*) evt->arg1, &argv[0]);
				argv[1] = (STRING_TO_JSVAL(JS_NewStringCopyZ(cx, (char*) evt->arg2)));
		
				for(int j = 0 ; j < evt->argc; j++)
					JS_AddRoot(cx, &argv[j]);
		
				jsval rval;			
				for(FunctionList::iterator it = evt->owner->functions[evtName].begin(); it != evt->owner->functions[evtName].end(); it++)
				{
					JS_CallFunctionValue(cx, JS_GetGlobalObject(cx), *(*it)->value(), 4, argv, &rval);				
				}
			JS_EndRequest(cx);
			for(int j = 0 ; j < evt->argc; j++)
				JS_RemoveRoot(cx, &argv[j]);
		}
		delete evt->arg1;
		delete evt->arg2;
		delete evt;
		return true;	
	}
	if (strcmp(evtName, "chatmsg") == 0 || strcmp(evtName, "whispermsg") == 0 || strcmp(evtName, "chatmsgblocker") == 0 || strcmp(evtName, "whispermsgblocker") == 0){
		bool block = false;
		if(!clearList)
		{
			jsval* argv = new jsval[evt->argc];
			JS_BeginRequest(cx);		
				argv[0] = (STRING_TO_JSVAL(JS_NewStringCopyZ(cx, (char*) evt->arg1)));
				argv[1] = (STRING_TO_JSVAL(JS_NewStringCopyZ(cx, (char*) evt->arg2)));
		
				for(int j = 0 ; j < evt->argc; j++)
					JS_AddRoot(cx, &argv[j]);
		
				jsval rval;			
				for(FunctionList::iterator it = evt->owner->functions[evtName].begin(); it != evt->owner->functions[evtName].end(); it++)
				{
					JS_CallFunctionValue(cx, JS_GetGlobalObject(cx), *(*it)->value(), 4, argv, &rval);	
					block |= (JSVAL_IS_BOOLEAN(rval) && JSVAL_TO_BOOLEAN(rval));
				}
			JS_EndRequest(cx);
			for(int j = 0 ; j < evt->argc; j++)
				JS_RemoveRoot(cx, &argv[j]);
		}
		if (strcmp(evtName, "keydownblocker") == 0 ){
			evt->arg4 =  new bool(block);
			SetEvent(evt->arg5);
		}else{
			delete evt->arg1;
			delete evt->arg2;
			delete evt->name;
			delete evt;
		}
		return true;	
	}
	if (strcmp(evtName, "mousemove") == 0) {
		if(!clearList)
		{
			jsval* argv = new jsval[evt->argc];
			JS_BeginRequest(cx);		
				JS_NewNumberValue(cx, *(DWORD*) evt->arg1, &argv[0]);
				JS_NewNumberValue(cx, *(DWORD*) evt->arg2, &argv[1]);
		
				for(int j = 0 ; j < evt->argc; j++)
					JS_AddRoot(cx, &argv[j]);
		
				jsval rval;			
				for(FunctionList::iterator it = evt->owner->functions[evtName].begin(); it != evt->owner->functions[evtName].end(); it++)
				{
					JS_CallFunctionValue(cx, JS_GetGlobalObject(cx), *(*it)->value(), 4, argv, &rval);	
				}
			JS_EndRequest(cx);
			for(int j = 0 ; j < evt->argc; j++)
				JS_RemoveRoot(cx, &argv[j]);
		}		
		delete evt->arg1;
		delete evt->arg2;		
		delete evt;		
		return true;	
	}
	if (strcmp(evtName, "mouseclick") == 0) {
		if(!clearList)
		{
			jsval* argv = new jsval[evt->argc];
			JS_BeginRequest(cx);		
				JS_NewNumberValue(cx, *(DWORD*) evt->arg1, &argv[0]);
				JS_NewNumberValue(cx, *(DWORD*) evt->arg2, &argv[1]);
				JS_NewNumberValue(cx, *(DWORD*) evt->arg3, &argv[2]);
				JS_NewNumberValue(cx, *(DWORD*) evt->arg4, &argv[3]);

				for(int j = 0 ; j < evt->argc; j++)
					JS_AddRoot(cx, &argv[j]);
		
				jsval rval;			
				for(FunctionList::iterator it = evt->owner->functions[evtName].begin(); it != evt->owner->functions[evtName].end(); it++)
				{
					JS_CallFunctionValue(cx, JS_GetGlobalObject(cx), *(*it)->value(), 4, argv, &rval);	
				}
			JS_EndRequest(cx);
			for(int j = 0 ; j < evt->argc; j++)
				JS_RemoveRoot(cx, &argv[j]);
		}
		delete evt->arg1;
		delete evt->arg2;
		delete evt->arg3;
		delete evt->arg4;
		delete evt;
		return true;	
	}
	if (strcmp(evtName, "keyup") == 0 || strcmp(evtName, "keydownblocker") == 0  || strcmp(evtName, "keydown") == 0 || strcmp(evtName, "memana") == 0 || strcmp(evtName, "melife") == 0) {
		bool block = false;
		if(!clearList)
		{
			jsval* argv = new jsval[evt->argc];			
			JS_BeginRequest(cx);		
				JS_NewNumberValue(cx, *(DWORD*) evt->arg1, &argv[0]);
					
				for(int j = 0 ; j < evt->argc; j++)
					JS_AddRoot(cx, &argv[j]);
		
				jsval rval;			
				for(FunctionList::iterator it = evt->owner->functions[evtName].begin(); it != evt->owner->functions[evtName].end(); it++)
				{
					JS_CallFunctionValue(cx, JS_GetGlobalObject(cx), *(*it)->value(), 4, argv, &rval);	
					block |= (JSVAL_IS_BOOLEAN(rval) && JSVAL_TO_BOOLEAN(rval));
				}
			JS_EndRequest(cx);
			for(int j = 0 ; j < evt->argc; j++)
				JS_RemoveRoot(cx, &argv[j]);
		}
		if (strcmp(evtName, "keydownblocker") == 0 ){
			evt->arg4 =  new bool(block);
			SetEvent(evt->arg5);
		}else{
			delete evt->arg1;
			delete evt->name;
			delete evt;
		}
		return true;	
	}
	if (strcmp(evtName, "ScreenHookClick") == 0 || strcmp(evtName, "keydownblocker") == 0  || strcmp(evtName, "keydown") == 0 || strcmp(evtName, "memana") == 0 || strcmp(evtName, "melife") == 0) {
		bool block = false;
		if(!clearList)
		{
			jsval* argv = new jsval[evt->argc];			
			JS_BeginRequest(cx);		
				JS_NewNumberValue(cx, *(DWORD*) evt->arg1, &argv[0]);
				JS_NewNumberValue(cx, *(DWORD*) evt->arg2, &argv[1]);
				JS_NewNumberValue(cx, *(DWORD*) evt->arg3, &argv[2]);
				for(int j = 0 ; j < evt->argc; j++)
					JS_AddRoot(cx, &argv[j]);
		
				jsval rval;			
				for(FunctionList::iterator it = evt->owner->functions[evtName].begin(); it != evt->owner->functions[evtName].end(); it++)
				{
					JS_CallFunctionValue(cx, JS_GetGlobalObject(cx), *(*it)->value(), 4, argv, &rval);	
					block |= (JSVAL_IS_BOOLEAN(rval) && JSVAL_TO_BOOLEAN(rval));
				}
			JS_EndRequest(cx);
			for(int j = 0 ; j < evt->argc; j++)
				JS_RemoveRoot(cx, &argv[j]);
		}
		evt->arg4 =  new bool(block);
		SetEvent(evt->arg5);
		
		return true;	
	}
	if (strcmp(evtName, "Command") == 0){

		char* cmd =  (char*) evt->arg1;	
		JS_BeginRequest(cx);
		jsval rval;
		if(JS_EvaluateScript(cx, JS_GetGlobalObject(cx), cmd, strlen(cmd), "Command Line", 0, &rval))
		{
			if(!JSVAL_IS_NULL(rval) && !JSVAL_IS_VOID(rval))
			{
				JS_ConvertValue(cx, rval, JSTYPE_STRING, &rval);
				char* text =_strdup( JS_EncodeString(cx,JS_ValueToString(cx, rval)));
				std::replace(text, text + strlen(text), '%', (char)(unsigned char)0xFE);
				Print(text);
				//free(text);
			}
		}
		JS_EndRequest(cx);
		//delete evt->arg1;
		//delete evt;
	}
		
	
}