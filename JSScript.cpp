#include "JSScript.h"
#include "Script.h"
#include "ScriptEngine.h"
#include "D2BS.h"
#include "Helpers.h"

EMPTY_CTOR(script)

struct FindHelper
{
	DWORD tid;
	char* name;
	Script* script;
};

bool __fastcall FindScriptByTid(Script* script, void* argv, uint argc);
bool __fastcall FindScriptByName(Script* script, void* argv, uint argc);

JSAPI_PROP(script_getProperty)
{
	JSContext* iterp = (JSContext*)JS_GetInstancePrivate(cx, obj, &script_class, NULL);
	Script* script = (Script*)JS_GetContextPrivate(iterp);

	// TODO: make this check stronger
	if(!script)
		return JS_TRUE;
	jsval ID;
	JS_IdToValue(cx,id,&ID);
	switch(JSVAL_TO_INT(ID)) 
	{
		case SCRIPT_FILENAME:
			*vp = STRING_TO_JSVAL(JS_InternString(cx, script->GetShortFilename()));
			break;
		case SCRIPT_GAMETYPE:
			*vp = script->GetState() == InGame ? INT_TO_JSVAL(0) : INT_TO_JSVAL(1);
			break;
		case SCRIPT_RUNNING:
			*vp = BOOLEAN_TO_JSVAL(script->IsRunning());
			break;
		case SCRIPT_THREADID:
			*vp = INT_TO_JSVAL(script->GetThreadId());
			break;
		default:
			break;
	}

	return JS_TRUE;
}

JSAPI_FUNC(script_getNext)
{
	JSContext* iterp = (JSContext*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &script_class, NULL);
	if(JS_ContextIterator(ScriptEngine::GetRuntime(), &iterp) == NULL || !JS_GetContextPrivate(iterp))
		JS_SET_RVAL(cx, vp, JSVAL_FALSE);
	else
	{
		JS_SetPrivate(cx, JS_THIS_OBJECT(cx, vp), iterp);
		JS_SET_RVAL(cx, vp, JSVAL_TRUE);
	}

	return JS_TRUE;
}

JSAPI_FUNC(script_stop)
{
	JSContext* iterp = (JSContext*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &script_class, NULL);
	Script* script = (Script*)JS_GetContextPrivate(iterp);
	if(script->IsRunning())
		script->Stop();

	return JS_TRUE;
}

JSAPI_FUNC(script_pause)
{
	JSContext* iterp = (JSContext*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &script_class, NULL);
	Script* script = (Script*)JS_GetContextPrivate(iterp);

	if(script->IsRunning())
		script->Pause();

	return JS_TRUE;
}

JSAPI_FUNC(script_resume)
{
	JSContext* iterp = (JSContext*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &script_class, NULL);
	Script* script = (Script*)JS_GetContextPrivate(iterp);

	if(script->IsPaused())
		script->Resume();

	return JS_TRUE;	
}

JSAPI_FUNC(script_send)
{
	JSContext* iterp = (JSContext*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &script_class, NULL);
	Script* script = (Script*)JS_GetContextPrivate(iterp);

	AutoRoot** args = new AutoRoot*[argc];
	for(uintN i = 0; i < argc; i++)
		args[i] = new AutoRoot(JS_ARGV(cx, vp)[i]);


	/*eventHelper* helper;
	helper->eventName = "scriptmsg";
	helper->arg1 = argc;
	helper->argv = JS_ARGV(cx, vp);*/
	// this event has to occur as such because it's not a broadcasted event, just a local one
	script->ExecEventAsync("scriptmsg", argc, args);

	return JS_TRUE;
}

JSAPI_FUNC(script_join)
{
	JSContext* iterp = (JSContext*)JS_GetInstancePrivate(cx, JS_THIS_OBJECT(cx, vp), &script_class, NULL);
	Script* script = (Script*)JS_GetContextPrivate(iterp);
	
	script->Join();

	return JS_TRUE;
}

JSAPI_FUNC(my_getScript)
{
	JSContext* iterp = NULL;
	if(argc == 1 && JSVAL_IS_BOOLEAN(JS_ARGV(cx, vp)[0]) && JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[0]) == JS_TRUE)
		iterp = cx;
	else if(argc == 1 && JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
	{
		// loop over the Scripts in ScriptEngine and find the one with the right threadid
		DWORD tid = (DWORD)JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
		FindHelper args = {tid, NULL, NULL};
		ScriptEngine::ForEachScript(FindScriptByTid, &args, 1);
		if(args.script != NULL)
			iterp = args.script->GetContext();
		else
			return JS_TRUE;
	}
	else if(argc == 1 && JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
	{
		char* name = JS_EncodeString(cx,JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
		if(name)
			StringReplace(name, '/', '\\', strlen(name));
		FindHelper args = {0, name, NULL};
		ScriptEngine::ForEachScript(FindScriptByName, &args, 1);
		if(args.script != NULL)
			iterp = args.script->GetContext();
		else
			return JS_TRUE;
	}
	else
	{
		// find the first context that has private data
		while(JS_ContextIterator(ScriptEngine::GetRuntime(), &iterp) != NULL)
			if(JS_GetContextPrivate(iterp) != NULL)
				break;

		if(iterp == NULL)
			return JS_TRUE;
	}

	JSObject* res = BuildObject(cx, &script_class, script_methods, script_props, iterp);

	if(!res)
		THROW_ERROR(cx, "Failed to build the script object");
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(res));

	return JS_TRUE;
}

bool __fastcall FindScriptByName(Script* script, void* argv, uint argc)
{
	FindHelper* helper = (FindHelper*)argv;
	static uint pathlen = strlen(Vars.szScriptPath) + 1;
	const char* fname = script->GetShortFilename();
	if(_strcmpi(fname, helper->name) == 0)
	{
		helper->script = script;
		return false;
	}
	return true;
}

bool __fastcall FindScriptByTid(Script* script, void* argv, uint argc)
{
	FindHelper* helper = (FindHelper*)argv;
	if(script->GetThreadId() == helper->tid)
	{
		helper->script = script;
		return false;
	}
	return true;
}
