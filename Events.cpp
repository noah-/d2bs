#include "ScriptEngine.h"

struct ChatEventHelper
{
	char *event, *nick, *msg;
};

struct CopyDataHelper
{
	DWORD mode;
	char* msg;
};

struct ItemEventHelper
{
	DWORD id;
	char* code;
	WORD mode;
	bool global;
};

struct KeyEventHelper
{
	BOOL up;
	WPARAM key;
};

struct GameActionEventHelper
{
	BYTE mode;
	DWORD param1, param2;
	char *name1, *name2;
};

struct SingleArgHelper
{
	DWORD arg1;
};

struct DoubleArgHelper
{
	DWORD arg1, arg2;
};

struct TripleArgHelper
{
	DWORD arg1, arg2, arg3;
};
struct QuadArgHelper
{
	DWORD arg1, arg2, arg3, arg4;
};

struct BCastEventHelper
{
	jsval* argv;
	uintN argc;
};

bool __fastcall LifeEventCallback(Script* script, void* argv, uint argc)
{
	SingleArgHelper* helper = (SingleArgHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("melife"))
	{
		AutoRoot** argv2 = new AutoRoot*[1];
		argv2[0] = new AutoRoot();
		JS_NewNumberValue(ScriptEngine::GetGlobalContext(), helper->arg1, argv2[0]->value());
		script->ExecEventAsync("melife", 1, argv2);
	}
	return true;
}

void LifeEvent(DWORD dwLife)
{
	SingleArgHelper helper = {dwLife};
	ScriptEngine::ForEachScript(LifeEventCallback, &helper, 1);
}

bool __fastcall ManaEventCallback(Script* script, void* argv, uint argc)
{
	SingleArgHelper* helper = (SingleArgHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("memana"))
	{
		AutoRoot** argv = new AutoRoot*[1];
		argv[0] = new AutoRoot();
		JS_NewNumberValue(ScriptEngine::GetGlobalContext(), helper->arg1, argv[0]->value());
		script->ExecEventAsync("memana", 1, argv);
	}
	return true;
}

void ManaEvent(DWORD dwMana)
{
	SingleArgHelper helper = {dwMana};
	ScriptEngine::ForEachScript(ManaEventCallback, &helper, 1);
}

bool __fastcall KeyEventCallback(Script* script, void* argv, uint argc)
{
	KeyEventHelper* helper = (KeyEventHelper*)argv;
	char* event = (helper->up ? "keyup" : "keydown");
	if(script->IsRunning() && script->IsListenerRegistered(event))
	{
		AutoRoot** argv = new AutoRoot*[1];
		argv[0] = new AutoRoot();
		JS_NewNumberValue(ScriptEngine::GetGlobalContext(), helper->key, argv[0]->value());
		script->ExecEventAsync(event, 1, argv);
	}
	bool block = false;
	event = (helper->up ? "keyupblocker" : "keydownblocker");
	if(script->IsRunning() && script->IsListenerRegistered(event))
	{
		AutoRoot** argv = new AutoRoot*[1];
		argv[0] = new AutoRoot();
		JS_NewNumberValue(ScriptEngine::GetGlobalContext(), helper->key, argv[0]->value());
		if(script->ExecEvent(event, 1, argv))
			block = true;
	}

	return block;
}

bool KeyDownUpEvent(WPARAM key, BYTE bUp)
{
	KeyEventHelper helper = {bUp, key};
	return ScriptEngine::ForEachScript(KeyEventCallback, &helper, 1);
}

bool __fastcall PlayerAssignCallback(Script* script, void* argv, uint argc)
{
	SingleArgHelper* helper = (SingleArgHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("playerassign"))
	{
		AutoRoot** argv = new AutoRoot*[1];
		argv[0] = new AutoRoot();
		JS_NewNumberValue(ScriptEngine::GetGlobalContext(), helper->arg1, argv[0]->value());
		script->ExecEventAsync("playerassign", 1, argv);
	}
	return true;
}

void PlayerAssignEvent(DWORD dwUnitId)
{
	SingleArgHelper helper = {dwUnitId};
	ScriptEngine::ForEachScript(PlayerAssignCallback, &helper, 1);
}

bool __fastcall MouseClickCallback(Script* script, void* argv, uint argc)
{
	QuadArgHelper* helper = (QuadArgHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("mouseclick"))
	{
		AutoRoot** argv = new AutoRoot*[4];
		argv[0] = new AutoRoot(INT_TO_JSVAL(helper->arg1));
		argv[1] = new AutoRoot(INT_TO_JSVAL(helper->arg2));
		argv[2] = new AutoRoot(INT_TO_JSVAL(helper->arg3));
		argv[3] = new AutoRoot(INT_TO_JSVAL(helper->arg4));
	
		script->ExecEventAsync("mouseclick", 4, argv);
	}
	return true;
}

void MouseClickEvent(int button, POINT pt, bool bUp)
{
	QuadArgHelper helper = {button, pt.x, pt.y, bUp};
	ScriptEngine::ForEachScript(MouseClickCallback, &helper, 1);
}

bool __fastcall MouseMoveCallback(Script* script, void* argv, uint argc)
{
	DoubleArgHelper* helper = (DoubleArgHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("mousemove"))
	{
		AutoRoot** argv = new AutoRoot*[2];
		argv[0] = new AutoRoot();
		argv[1] = new AutoRoot();
		JS_NewNumberValue(ScriptEngine::GetGlobalContext(), helper->arg1, argv[0]->value());
		JS_NewNumberValue(ScriptEngine::GetGlobalContext(), helper->arg2, argv[1]->value());
		script->ExecEventAsync("mousemove", 2, argv);
	}
	return true;
}

void MouseMoveEvent(POINT pt)
{
	if (pt.x < 1 || pt.y < 1)
		return;
	DoubleArgHelper helper = {pt.x, pt.y};
	ScriptEngine::ForEachScript(MouseMoveCallback, &helper, 1);
}

bool __fastcall BCastEventCallback(Script* script, void* argv, uint argc)
{
	BCastEventHelper* helper = (BCastEventHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("scriptmsg"))
	{
		AutoRoot** args = new AutoRoot*[helper->argc];
		for(uintN i = 0; i < helper->argc; i++)
			args[i] = new AutoRoot(helper->argv[i]);
		script->ExecEventAsync("scriptmsg", helper->argc, args);
	}
	return true;
}

void ScriptBroadcastEvent(uintN argc, jsval* args)
{
	BCastEventHelper helper = {args, argc};
	ScriptEngine::ForEachScript(BCastEventCallback, &helper, 1);
}

bool __fastcall GoldDropCallback(Script* script, void* argv, uint argc)
{
	DoubleArgHelper* helper = (DoubleArgHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("golddrop"))
	{
		AutoRoot** argv = new AutoRoot*[2];
		argv[0] = new AutoRoot();
		argv[1] = new AutoRoot();
		JS_NewNumberValue(ScriptEngine::GetGlobalContext(), helper->arg1, argv[0]->value());
		JS_NewNumberValue(ScriptEngine::GetGlobalContext(), helper->arg2, argv[1]->value());
		script->ExecEventAsync("golddrop", 2, argv);
	}
	return true;
}

void GoldDropEvent(DWORD GID, BYTE Mode)
{
	DoubleArgHelper helper = {GID, Mode};
	ScriptEngine::ForEachScript(GoldDropCallback, &helper, 1);
}

bool __fastcall ChatEventCallback(Script* script, void* argv, uint argc)
{
	ChatEventHelper* helper = (ChatEventHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered(helper->event))
	{
		AutoRoot** argv = new AutoRoot*[2];
		JS_SetContextThread(ScriptEngine::GetGlobalContext());
		argv[0] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(ScriptEngine::GetGlobalContext(), helper->nick)));
		argv[1] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(ScriptEngine::GetGlobalContext(), helper->msg)));
		JS_ClearContextThread(ScriptEngine::GetGlobalContext());
		script->ExecEventAsync(helper->event, 2, argv);
	}
	std::string evt = helper->event;
	evt = evt + "blocker";
	bool block = false;

	if(script->IsRunning() && script->IsListenerRegistered(evt.c_str()))
	{
		AutoRoot** argv = new AutoRoot*[2];
		JS_SetContextThread(ScriptEngine::GetGlobalContext());
		argv[0] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(ScriptEngine::GetGlobalContext(), helper->nick)));
		argv[1] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(ScriptEngine::GetGlobalContext(), helper->msg)));
		JS_ClearContextThread(ScriptEngine::GetGlobalContext());
		if (script->ExecEvent(const_cast<char*>(evt.c_str()), 2, argv))
			block = true;
	}
	return block;
}

bool ChatEvent(char* lpszNick, char* lpszMsg)
{
	ChatEventHelper helper = {"chatmsg", lpszNick, lpszMsg};
	return ScriptEngine::ForEachScript(ChatEventCallback, &helper, 1);
}

bool WhisperEvent(char* lpszNick, char* lpszMsg)
{
	ChatEventHelper helper = {"whispermsg", lpszNick, lpszMsg};
	return ScriptEngine::ForEachScript(ChatEventCallback, &helper, 1);
}

bool __fastcall CopyDataCallback(Script* script, void* argv, uint argc)
{
	CopyDataHelper* helper = (CopyDataHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("copydata"))
	{
		AutoRoot** argv = new AutoRoot*[2];
		JS_SetContextThread(ScriptEngine::GetGlobalContext());
		argv[0] = new AutoRoot();
		JS_NewNumberValue(ScriptEngine::GetGlobalContext(), helper->mode, argv[0]->value());
		argv[1] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(ScriptEngine::GetGlobalContext(), helper->msg)));
		JS_ClearContextThread(ScriptEngine::GetGlobalContext());
		script->ExecEventAsync("copydata", 2, argv);
	}
	return true;
}

void CopyDataEvent(DWORD dwMode, char* lpszMsg)
{
	CopyDataHelper helper = {dwMode, lpszMsg};
	ScriptEngine::ForEachScript(CopyDataCallback, &helper, 1);
}

bool __fastcall GameEventCallback(Script* script, void* argv, uint argc)
{
	if(script->IsRunning() && script->IsListenerRegistered("gamemsg"))
	{
		AutoRoot** argv = new AutoRoot*[1];
		JS_SetContextThread(ScriptEngine::GetGlobalContext());
		argv[0] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(ScriptEngine::GetGlobalContext(), (char*)argv)));
		JS_ClearContextThread(ScriptEngine::GetGlobalContext());
		script->ExecEventAsync("gamemsg", 1, argv);
	}
	return true;
}

void GameMsgEvent(char* lpszMsg)
{
	ScriptEngine::ForEachScript(GameEventCallback, lpszMsg, 1);
}

bool __fastcall ItemEventCallback(Script* script, void* argv, uint argc)
{
	ItemEventHelper* helper = (ItemEventHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("itemaction"))
	{
		AutoRoot** argv = new AutoRoot*[4];
		JS_SetContextThread(ScriptEngine::GetGlobalContext());
		argv[0] = new AutoRoot();
		argv[1] = new AutoRoot();
		JS_NewNumberValue(ScriptEngine::GetGlobalContext(), helper->id, argv[0]->value());
		JS_NewNumberValue(ScriptEngine::GetGlobalContext(), helper->mode, argv[1]->value());
		argv[2] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(ScriptEngine::GetGlobalContext(), helper->code)));
		argv[3] = new AutoRoot(BOOLEAN_TO_JSVAL(helper->global));
		JS_ClearContextThread(ScriptEngine::GetGlobalContext());
		script->ExecEventAsync("itemaction", 4, argv);
	}
	return true;
}

void ItemActionEvent(DWORD GID, char* Code, BYTE Mode, bool Global)
{
	ItemEventHelper helper = {GID, Code, Mode, Global};
	ScriptEngine::ForEachScript(ItemEventCallback, &helper, 1);
}

bool __fastcall GameActionEventCallback(Script* script, void* argv, uint argc)
{
	GameActionEventHelper* helper = (GameActionEventHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("gameevent"))
	{
		AutoRoot** argv = new AutoRoot*[5];
		JS_SetContextThread(ScriptEngine::GetGlobalContext());
		argv[0] = new AutoRoot();
		argv[1] = new AutoRoot();
		argv[2] = new AutoRoot();
		JS_NewNumberValue(ScriptEngine::GetGlobalContext(), helper->mode, argv[0]->value());
		JS_NewNumberValue(ScriptEngine::GetGlobalContext(), helper->param1, argv[1]->value());
		JS_NewNumberValue(ScriptEngine::GetGlobalContext(), helper->param2, argv[2]->value());
		argv[3] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(ScriptEngine::GetGlobalContext(), helper->name1)));
		argv[4] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(ScriptEngine::GetGlobalContext(), helper->name2)));
		JS_ClearContextThread(ScriptEngine::GetGlobalContext());
		script->ExecEventAsync("gameevent", 5, argv);
	}
	return true;
}

void GameActionEvent(BYTE mode, DWORD param1, DWORD param2, char* name1, char* name2)
{
	GameActionEventHelper helper = {mode, param1, param2, name1, name2};
	ScriptEngine::ForEachScript(GameActionEventCallback, &helper, 1);
}
