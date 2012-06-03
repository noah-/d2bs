#include <io.h>
#include <algorithm>

#include "Script.h"
#include "Core.h"
#include "Constants.h"
#include "D2Ptrs.h"
#include "JSUnit.h"
#include "Helpers.h"
#include "ScriptEngine.h"
#include "D2BS.h"

using namespace std;

Script::Script(const char* file, ScriptState state, uintN argc, jsval* argv) :
			context(NULL), globalObject(NULL), scriptObject(NULL), script(NULL), execCount(0),
			isAborted(false), isPaused(false), isReallyPaused(false), scriptState(state),
			threadHandle(INVALID_HANDLE_VALUE), threadId(0), argc(argc), argv(argv)
{
	InitializeCriticalSection(&lock);
	EnterCriticalSection(&lock);

	if(scriptState == Command)
	{
		fileName = string("Command Line");
	}
	else
	{
		if(_access(file, 0) != 0)
			throw std::exception("File not found");

		char* tmpName = _strdup(file);
		if(!tmpName)
			throw std::exception("Could not dup filename");

		_strlwr_s(tmpName, strlen(file)+1);
		fileName = string(tmpName);
		replace(fileName.begin(), fileName.end(), '/', '\\');
		free(tmpName);
	}
	try
	{
		context = JS_NewContext(ScriptEngine::GetRuntime(), 0x2000);
		if(!context)
			throw std::exception("Couldn't create the context");



		
		JS_SetContextThread(context);
		JS_SetContextPrivate(context, this);
		JS_BeginRequest(context);

		globalObject = JS_GetGlobalObject(context);
		jsval meVal = JSVAL_VOID;
		if(JS_GetProperty(GetContext(), globalObject, "me", &meVal) != JS_FALSE)
		{
			JSObject* meObject = JSVAL_TO_OBJECT(meVal);
			me = (myUnit*)JS_GetPrivate(GetContext(), meObject);
		}

		if(state == Command){
			char * cmd = "while (true) delay(10000);";
			script = JS_CompileScript(context, globalObject, cmd, strlen(cmd), "Command Line", 1);
		}
		else
			script = JS_CompileFile(context, globalObject, fileName.c_str());
		if(!script)
			throw std::exception("Couldn't compile the script");

		/*scriptObject = JS_NewScriptObject(context, script);
		if(!scriptObject)
			throw std::exception("Couldn't create the script object");*/

	/*	if(JS_AddNamedRoot(context, &scriptObject, "script object") == JS_FALSE)
			throw std::exception("Couldn't add named root for scriptObject");*/

		JS_EndRequest(context);
		JS_ClearContextThread(context);
		LeaveCriticalSection(&lock);
	} catch(std::exception&) {
		if(scriptObject)
			JS_RemoveRoot(context, &scriptObject);
		//if(script && !scriptObject)
			//JS_DestroyScript(context, script);
		if(context)
		{
			JS_EndRequest(context);
			//1.8 unneeded?
			//JS_DestroyContext(context);
		}
		LeaveCriticalSection(&lock);
		throw;
	}
}

Script::~Script(void)
{
	EnterCriticalSection(&lock);
	Stop(true, true);

	//JS_SetContextThread(context);

	//JS_BeginRequest(context);

	// these calls can, and probably should, be moved to the context callback on cleanup
	//JS_RemoveObjectRoot(context, &globalObject);
	//JS_RemoveRoot(&scriptObject);

	//JS_DestroyContextNoGC(context);

	context = NULL;
	scriptObject = NULL;
	globalObject = NULL;
	script = NULL;

	includes.clear();
	if(threadHandle != INVALID_HANDLE_VALUE)
		CloseHandle(threadHandle);
	LeaveCriticalSection(&lock);
	DeleteCriticalSection(&lock);
}

int Script::GetExecutionCount(void)
{
	return execCount;
}

DWORD Script::GetThreadId(void)
{
	return (threadHandle == INVALID_HANDLE_VALUE ? -1 : threadId);
}

void Script::RunCommand(const char* command)
{
	RUNCOMMANDSTRUCT* rcs = new RUNCOMMANDSTRUCT;
	size_t commandLength = strlen(command) + 1;
	char* passCommand = new char[commandLength];
	strcpy_s(passCommand, commandLength, command);

	rcs->script = this;
	rcs->command = passCommand;
	
	Event* evt = new Event;
		evt->owner = this;
		evt->argc = argc;
		evt->name = "Command";
		evt->arg1 =  passCommand;
 		//evt->arg2 =  new DWORD(helper->arg2);

		if(!JS_IsRunning(evt->owner->GetContext())){
			CreateThread(NULL, 0, RunCommandThread, (void*) rcs, 0, NULL);
		}else{

		EnterCriticalSection(&Vars.cEventSection);
		evt->owner->EventList.push_front(evt);
		LeaveCriticalSection(&Vars.cEventSection);
		//if(script->IsRunning())
			JS_TriggerOperationCallback(evt->owner->GetContext());
		}
		
}

bool Script::BeginThread(LPTHREAD_START_ROUTINE ThreadFunc)
{	
	EnterCriticalSection(&lock);
	DWORD dwExitCode = STILL_ACTIVE;
	
	if ((!GetExitCodeThread(threadHandle, &dwExitCode) || dwExitCode != STILL_ACTIVE) &&
		(threadHandle = CreateThread(0, 0, ThreadFunc, this, 0, &threadId)) != NULL)
	{
		LeaveCriticalSection(&lock);
		return true;
	}

	threadHandle = INVALID_HANDLE_VALUE;
	LeaveCriticalSection(&lock);
	return false;
}

void Script::Run(void)
{
	// only let the script run if it's not already running
	if(IsRunning())
		return;
	
	isAborted = false;

	jsval main = JSVAL_VOID, dummy = JSVAL_VOID;
	
	JS_SetContextThread(GetContext());
	JS_BeginRequest(GetContext());

	if(JS_ExecuteScript(GetContext(), globalObject, script, &dummy) != JS_FALSE &&
	   JS_GetProperty(GetContext(), globalObject, "main", &main) != JS_FALSE &&
	   JSVAL_IS_FUNCTION(GetContext(), main))
	{
		JS_AddValueRoot(GetContext(),&main);
		JS_CallFunctionValue(GetContext(), globalObject, main, this->argc, this->argv, &dummy);
		JS_RemoveValueRoot(GetContext(), &main);
	}

	
	JS_EndRequest(GetContext());
	JS_ClearContextThread(GetContext());

	execCount++;
	Stop();
}

void Script::UpdatePlayerGid(void)
{
	me->dwUnitId = (D2CLIENT_GetPlayerUnit() == NULL ? NULL : D2CLIENT_GetPlayerUnit()->dwUnitId);
}

void Script::Pause(void)
{
	//EnterCriticalSection(&lock);
	if(!IsAborted() && !IsPaused())
		isPaused = true;
	JS_TriggerOperationCallback(GetContext());
	//LeaveCriticalSection(&lock);
}

void Script::Join()
{
	EnterCriticalSection(&lock);
	HANDLE hThread = threadHandle;
	LeaveCriticalSection(&lock);

	if (hThread != INVALID_HANDLE_VALUE)
		WaitForSingleObject(hThread, INFINITE);
}

void Script::Resume(void)
{
	//EnterCriticalSection(&lock);
	if(!IsAborted() && IsPaused())
		isPaused = false;
	JS_TriggerOperationCallback(GetContext());
	//LeaveCriticalSection(&lock);
}

bool Script::IsPaused(void)
{
	return isPaused;
}

const char* Script::GetShortFilename()
{
	if(strcmp(fileName.c_str(), "Command Line") == 0) return fileName.c_str();
	else return (fileName.c_str() + strlen(Vars.szScriptPath) + 1);
}

void Script::Stop(bool force, bool reallyForce)
{
	// if we've already stopped, just return
	if(isAborted)
		return;

	EnterCriticalSection(&lock);

	// tell everyone else that the script is aborted FIRST
	isAborted = true;
	isPaused = false;
	isReallyPaused = false;
	if(GetState() != Command)
	{
		const char* displayName = fileName.c_str() + strlen(Vars.szScriptPath) + 1;
		Print("Script %s ended", displayName);
	}

	//ClearAllEvents();  //moved to cx callback
	//Genhook::Clean(this);

	// normal wait: 500ms, forced wait: 300ms, really forced wait: 100ms
	int maxCount = (force ? (reallyForce ? 10 : 30) : 50);
	for(int i = 0; IsRunning(); i++)
	{
		// if we pass the time frame, just ignore the wait because the thread will end forcefully anyway
		if(i >= maxCount)
			break;
		Sleep(10);
	}
	JS_TriggerOperationCallback(GetContext());
	if(threadHandle != INVALID_HANDLE_VALUE)
		CloseHandle(threadHandle);
	threadHandle = INVALID_HANDLE_VALUE;
	LeaveCriticalSection(&lock);
}

bool Script::IsIncluded(const char* file)
{
	uint count = 0;
	char* fname = _strdup(file);
	if(!fname)
		return false;

	_strlwr_s(fname, strlen(fname)+1);
	StringReplace(fname, '/', '\\', strlen(fname));
	count = includes.count(string(fname));
	free(fname);

	return !!count;
}

bool Script::Include(const char* file)
{
	// since includes will happen on the same thread, locking here is acceptable
	EnterCriticalSection(&lock);
	char* fname = _strdup((char*)file);
	if(!fname)
		return false;
	_strlwr_s(fname, strlen(fname)+1);
	StringReplace(fname, '/', '\\', strlen(fname));

	// don't invoke the string ctor more than once...
	string currentFileName = string(fname);
	// ignore already included, 'in-progress' includes, and self-inclusion
	if(!!includes.count(currentFileName) ||
	   !!inProgress.count(string(currentFileName)) ||
	    (fileName == string(currentFileName)))
	{
		LeaveCriticalSection(&lock);
		free(fname);
		return true;
	}
	bool rval = false;

	JSContext* cx = GetContext();

	JS_BeginRequest(cx);

	JSScript* script = JS_CompileFile(cx, GetGlobalObject(), fname);
	if(script)
	{
		//JSObject* scriptObj = JS_NewScriptObject(cx, script);
		//JS_AddRoot(&scriptObj);
		jsval dummy;
		inProgress[fname] = true;
		rval = !!JS_ExecuteScript(cx, GetGlobalObject(), script, &dummy);
		if(rval)
			includes[fname] = true;
		else
			JS_ReportPendingException(cx);
		inProgress.erase(fname);
		//JS_RemoveRoot(&scriptObj);
	} else JS_ReportPendingException(cx);

	JS_EndRequest(cx);
	LeaveCriticalSection(&lock);
	free(fname);
	return rval;
}

bool Script::IsRunning(void)
{
	return context && !(!JS_IsRunning(context) || IsPaused());
}

bool Script::IsAborted()
{
	return isAborted;
}

bool Script::IsListenerRegistered(const char* evtName)
{
	return strlen(evtName) > 0 && functions.count(evtName) > 0;
}

void Script::RegisterEvent(const char* evtName, jsval evtFunc)
{
	EnterCriticalSection(&lock);
	if(JSVAL_IS_FUNCTION(context, evtFunc) && strlen(evtName) > 0)
	{
		AutoRoot* root = new AutoRoot(context, evtFunc);
		functions[evtName].push_back(root);
	}
	LeaveCriticalSection(&lock);
}

bool Script::IsRegisteredEvent(const char* evtName, jsval evtFunc)
{
	// nothing can be registered under an empty name
	if(strlen(evtName) < 1)
		return false;

	// if there are no events registered under that name at all, then obviously there
	// can't be a specific one registered under that name
	if(functions.count(evtName) < 1)
		return false;

	for(FunctionList::iterator it = functions[evtName].begin(); it != functions[evtName].end(); it++)
		if(*(*it)->value() == evtFunc)
			return true;

	return false;
}

void Script::UnregisterEvent(const char* evtName, jsval evtFunc)
{
	if(strlen(evtName) < 1)
		return;

	EnterCriticalSection(&lock);
	AutoRoot* func = NULL;
	for(FunctionList::iterator it = functions[evtName].begin(); it != functions[evtName].end(); it++)
	{
		if(*(*it)->value() == evtFunc)
		{
			func = *it;
			break;
		}
	}
	functions[evtName].remove(func);
	func->Release();
	delete func;
	LeaveCriticalSection(&lock);
}

void Script::ClearEvent(const char* evtName)
{
	EnterCriticalSection(&lock);
	for(FunctionList::iterator it = functions[evtName].begin(); it != functions[evtName].end(); it++)
	{
		AutoRoot* func = *it;
		func->Release();
		delete func;
	}
	functions[evtName].clear();
	LeaveCriticalSection(&lock);
}

void Script::ClearAllEvents(void)
{
	EnterCriticalSection(&lock);
	for(FunctionMap::iterator it = functions.begin(); it != functions.end(); it++)
		ClearEvent(it->first.c_str());
	functions.clear();
	LeaveCriticalSection(&lock);
}

void Script::ExecEventAsync(char* evtName, uintN argc, AutoRoot** argv)
{
	if(!(!IsAborted() && !IsPaused() && functions.count(evtName)))
	{
		// no event will happen, clean up the roots
		for(uintN i = 0; i < argc; i++)
			delete argv[i];
		delete[] argv;
		return;
	}

	/*for(uintN i = 0; i < argc; i++)
		argv[i]->Take();
*/
	/*char* test = "testing";
	
	Event* evt = new Event;
	evt->owner = this;
	evt->functions = functions[evtName];
	evt->argc = argc;
	evt->argv = argv;
	evt->name = test;
	evt->object = globalObject;

	int timeout = 0;
	EnterCriticalSection(&Vars.cEventSection) ;
	
	evt->owner->EventList.push_front(evt);
	LeaveCriticalSection(&Vars.cEventSection);
	JS_TriggerOperationCallback(evt->owner->GetContext());*/

}
bool Script::ExecEvent(char* evtName, uintN argc, AutoRoot** argv)
{
	//if(!(!IsAborted() && !IsPaused() && functions.count(evtName)))
	//{
	//	// no event will happen, clean up the roots
	//	for(uintN i = 0; i < argc; i++)
	//		delete argv[i];
	//	delete[] argv;
	//	return false;
	//}

	//for(uintN i = 0; i < argc; i++)
	//	argv[i]->Take();

	//Event* evt = new Event;
	//evt->owner = this;
	//evt->functions = functions[evtName];
	//evt->argc = argc;
	//evt->argv = argv;
	//evt->object = globalObject;
	//DWORD ExitCode = 0;
	/*HANDLE hThread;
	hThread = CreateThread(0, 0, FuncThread, evt, 0, 0);
	WaitForSingleObject(hThread, 1000);
	
	GetExitCodeThread( hThread, &ExitCode);
	CloseHandle(hThread);
	*/
	return false; //(ExitCode == 1);
}

#ifdef DEBUG
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // must be 0x1000
	LPCSTR szName; // pointer to name (in user addr space)
	DWORD dwThreadID; // thread ID (-1=caller thread)
	DWORD dwFlags; // reserved for future use, must be zero
} THREADNAME_INFO;

void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = szThreadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try
	{
		RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info );
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}
#endif

DWORD WINAPI RunCommandThread(void* data)
{
	RUNCOMMANDSTRUCT* rcs = (RUNCOMMANDSTRUCT*) data;
	
	//JSContext* cx = JS_NewContext(ScriptEngine::GetRuntime(), 8192);
	//JS_SetContextPrivate(cx, rcs->script);
	JSContext* cx = rcs->script->GetContext();
	JS_SetContextThread(cx);
	JS_BeginRequest(cx);
	jsval rval;
	//JSContext* orignalCx = rcs->script->GetContext();
	//rcs->script->SetContext(cx);

	if(JS_EvaluateScript(cx, JS_GetGlobalObject(cx), rcs->command, strlen(rcs->command), "Command Line", 0, &rval))
	{
		if(!JSVAL_IS_NULL(rval) && !JSVAL_IS_VOID(rval))
		{
			JS_ConvertValue(cx, rval, JSTYPE_STRING, &rval);
			char* text =_strdup( JS_EncodeString(cx,JS_ValueToString(cx, rval)));
			std::replace(text, text + strlen(text), '%', (char)(unsigned char)0xFE);
			Print(text);
			free(text);
		}
	}
	JS_EndRequest(cx);
	//JS_DestroyContext(cx);// 1.8 needed? it crashes
	JS_ClearContextThread(cx);
	JS_TriggerOperationCallback(cx);
	//rcs->script->SetContext(orignalCx);
	
//	delete rcs->command;
	//delete rcs;

	return 0;
}

DWORD WINAPI ScriptThread(void* data)
{
	Script* script = (Script*)data;
	if(script)
	{
#ifdef DEBUG
		SetThreadName(0xFFFFFFFF, script->GetShortFilename());
#endif
		script->Run();
		if(Vars.bDisableCache)
			ScriptEngine::DisposeScript(script);
	}
	return 0;
}

DWORD WINAPI FuncThread(void* data)
{
	Event* evt = (Event*)data;
	if(!evt)
		return 0;

	JSContext* cx = JS_NewContext(ScriptEngine::GetRuntime(), 8192);
	JS_SetContextPrivate(cx, evt->owner);
	JS_BeginRequest(cx);
	bool block =false;
	if(evt->owner->IsRunning() && !(evt->owner->GetState() == InGame && ClientState() != ClientStateInGame))
	{
		jsval* args = new jsval[evt->argc];
		for(uintN i = 0; i < evt->argc; i++)
		{
			args[i] = *evt->argv[i]->value();
			if(JS_AddRoot(evt->owner->GetContext(), &args[i]) == JS_FALSE)
			{
				if(evt->argv)
					delete[] evt->argv;
				delete evt;
				return NULL;
			}
		}
		jsval rval = JSVAL_VOID;

		for(FunctionList::iterator it = evt->functions.begin(); it != evt->functions.end(); it++)
		{
			JS_CallFunctionValue(cx, evt->object, *(*it)->value(), evt->argc, args, &rval);
			block |= (JSVAL_IS_BOOLEAN(rval) && JSVAL_TO_BOOLEAN(rval));
		}

		for(uintN i = 0; i < evt->argc; i++)
			JS_RemoveRoot(evt->owner->GetContext(), &args[i]);
		delete[] args;
	}

	JS_DestroyContextNoGC(cx);
	// we have to clean up the event
	for(uintN i = 0; i < evt->argc; i++)
	{
		evt->argv[i]->Release();
		if(evt->argv[i])
			delete evt->argv[i];
	}
	if(evt->argv)
		delete[] evt->argv;
	delete evt;
	
	return block;
}
DWORD WINAPI EventThread(LPVOID lpParam)
{

	while(Vars.bNeedShutdown)
	{
		Sleep(10);
		while(Vars.EventList.size() > 0)
		{
			EnterCriticalSection(&Vars.cEventSection);
				Event* evt = Vars.EventList.back();
				Vars.EventList.pop_back();
			LeaveCriticalSection(&Vars.cEventSection);
			
			JSContext* cx = JS_NewContext(ScriptEngine::GetRuntime(), 8192);
			JS_SetContextPrivate(cx, evt->owner);
			JS_BeginRequest(cx);

				callEventFunction(cx,evt); // call the first event
			bool match = false;  // vars to keep event list unlocked dont want the list locked while in event call
			bool fullSearch = false; 
			while (!fullSearch)
			{
				match = false;
				EnterCriticalSection(&Vars.cEventSection); // call any other events on the que with the same script
				for(list<Event*>::iterator it = Vars.EventList.begin(); it != Vars.EventList.end(); it++)
				{					
					if((*it)->owner->GetThreadId() == evt->owner->GetThreadId())
					{
						match = true;
						evt=(*it);
						Vars.EventList.erase(it);
						break;
					}			
				}
				LeaveCriticalSection(&Vars.cEventSection);
				if(match)				
					callEventFunction(cx,evt);
				else
					fullSearch = true; 
			}

			JS_DestroyContextNoGC(cx);
			// we have to clean up the event
			for(uintN i = 0; i < evt->argc; i++)
			{
				evt->argv[i]->Release();
				if(evt->argv[i])
					delete evt->argv[i];
			}
			if(evt->argv)
				delete[] evt->argv;
			delete evt;
		}		
	}			
	return true;
}
bool callEventFunction(JSContext* cx ,Event* evt)
{
	bool block =false;
	if(evt->owner->IsRunning() && !(evt->owner->GetState() == InGame && ClientState() != ClientStateInGame))
	{
		jsval* args = new jsval[evt->argc];
		for(uintN i = 0; i < evt->argc; i++)
		{
			args[i] = *evt->argv[i]->value();
			if(JS_AddRoot(evt->owner->GetContext(), &args[i]) == JS_FALSE)
			{
				if(evt->argv)
					delete[] evt->argv;
				delete evt;
				return false;
			}
		}
		jsval rval = JSVAL_VOID;

		for(FunctionList::iterator it = evt->functions.begin(); it != evt->functions.end(); it++)
		{
			JS_CallFunctionValue(cx, evt->object, *(*it)->value(), evt->argc, args, &rval);
			block |= (JSVAL_IS_BOOLEAN(rval) && JSVAL_TO_BOOLEAN(rval));
		}

		for(uintN i = 0; i < evt->argc; i++)
			JS_RemoveRoot(evt->owner->GetContext(), &args[i]);
		delete[] args;
	}
return block;
}
