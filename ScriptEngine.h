#pragma once
#ifndef __SCRIPTENGINE_H__
#define __SCRIPTENGINE_H__

#include <list>
#include <map>
#include <string>

#include "js32.h"
#include "AutoRoot.h"
#include "Script.h"

typedef std::map<std::string, Script*> ScriptMap;

typedef bool (__fastcall *ScriptCallback)(Script*, void*, uint);

enum EngineState {
	Starting,
	Running,
	Paused,
	Stopping,
	Stopped
};

class ScriptEngine
{
	ScriptEngine(void) {};
	virtual ~ScriptEngine(void) = 0;
	ScriptEngine(const ScriptEngine&);
	ScriptEngine& operator=(const ScriptEngine&);
	static JSRuntime* runtime;
	static JSContext* context;
	static Script* console;
	static EngineState state;
	static std::list<Event*> DelayedExecList;
	static int delayedExecKey;
	static CRITICAL_SECTION scriptListLock;
public:
	friend class Script;
	static ScriptMap scripts;
	
	static CRITICAL_SECTION lock;
	static BOOL Startup(void);
	static void Shutdown(void);
	static EngineState GetState(void) { return state; }

	static void FlushCache(void);

	static Script* CompileFile(const char* file, ScriptState state, uintN argc = 0, jsval* argv = NULL, bool recompile = false);
	static void RunCommand(const char* command);
	static void DisposeScript(Script* script);

	static void LockScriptList( char* loc);
	static void UnLockScriptList( char* loc);

	static bool ForEachScript(ScriptCallback callback, void* argv, uint argc);
	static unsigned int GetCount(bool active = true, bool unexecuted = false);

	static JSRuntime* GetRuntime(void) { return runtime; }
	static JSContext* GetGlobalContext(void) { return context; }
	
	static void StopAll(bool forceStop = false);
	static void ExecEventAsync(char* evtName, AutoRoot** argv, uintN argc);
	static void InitClass(JSContext* context, JSObject* globalObject, JSClass* classp,
							 JSFunctionSpec* methods, JSPropertySpec* props,
							 JSFunctionSpec* s_methods, JSPropertySpec* s_props);
	static void DefineConstant(JSContext* context, JSObject* globalObject, const char* name, int value);
	static void UpdateConsole();
	static int AddDelayedEvent(Event* evt, int freq);
	static void RemoveDelayedEvent(int key);
	JSGCCallback gcCallback(JSRuntime* rt, JSGCStatus status);
};

// these ForEachScript helpers are exposed in case they can be of use somewhere
bool __fastcall StopIngameScript(Script* script, void*, uint);
bool __fastcall ExecEventOnScript(Script* script, void* argv, uint argc);
struct EventHelper
{
	char* evtName;
	AutoRoot** argv;
	uintN argc;
	bool executed;
};
JSBool operationCallback (JSContext *cx);
JSBool contextCallback(JSContext* cx, uintN contextOp);
//gcCallback(JSContext* cx, JSGCStatus status);
void reportError(JSContext *cx, const char *message, JSErrorReport *report);
bool ExecScriptEvent(Event* evt,bool clearList);
void CALLBACK EventTimerProc(LPVOID lpArg, DWORD dwTimerLowValue, DWORD dwTimerHighValue);
#endif

