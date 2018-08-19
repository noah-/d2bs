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

Script::Script(const char* file, ScriptState state, uintN argc, JSAutoStructuredCloneBuffer** argv) :
			context(NULL), globalObject(NULL), scriptObject(NULL), script(NULL), execCount(0),
			isAborted(false), isPaused(false), isReallyPaused(false), scriptState(state),
			threadHandle(INVALID_HANDLE_VALUE), threadId(0), argc(argc), argv(argv)
{
	InitializeCriticalSection(&lock);
	// moved the runtime initilization to thread start	
	LastGC = 0;
	hasActiveCX=false;
	eventSignal = CreateEvent(nullptr, true, false, nullptr);
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
}

Script::~Script(void)
{
    isAborted = true;
	hasActiveCX = false;
    //JS_SetPendingException(context, JSVAL_NULL);
    if(JS_IsInRequest(runtime))
        JS_EndRequest(context);
	
    EnterCriticalSection(&lock);
//    JS_SetRuntimeThread(rt);
    JS_DestroyContext(context);
    //JS_ClearRuntimeThread(rt);
    JS_DestroyRuntime(runtime);


    context = NULL;
    scriptObject = NULL;
    globalObject = NULL;
    script = NULL;
    CloseHandle(eventSignal);
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
	
		if(isAborted){  //this should never happen -bob
			char* command = "delay(1000000);";
			RUNCOMMANDSTRUCT* rcs = new RUNCOMMANDSTRUCT;
			size_t commandLength = strlen(command) + 1;
			char* passCommand = new char[commandLength];
			strcpy_s(passCommand, commandLength, command);

			rcs->script = this;
			rcs->command = passCommand;
	
			Log("Console Aborted HELP!");
			//HANDLE hwnd = CreateThread(NULL, 0, RunCommandThread, (void*) rcs, 0, NULL);
			
		}
			Event* evt = new Event;
			evt->owner = this;
			evt->argc = argc;
			evt->name = "Command";
			evt->arg1 =  passCommand;
			EnterCriticalSection(&Vars.cEventSection);
			evt->owner->EventList.push_front(evt);
			LeaveCriticalSection(&Vars.cEventSection);
			evt->owner->TriggerOperationCallback();
			SetEvent(evt->owner->eventSignal);
		
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
	try
	{
		runtime = JS_NewRuntime(Vars.dwMemUsage,JS_NO_HELPER_THREADS);   
		
		//JS_SetRuntimeThread(runtime);
		JS_SetContextCallback(runtime, contextCallback);
			
		context = JS_NewContext(runtime, 0x2000);
		if(!context)
			throw std::exception("Couldn't create the context");
								
		JS_SetErrorReporter(context, reportError);
		JS_SetOperationCallback(context, operationCallback);
		JS_SetOptions(context, JSOPTION_STRICT|JSOPTION_VAROBJFIX);
		JS_SetVersion(context, JSVERSION_LATEST);
//	
		
		JS_SetContextPrivate(context, this);
		
		JS_BeginRequest(context);
		

		globalObject = JS_GetGlobalObject(context);
		jsval meVal = JSVAL_VOID;
		if(JS_GetProperty(GetContext(), globalObject, "me", &meVal) != JS_FALSE)
		{
			JSObject* meObject = JSVAL_TO_OBJECT(meVal);
			me = (myUnit*)JS_GetPrivate(GetContext(), meObject);
		}

		if(scriptState == Command){			
			char * cmd = "var global = this;function main() { print('�c2D2BS�c0 :: Started Console'); while (true){delay(10000)};}  ";
			script = JS_CompileScript(context, globalObject, cmd, strlen(cmd), "Command Line", 1);
			JS_AddNamedScriptRoot(context, &script, "console script");
		}
		else{
			
			std::string fileHeader;
			fileHeader.append( "var global = this;function mainLoop() { while (true){delay(100)};} ");
			
			std::ifstream ifs(fileName.c_str());
			std::string content( (std::istreambuf_iterator<char>(ifs) ),
								   (std::istreambuf_iterator<char>()    ) );

			fileHeader.append( content );
			fileHeader.append( "" );
			
			char * cmd = (char *)fileHeader.data();

			script = JS_CompileScript(context, globalObject, cmd, strlen(cmd),  fileName.c_str() , 1);
			
			//script = JS_CompileFile(context, globalObject, fileName.c_str());
		}
		if(!script)
			throw std::exception("Couldn't compile the script");
		
		JS_EndRequest(context);
		//JS_RemoveScriptRoot(context, &script);
	} catch(std::exception&) {
		if(scriptObject)
			JS_RemoveRoot(context, &scriptObject);
		if(context)
		{
			JS_EndRequest(context);
			JS_DestroyContext(context);
		}
		LeaveCriticalSection(&lock);
		throw;
	}
	// only let the script run if it's not already running
	if(IsRunning())
		return;
	hasActiveCX=true;	
	isAborted = false;

	jsval main = INT_TO_JSVAL(1), dummy = INT_TO_JSVAL(1);
	JS_BeginRequest(GetContext());
	
	//args passed from load
	jsval* argvalue = new jsval[argc];
			for(uintN i = 0; i < argc; i++)
				argv[i]->read(context, &argvalue[i]);			
				
			for(uint j = 0 ; j < argc; j++)
				JS_AddValueRoot(context, &argvalue[j]);


	
	JS_AddValueRoot(GetContext(), &main);
	JS_AddValueRoot(GetContext(), &dummy);
	if(JS_ExecuteScript(GetContext(), globalObject, script, &dummy) != JS_FALSE &&
	   JS_GetProperty(GetContext(), globalObject, "main", &main) != JS_FALSE && 
	   JSVAL_IS_FUNCTION(GetContext(), main))
	{	
		JS_CallFunctionValue(GetContext(), globalObject, main, this->argc, argvalue, &dummy);			
	}
	JS_RemoveValueRoot(GetContext(), &main);
	JS_RemoveValueRoot(GetContext(), &dummy);
	for(uint j = 0 ; j < argc; j++)
		JS_RemoveValueRoot(GetContext(), &argvalue[j]);
	
	/*for(uintN i = 0; i < argc; i++)  //crashes spidermonkey cleans itself up?
	{
		argv[i]->clear();
		delete argv[i];
	}*/
	
	std::string loop;

	loop.append( "try{ mainLoop() } catch (error){print(error)}");
		
	jsval rval;
	JS_EvaluateScript(GetContext(), globalObject, loop.data() , loop.length(), "Main Loop", 0 , &rval);

	JS_EndRequest(GetContext());
	
	execCount++;
	//Stop();
}

void Script::UpdatePlayerGid(void)
{
		me->dwUnitId = (D2CLIENT_GetPlayerUnit() == NULL ? NULL : D2CLIENT_GetPlayerUnit()->dwUnitId);
}

void Script::Pause(void)
{
	if(!IsAborted() && !IsPaused())
		isPaused = true;
	TriggerOperationCallback();
	SetEvent(eventSignal);
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
	if(!IsAborted() && IsPaused())
		isPaused = false;
	TriggerOperationCallback();	
	SetEvent(eventSignal);
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

	//trigger call back so script ends
	TriggerOperationCallback();
	SetEvent(eventSignal);

	// normal wait: 500ms, forced wait: 300ms, really forced wait: 100ms
	int maxCount = (force ? (reallyForce ? 10 : 30) : 50);
	if (GetCurrentThreadId() != GetThreadId()){
		for(int i = 0; hasActiveCX == true; i++)
		{
			// if we pass the time frame, just ignore the wait because the thread will end forcefully anyway
			if(i >= maxCount)
				break;
			Sleep(10);
		}
	}
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
	//JS_RemoveScriptRoot(cx, &script);
	LeaveCriticalSection(&lock);
	free(fname);
	return rval;
}

bool Script::IsRunning(void)
{
	return context && !(IsAborted() || IsPaused() || !hasActiveCX); 
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
	//func->Release();
	delete func;

	// Remove event completely if there are no listeners for it.
	if (functions.count(evtName) > 0 && functions[evtName].size() == 0)
		functions.erase(evtName);

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
void Script::FireEvent(Event* evt)
{ 
	//EnterCriticalSection(&ScriptEngine::lock);
	EnterCriticalSection(&Vars.cEventSection);
		evt->owner->EventList.push_front(evt);
	LeaveCriticalSection(&Vars.cEventSection);

	if (evt->owner && evt->owner->IsRunning())
	{		
		evt->owner->TriggerOperationCallback();
	}
	SetEvent(eventSignal);
	//LeaveCriticalSection(&ScriptEngine::lock);	
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
	JSContext* cx = rcs->script->GetContext();
	JS_BeginRequest(cx);
	jsval rval;
	JS_AddNamedValueRoot(cx, &rval, "Cmd line rtl");
	if(JS_EvaluateScript(cx, JS_GetGlobalObject(cx), rcs->command, strlen(rcs->command), "Command Line", 0, &rval))
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
	JS_RemoveRoot(cx, &rval);
	JS_EndRequest(cx);
	JS_TriggerOperationCallback(JS_GetRuntime(cx));
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
