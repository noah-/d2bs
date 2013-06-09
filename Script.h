#pragma once

#include <windows.h>
#include <string>
#include <map>
#include <list>

#include "js32.h"
#include "JSGlobalClasses.h"
#include "AutoRoot.h"
#include "JSUnit.h"
#include "Events.h"
enum ScriptState {
	InGame,
	OutOfGame,
	Command
};

class Script;

typedef std::map<std::string, bool> IncludeList;
typedef std::list<AutoRoot*> FunctionList;
typedef std::map<std::string, FunctionList> FunctionMap;
typedef std::list<Script*> ScriptList;

struct Event {
	Script* owner;
	JSObject* object;
	FunctionList functions;
	JSAutoStructuredCloneBuffer** argv;
	uintN argc;	
	char* name;
	void* arg1;
	void* arg2;
	void* arg3; 
	void* arg4;
	void* arg5;
};

class Script
{
private:
	std::string fileName;
	int execCount;
	ScriptState scriptState;
	JSContext* context;
	JSScript* script;
	JSRuntime* runtime;
	myUnit* me;
	uintN argc;
	jsval* argv;

	JSObject *globalObject, *scriptObject;
	bool isLocked, isPaused, isReallyPaused, isAborted;

	IncludeList includes, inProgress;
	
	HANDLE threadHandle;
	
	CRITICAL_SECTION lock;

	Script(const char* file, ScriptState state, uintN argc = 0, jsval* argv = NULL);
	Script(const Script&);
	Script& operator=(const Script&);
	~Script(void);

public:
	DWORD threadId;
	friend class ScriptEngine;
	FunctionMap functions;
	void Run(void);
	void Join(void);
	void Pause(void);
	void Resume(void);
	bool IsPaused(void);
	bool BeginThread(LPTHREAD_START_ROUTINE ThreadFunc);
	void RunCommand(const char* command);
	inline void SetPauseState(bool reallyPaused) { isReallyPaused = reallyPaused; }
	inline bool IsReallyPaused(void) { return isReallyPaused; }
	void Stop(bool force = false, bool reallyForce = false);
	
	inline const char* GetFilename(void) { return fileName.c_str(); }
	const char* GetShortFilename(void);
	inline JSContext* GetContext(void) { return context; }
	inline JSRuntime* GetRuntime(void) { return runtime; }
	inline JSObject* GetGlobalObject(void) { return globalObject; }
	inline JSObject* GetScriptObject(void) { return scriptObject; }
	inline ScriptState GetState(void) { return scriptState; }
	inline void TriggerOperationCallback(void) {if (hasActiveCX) JS_TriggerOperationCallback(runtime);}
	int GetExecutionCount(void);
	DWORD GetThreadId(void);
	DWORD LastGC;
	bool hasActiveCX; //hack to get away from JS_IsRunning
	HANDLE eventSignal;

	// UGLY HACK to fix up the player gid on game join for cached scripts/oog scripts
	void UpdatePlayerGid(void);
	// Hack. Include from console needs to run on the RunCommandThread / cx. 
	//		 a better solution may be to keep a list of threadId / cx and have a GetCurrentThreadCX()   
	inline void SetContext(JSContext* cx) { context = cx;}
	bool IsRunning(void);
	bool IsAborted(void);
	void Lock() { EnterCriticalSection(&lock);} // needed for events walking function list
	void Unlock() { LeaveCriticalSection(&lock);}
	bool IsIncluded(const char* file);
	bool Include(const char* file);

	bool IsListenerRegistered(const char* evtName);
	void RegisterEvent(const char* evtName, jsval evtFunc);
	bool IsRegisteredEvent(const char* evtName, jsval evtFunc);
	void UnregisterEvent(const char* evtName, jsval evtFunc);
	void ClearEvent(const char* evtName);
	void ClearAllEvents(void);
	void FireEvent(Event* );
	std::list<Event*> EventList;	
};

struct RUNCOMMANDSTRUCT {
	Script* script;
	const char* command;
};

DWORD WINAPI RunCommandThread(void* data);
DWORD WINAPI ScriptThread(void* data);
DWORD WINAPI FuncThread(void* data);
DWORD WINAPI EventThread(LPVOID lpParam);
bool callEventFunction(JSContext* cx ,Event* evt);
