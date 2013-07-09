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
CRITICAL_SECTION ScriptEngine::scriptListLock = {0};
std::list<Event*> ScriptEngine::DelayedExecList ;
int ScriptEngine::delayedExecKey;
JSContext* ScriptEngine::context = NULL;
// internal ForEachScript helpers
bool __fastcall DisposeScript(Script* script, void*, uint);
bool __fastcall StopScript(Script* script, void* argv, uint argc);


Script* ScriptEngine::CompileFile(const char* file, ScriptState state, uintN argc, jsval* argv, bool recompile)
{
	if(GetState() != Running)
		return NULL;
	char* fileName = _strdup(file);
	_strlwr_s(fileName, strlen(file)+1);
	try
	{
		Script* script = new Script(fileName, state, argc, argv);
		scripts[fileName] = script;
		free(fileName);
		return script;
	}
	catch(std::exception e)
	{
		LeaveCriticalSection(&lock);
		Print(const_cast<char*>(e.what()));
		free(fileName);
		return NULL;
	}
}

void ScriptEngine::RunCommand(const char* command)
{
	if(GetState() != Running)
		return;
	try
	{
		//EnterCriticalSection(&lock);
		console->RunCommand(command);
		//LeaveCriticalSection(&lock);
	}
	catch(std::exception e)
	{
		//LeaveCriticalSection(&lock);
		Print(const_cast<char*>(e.what()));
	}
}

void ScriptEngine::DisposeScript(Script* script)
{
	if(scripts.count(script->GetFilename()))
		scripts.erase(script->GetFilename());
	if(GetCurrentThreadId() == script->threadId)
		delete script;
	else
	{	
	  //bad things happen if we delete from another thread
		Event* evt = new Event;
		evt->owner = script;
		evt->name = strdup("DisposeMe");
		script->FireEvent(evt);
	}
}
void ScriptEngine::LockScriptList(char* loc)
{
	EnterCriticalSection(&scriptListLock);
	//Log(loc);
}
void ScriptEngine::UnLockScriptList(char* loc)
{
	//Log(loc);
	LeaveCriticalSection(&scriptListLock);
}
unsigned int ScriptEngine::GetCount(bool active, bool unexecuted)
{
	if(GetState() != Running)
		return 0;
	LockScriptList("getCount");
	//EnterCriticalSection(&lock);
	int count = scripts.size();
	for(ScriptMap::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!active && it->second->IsRunning() && !it->second->IsAborted())
			count--;
		if(!unexecuted && it->second->GetExecutionCount() == 0 && !it->second->IsRunning())
			count--;
	}
	assert(count >= 0);
	UnLockScriptList("getCount");
	//LeaveCriticalSection(&lock);
	return count;
}

BOOL ScriptEngine::Startup(void)
{
	if(GetState() == Stopped)
	{
		state = Starting;
		InitializeCriticalSection(&scriptListLock);
		//InitializeCriticalSection(&lock);
		//EnterCriticalSection(&lock);
		LockScriptList("startup - enter");

		console = new Script("", Command);
		scripts["console"] = console;
		console->BeginThread(ScriptThread);
		state = Running;
		//LeaveCriticalSection(&lock);
		UnLockScriptList("startup - leave");		
	}
	return TRUE;
}

void ScriptEngine::Shutdown(void)
{
	if(GetState() == Running)
	{
		// bring the engine down properly
		//EnterCriticalSection(&lock);
		LockScriptList("Shutdown");
		state = Stopping;
		StopAll(true);
		console->Stop(true, true);

		// clear all scripts now that they're stopped
		ForEachScript(::DisposeScript, NULL, 0);

		if(!scripts.empty())
			scripts.clear();

		if(runtime)
		{
			JS_DestroyContext(context);
			JS_DestroyRuntime(runtime);
			JS_ShutDown();
			runtime = NULL;
		}
		UnLockScriptList("shutdown");
		//LeaveCriticalSection(&lock);
		DeleteCriticalSection(&lock);
		state = Stopped;
	}
}

void ScriptEngine::StopAll(bool forceStop)
{
	if(GetState() != Running)
		return;
	
	//EnterCriticalSection(&lock);
	ForEachScript(StopScript, &forceStop, 1);

	//LeaveCriticalSection(&lock);
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

	//EnterCriticalSection(&lock);
	// TODO: examine if this lock is necessary any more
	EnterCriticalSection(&Vars.cFlushCacheSection);

	isFlushing = true;

	ForEachScript(::DisposeScript, NULL, 0);

	isFlushing = false;

	LeaveCriticalSection(&Vars.cFlushCacheSection);
	//LeaveCriticalSection(&lock);
}

bool ScriptEngine::ForEachScript(ScriptCallback callback, void* argv, uint argc)
{
	if(callback == NULL || scripts.size() < 1)
		return false;
	bool block = false;
	//EnterCriticalSection(&lock);
	
	LockScriptList("forEachScript");

	// damn std::list not supporting operator[]...
	std::vector<Script*> list;
	for(ScriptMap::iterator it = scripts.begin(); it != scripts.end(); it++)
		list.push_back(it->second);
	
	UnLockScriptList("forEachScript");

	int count = list.size();
	// damn std::iterator not supporting manipulating the list...
	for(int i = 0; i < count; i++)
	{
		if(callback(list[i], argv, argc))
			block = true;
	}
	//LeaveCriticalSection(&lock);  // was previously locked after callback calls.  

	
	return block;
}

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
	script->TriggerOperationCallback();
	if(script->GetState() != Command)
	script->Stop(*(bool*)(argv), ScriptEngine::GetState() == Stopping);
	return true;
}

bool __fastcall StopIngameScript(Script* script, void*, uint)
{
	if(script->GetState() == InGame)
		script->Stop(true);
	return true;
}

JSBool operationCallback(JSContext* cx)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);
	static int callBackCount = 0;
	callBackCount ++;
	
	//moved this to delay. operation callback isnt a good spot for this anymore 
	//event heavy stuff like shop bot would trigger gc alot while normal playing would not.
	/*if (callBackCount % 30 == 0){		
		JS_GC(JS_GetRuntime(cx));		
		callBackCount = 0;
	}*/
		
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
		return false;
		
	}
	
}

JSBool contextCallback(JSContext* cx, uintN contextOp)
{
	if(contextOp == JSCONTEXT_NEW)
	{
		JS_BeginRequest(cx);

		JS_SetErrorReporter(cx, reportError);
		JS_SetOperationCallback(cx, operationCallback);
		
		JS_SetVersion(cx, JSVERSION_LATEST);
		JS_SetOptions(cx, 
					  JSOPTION_METHODJIT|
					  JSOPTION_TYPE_INFERENCE|
					  JSOPTION_ION|
					  JSOPTION_VAROBJFIX|
					  JSOPTION_ALLOW_XML|
					  JSOPTION_MOAR_XML|
					  JSOPTION_STRICT);

		//JS_SetGCZeal(cx, 2, 1);
	JSObject* globalObject = JS_NewGlobalObject(cx, &global_obj, NULL);
	//JS_SetGCParameter(JS_GetRuntime(cx), JSGC_MODE, JSGC_MODE_INCREMENTAL);
		
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
		script->hasActiveCX = false;
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
	
	}
	return JS_TRUE;
}

//JSGCCallback gcCallback(JSRuntime *rt, JSGCStatus status)
//{
//	//Script* script = (Script*)JS_GetContextPrivate(cx);
//	DWORD cur =GetCurrentThreadId() ;
//	//if (cur != script->GetThreadId())
//	//	return JS_FALSE;
//	if(status == JSGC_BEGIN)
//	{
//		 
//		//Script* script = (Script*)JS_GetContextPrivate(cx);
//		//Print("Entering GC %s",script->GetShortFilename());
////		EnterCriticalSection(&ScriptEngine::lock);
//
//#ifdef DEBUG
//		Log("*** ENTERING GC ***");
//#ifdef lord2800_INFO
//		Print("*** ENTERING GC ***");
//#endif
//#endif
//	}
//	else if(status == JSGC_END)
//	{
//	//	Script* script = (Script*)JS_GetContextPrivate(cx);
//	//	Print("Leaving GC %s",script->GetShortFilename());
//#ifdef DEBUG
//		Log("*** LEAVING GC ***");
//#ifdef lord2800_INFO
//		Print("*** LEAVING GC ***");
//#endif
//#endif
////		LeaveCriticalSection(&ScriptEngine::lock);
//	}
//	//return JS_TRUE;
//}

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
		
			argv[0] = JS_NumberValue(*gid);
			argv[1] = JS_NumberValue(*mode);
			argv[2] = (STRING_TO_JSVAL(JS_NewStringCopyZ(cx, code)));
			argv[3] = (BOOLEAN_TO_JSVAL(*global));
			for(int j = 0 ; j < 4; j++)
				JS_AddValueRoot(cx, &argv[j]);		
			jsval rval;
			bool block;			
			for(FunctionList::iterator it = evt->owner->functions[evtName].begin(); it != evt->owner->functions[evtName].end(); it++)
			{
				JS_CallFunctionValue(cx, JS_GetGlobalObject(cx), *(*it)->value(),  4, argv, &rval);
				block |= (JSVAL_IS_BOOLEAN(rval) && JSVAL_TO_BOOLEAN(rval));
			}
			JS_EndRequest(cx);
			for(int j = 0 ; j < 4; j++)
				JS_RemoveValueRoot(cx, &argv[j]);
		}
		delete evt->arg1;
		free (evt->arg2);
		delete evt->arg3;
		delete evt->arg4;
		delete evt;
		return true;		
	}
	if (strcmp(evtName, "gameevent") == 0){
		if(!clearList)
		{
			jsval* argv = new jsval[5];
			JS_BeginRequest(cx);		
				argv[0] = JS_NumberValue(*(BYTE*) evt->arg1);
				argv[1] = JS_NumberValue(*(DWORD*) evt->arg2);
				argv[2] = JS_NumberValue(*(DWORD*) evt->arg3);
				argv[3] = (STRING_TO_JSVAL(JS_NewStringCopyZ(cx, (char*) evt->arg4)));
				argv[4] = (STRING_TO_JSVAL(JS_NewStringCopyZ(cx, (char*) evt->arg5)));
		
				for(int j = 0 ; j < 5; j++)
					JS_AddValueRoot(cx, &argv[j]);
		
				jsval rval;	
				for(FunctionList::iterator it = evt->owner->functions[evtName].begin(); it != evt->owner->functions[evtName].end(); it++)
				{
					JS_CallFunctionValue(cx, JS_GetGlobalObject(cx), *(*it)->value(),  5, argv, &rval);				
				}
			JS_EndRequest(cx);
			for(int j = 0 ; j < 5; j++)
				JS_RemoveValueRoot(cx, &argv[j]);
		}
		delete evt->arg1;
		delete evt->arg2;
		delete evt->arg3;
		free(evt->arg4);
		free(evt->arg5);
		delete evt;
		return true;	
	}
	if (strcmp(evtName, "copydata") == 0){
		if(!clearList)
		{
			jsval* argv = new jsval[2];
			JS_BeginRequest(cx);		
				argv[0] =  JS_NumberValue(*(DWORD*) evt->arg1);
				argv[1] = (STRING_TO_JSVAL(JS_NewStringCopyZ(cx, (char*) evt->arg2)));
		
				for(int j = 0 ; j < 2; j++)
					JS_AddValueRoot(cx, &argv[j]);
		
				jsval rval;		
				for(FunctionList::iterator it = evt->owner->functions[evtName].begin(); it != evt->owner->functions[evtName].end(); it++)
				{
					JS_CallFunctionValue(cx, JS_GetGlobalObject(cx), *(*it)->value(), 2, argv, &rval);				
				}
			JS_EndRequest(cx);
			for(int j = 0 ; j < 2; j++)
				JS_RemoveValueRoot(cx, &argv[j]);
		}
		delete evt->arg1;
		free (evt->arg2);
		delete evt;
		return true;	
	}
	if (strcmp(evtName, "chatmsg") == 0 || strcmp(evtName, "whispermsg") == 0 || strcmp(evtName, "chatmsgblocker") == 0 || strcmp(evtName, "whispermsgblocker") == 0){
		bool block = false;
		if(!clearList)
		{
			jsval* argv = new jsval[2];
			JS_BeginRequest(cx);		
				argv[0] = (STRING_TO_JSVAL(JS_NewStringCopyZ(cx, (char*) evt->arg1)));
				argv[1] = (STRING_TO_JSVAL(JS_NewStringCopyZ(cx, (char*) evt->arg2)));
		
				for(int j = 0 ; j < 2; j++)
					JS_AddValueRoot(cx, &argv[j]);
		
				jsval rval;	
				for(FunctionList::iterator it = evt->owner->functions[evtName].begin(); it != evt->owner->functions[evtName].end(); it++)
				{
					JS_CallFunctionValue(cx, JS_GetGlobalObject(cx), *(*it)->value(),  2, argv, &rval);	
					block |= (JSVAL_IS_BOOLEAN(rval) && JSVAL_TO_BOOLEAN(rval));
				}
			JS_EndRequest(cx);
			for(int j = 0 ; j < 2; j++)
				JS_RemoveValueRoot(cx, &argv[j]);
		}
		if (strcmp(evtName, "chatmsgblocker") == 0 || strcmp(evtName, "whispermsgblocker") == 0){
			evt->arg4 =  new bool(block);
			SetEvent(Vars.eventSignal);
		}else{
			free(evt->arg1);
			free(evt->arg2);
			free(evt->name);
			delete evt;
		}
		return true;	
	}
	if (strcmp(evtName, "mousemove") == 0 || strcmp(evtName, "ScreenHookHover") == 0) {
		if(!clearList)
		{
			jsval* argv = new jsval[2];
			JS_BeginRequest(cx);		
				argv[0] = JS_NumberValue(*(DWORD*) evt->arg1);
				argv[1] = JS_NumberValue(*(DWORD*) evt->arg2);
		
				for(int j = 0 ; j < 2; j++)
					JS_AddValueRoot(cx, &argv[j]);
		
				jsval rval;
				if (strcmp(evtName, "ScreenHookHover") == 0)
				{
					for(FunctionList::iterator it = evt->functions.begin(); it != evt->functions.end(); it++)				
						JS_CallFunctionValue(cx, JS_GetGlobalObject(cx), *(*it)->value(),  evt->argc+1, argv, &rval);					
				}else{
					for(FunctionList::iterator it = evt->owner->functions[evtName].begin(); it != evt->owner->functions[evtName].end(); it++)					
						JS_CallFunctionValue(cx, JS_GetGlobalObject(cx), *(*it)->value(), 2, argv, &rval);	
				}
			JS_EndRequest(cx);
			for(int j = 0 ; j < 2; j++)
				JS_RemoveValueRoot(cx, &argv[j]);
		}		
		delete evt->arg1;
		delete evt->arg2;		
		delete evt;		
		return true;	
	}
	if (strcmp(evtName, "mouseclick") == 0) {
		if(!clearList)
		{
			jsval* argv = new jsval[4];
			JS_BeginRequest(cx);		
				argv[0] = JS_NumberValue(*(DWORD*) evt->arg1);
				argv[1] = JS_NumberValue(*(DWORD*) evt->arg2);
				argv[2] = JS_NumberValue(*(DWORD*) evt->arg3);
				argv[3] = JS_NumberValue(*(DWORD*) evt->arg4);

				for(int j = 0 ; j < evt->argc; j++)
					JS_AddValueRoot(cx, &argv[j]);
		
				jsval rval;	
				for(FunctionList::iterator it = evt->owner->functions[evtName].begin(); it != evt->owner->functions[evtName].end(); it++)
				{
					JS_CallFunctionValue(cx, JS_GetGlobalObject(cx), *(*it)->value(),  4, argv, &rval);	
				}
			JS_EndRequest(cx);
			for(int j = 0 ; j < 4; j++)
				JS_RemoveValueRoot(cx, &argv[j]);
		}
		delete evt->arg1;
		delete evt->arg2;
		delete evt->arg3;
		delete evt->arg4;
		delete evt;
		return true;	
	}
	if (strcmp(evtName, "keyup") == 0 || strcmp(evtName, "keydownblocker") == 0  || strcmp(evtName, "keydown") == 0 || strcmp(evtName, "memana") == 0 || strcmp(evtName, "melife" ) == 0 || strcmp(evtName, "playerassign") == 0) {
		bool block = false;
		if(!clearList)
		{
			jsval* argv = new jsval[1];			
			JS_BeginRequest(cx);		
				argv[0] = JS_NumberValue(*(DWORD*) evt->arg1);
				JS_AddValueRoot(cx, &argv[0]);
				jsval rval;		
				for(FunctionList::iterator it = evt->owner->functions[evtName].begin(); it != evt->owner->functions[evtName].end(); it++)
				{
					JS_CallFunctionValue(cx, JS_GetGlobalObject(cx), *(*it)->value(),  1, argv, &rval);	
					block |= (JSVAL_IS_BOOLEAN(rval) && JSVAL_TO_BOOLEAN(rval));
				}
			JS_EndRequest(cx);
			JS_RemoveValueRoot(cx, &argv[0]);
		}
		if (strcmp(evtName, "keydownblocker") == 0 ){
			evt->arg4 =  new bool(block);
			SetEvent(Vars.eventSignal);
		}else{
			delete evt->arg1;
			free (evt->name);
			delete evt;
		}
		return true;	
	}
	if (strcmp(evtName, "ScreenHookClick") == 0 ) {
		bool block = false;
		if(!clearList)
		{
			jsval* argv = new jsval[3];			
			JS_BeginRequest(cx);		
				argv[0] = JS_NumberValue(*(DWORD*) evt->arg1);
				argv[1] = JS_NumberValue(*(DWORD*) evt->arg2);
				argv[2] = JS_NumberValue(*(DWORD*) evt->arg3);
				for(int j = 0 ; j < 3; j++)
					JS_AddValueRoot(cx, &argv[j]);
		
				jsval rval;
					// diffrent function source for hooks
					for(FunctionList::iterator it = evt->functions.begin(); it != evt->functions.end(); it++)
					{ 
						JS_CallFunctionValue(cx, JS_GetGlobalObject(cx), *(*it)->value(),  3, argv, &rval);	
						block |= (JSVAL_IS_BOOLEAN(rval) && JSVAL_TO_BOOLEAN(rval));
					}
			
			JS_EndRequest(cx);
			for(int j = 0 ; j < 3; j++)
				JS_RemoveValueRoot(cx, &argv[j]);
		}
		evt->arg4 =  new bool(block);
		SetEvent(Vars.eventSignal);
		
		return true;	
	}
	
	if (strcmp(evtName, "Command") == 0){
		
		char* cmd =  (char*) evt->arg1;	
		std::string test;

		test.append( "try{ ");
		test.append( cmd );
		test.append(" } catch (error){print(error)}");
		JS_BeginRequest(cx);
		jsval rval;
		
		if(JS_EvaluateScript(cx, JS_GetGlobalObject(cx), test.data() , test.length(), "Command Line", 0, &rval))
		{
			if(!JSVAL_IS_NULL(rval) && !JSVAL_IS_VOID(rval))
			{
				JS_ConvertValue(cx, rval, JSTYPE_STRING, &rval);
				char* text = JS_EncodeString(cx,JS_ValueToString(cx, rval));
				std::replace(text, text + strlen(text), '%', (char)(unsigned char)0xFE);
				Print(text);
				JS_free(cx, text);
			}
		}
		JS_EndRequest(cx);
		free(evt->arg1);
		delete evt;
	}
	if (strcmp(evtName, "scriptmsg") == 0) {
		if(!clearList)
		{
			DWORD* argc = (DWORD*) evt->arg1;
			JS_BeginRequest(cx);	
			jsval* argv = new jsval[*argc];
			for(uintN i = 0; i < *argc; i++)
				evt->argv[i]->read(cx, &argv[i]);			
				
				for(int j = 0 ; j < *argc; j++)
					JS_AddValueRoot(cx, &argv[j]);
		
				jsval rval;	
				for(FunctionList::iterator it = evt->owner->functions[evtName].begin(); it != evt->owner->functions[evtName].end(); it++)
				{
					JS_CallFunctionValue(cx, JS_GetGlobalObject(cx), *(*it)->value(), *argc, argv, &rval);	
				}
			JS_EndRequest(cx);
			for(int j = 0 ; j < *argc; j++)
				JS_RemoveValueRoot(cx, &argv[j]);
		}
		for(uintN i = 0; i < evt->argc; i++){
				evt->argv[i]->clear();
				delete evt->argv[i];
		}		
		delete evt->arg1;
		delete evt;		
		return true;	
	}	
	if (strcmp(evtName, "gamepacket") == 0) {
		bool block = false;
		if(!clearList)
		{
			BYTE* help = (BYTE*) evt->arg1;
			DWORD* size = (DWORD*) evt->arg2;
			DWORD* argc = (DWORD*) 1;
			JS_BeginRequest(cx);

			jsval* jsarr = new jsval[*size];

			int i = 0;
			for(i=0; i< *size; i++) jsarr[i] = UINT_TO_JSVAL(help[i]);;
			JSObject* arr = JS_NewArrayObject(cx, *size, jsarr);
			
			jsval argv =  OBJECT_TO_JSVAL(arr);
			//evt->argv[0]->read(cx, &argv[0]);			
			//JS_AddValueRoot(cx, &argv[0]);
		
				jsval rval;	
				for(FunctionList::iterator it = evt->owner->functions[evtName].begin(); it != evt->owner->functions[evtName].end(); it++)
				{
					JS_CallFunctionValue(cx, JS_GetGlobalObject(cx), *(*it)->value(), 1, &argv, &rval);	
					block |= (JSVAL_IS_BOOLEAN(rval) && JSVAL_TO_BOOLEAN(rval));
				}
			evt->arg4 =  new DWORD(block);
			
			SetEvent(Vars.eventSignal);
			JS_EndRequest(cx);
		//	for(int j = 0 ; j < *argc; j++)
			//	JS_RemoveValueRoot(cx, &argv[j]);
		}
		
		//delete evt->arg1;
		//delete evt->arg2;
		
		return true;	
	}	
	if (strcmp(evtName, "setTimeout") == 0 || strcmp(evtName, "setInterval") == 0) {
		if(!clearList)
		{
			JS_BeginRequest(cx);	
			jsval* argv = new jsval[evt->argc];
			for(uintN i = 0; i < evt->argc; i++)
				evt->argv[i]->read(cx, &argv[i]);
							
				for(int j = 0 ; j < evt->argc; j++)
					JS_AddValueRoot(cx, &argv[j]);
				if(JSVAL_IS_STRING(argv[0]))
				{
					std::string test ;
					test.append( "try{ ");
					test.append( JS_EncodeString(cx,JS_ValueToString(cx, argv[0])) );
					test.append(" } catch (error){print(error)}");
					jsval rval;
					if(JS_EvaluateScript(cx, JS_GetGlobalObject(cx), test.data() , test.length(), "setTimeout", 0, &rval))
					{
						if(!JSVAL_IS_NULL(rval) && !JSVAL_IS_VOID(rval))
						{
							JS_ConvertValue(cx, rval, JSTYPE_STRING, &rval);
							char* text = JS_EncodeString(cx,JS_ValueToString(cx, rval));
							std::replace(text, text + strlen(text), '%', (char)(unsigned char)0xFE);
							Print(text);
							JS_free(cx, text);
						}
					}
				}
				
				if(JSVAL_IS_FUNCTION(cx,argv[0])) //not working
				{		
					
						jsval rval;	
					JS_CallFunctionValue(cx, JS_GetGlobalObject(cx), argv[0], evt->argc-2, &argv[2], &rval);	
					

				}
			JS_EndRequest(cx);
			for(int j = 0 ; j < evt->argc; j++)
				JS_RemoveValueRoot(cx, &argv[j]);
		}
			
		
		if (strcmp(evtName, "setTimeout") == 0)
		{
			ScriptEngine::RemoveDelayedEvent(*(DWORD*) evt->arg1);
		}	

		return true;	
	}
	if (strcmp(evtName, "DisposeMe") == 0)
	{
			ScriptEngine::DisposeScript(evt->owner);
	}
}
int ScriptEngine::AddDelayedEvent(Event* evt, int freq)
{	
	delayedExecKey++;
	evt->arg1 =  new DWORD(delayedExecKey);
	evt->arg2 = CreateWaitableTimer(NULL,true,NULL);	
	
	__int64  start;
	start = freq * -10000;
	LARGE_INTEGER lStart;
   // Copy the relative time into a LARGE_INTEGER.
	lStart.LowPart  = (DWORD) ( start & 0xFFFFFFFF );
	lStart.HighPart = (LONG)  ( start >> 32 );
	freq = 	(strcmp(evt->name, "setInterval") == 0) ? freq : 0;
	EnterCriticalSection(&Vars.cEventSection);
		DelayedExecList.push_back(evt);
		SetWaitableTimer((HANDLE*)evt->arg2, &lStart, freq, &EventTimerProc, evt, false);			
	LeaveCriticalSection(&Vars.cEventSection);

	return delayedExecKey;

}

 void ScriptEngine::RemoveDelayedEvent(int key)
{ 
	list<Event*>::iterator it;
	it = DelayedExecList.begin();
	while(it != DelayedExecList.end())
	{
		if( *(DWORD*)(*it)->arg1 ==  key)
		{
			//remove from script list if its already poped but not executed.
			list<Event*>::iterator sit;
			sit=(*it)->owner->EventList.begin();
			if( *(DWORD*)(*sit)->arg1 ==  key)
				sit = (*it)->owner->EventList.erase(sit);
			else
				sit++;

			CancelWaitableTimer((HANDLE*)(*it)->arg2);
			CloseHandle((HANDLE*)(*it)->arg2);
			Event* evt = *it;
			for(uintN i = 0; i < evt->argc; i++){
				evt->argv[i]->clear();
				delete evt->argv[i];
			}		
			delete evt;
			it = DelayedExecList.erase(it);
		}
		else
			it++;
	}
	LeaveCriticalSection(&Vars.cEventSection);
}
 void CALLBACK EventTimerProc(LPVOID lpArg, DWORD dwTimerLowValue, DWORD dwTimerHighValue ) 
 {
	Event* evt = (Event*) lpArg;
	evt->owner->FireEvent(evt);
 }