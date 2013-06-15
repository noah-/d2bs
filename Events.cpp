#include "ScriptEngine.h"
#include "D2BS.h"



bool __fastcall LifeEventCallback(Script* script, void* argv, uint argc)
{
	SingleArgHelper* helper = (SingleArgHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("melife"))
	{
		Event* evt = new Event;
		evt->owner = script;
		evt->argc = argc;
		evt->name = strdup("melife");
		evt->arg1 =  new DWORD(helper->arg1);
 		
		script->FireEvent(evt);
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
		Event* evt = new Event;
		evt->owner = script;
		evt->argc = argc;
		evt->name = strdup("memana");
		evt->arg1 =  new DWORD(helper->arg1);
 		
		script->FireEvent(evt);
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
		Event* evt = new Event;
		evt->owner = script;
		evt->argc = argc;
		evt->name = strdup(event); 
		evt->arg1 =  new DWORD((DWORD)helper->key);
 		
		script->FireEvent(evt);
	}
	bool block = false;
	event = (helper->up ? "keyupblocker" : "keydownblocker");
	if(script->IsRunning() && script->IsListenerRegistered(event))
	{
		Event* evt = new Event;
		evt->owner = script;
		evt->argc = argc;
		evt->name = strdup(event); 
		evt->arg1 =  new DWORD((DWORD)helper->key);
		evt->arg5 =  CreateEvent(nullptr, false, false, nullptr);

		script->FireEvent(evt);
		if(WaitForSingleObjectEx(evt->arg5, 1000, false) == WAIT_TIMEOUT)
			return false;
		bool* global = (bool*) evt->arg4;
		block = *global;
		delete evt->name;
		delete evt->arg1;
		delete evt;
	}

	return block;
}

bool KeyDownUpEvent(WPARAM key, BYTE bUp)
{
	KeyEventHelper helper = {bUp, key};
	return ScriptEngine::ForEachScript(KeyEventCallback, &helper, 2);
}

bool __fastcall PlayerAssignCallback(Script* script, void* argv, uint argc)
{
	SingleArgHelper* helper = (SingleArgHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("playerassign"))
	{
		Event* evt = new Event;
		evt->owner = script;
		evt->argc = argc;
		evt->name = strdup("playerassign"); 
		evt->arg1 =  new DWORD((DWORD)helper->arg1);
 		
		script->FireEvent(evt);
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
		Event* evt = new Event;
		evt->owner = script;
		evt->argc = argc;
		evt->name = "mouseclick";
		evt->arg1 =  new DWORD(helper->arg1);
 		evt->arg2 =  new DWORD(helper->arg2);
		evt->arg3 =  new DWORD(helper->arg3);
 		evt->arg4 =  new DWORD(helper->arg4);
		
		script->FireEvent(evt);
	}
	return true;
}

void MouseClickEvent(int button, POINT pt, bool bUp)
{
	QuadArgHelper helper = {button, pt.x, pt.y, bUp};
	ScriptEngine::ForEachScript(MouseClickCallback, &helper, 4);
}

bool __fastcall MouseMoveCallback(Script* script, void* argv, uint argc)
{
	DoubleArgHelper* helper = (DoubleArgHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("mousemove"))
	{
		Event* evt = new Event;
		evt->owner = script;
		evt->argc = argc;
		evt->name = "mousemove";
		evt->arg1 =  new DWORD(helper->arg1);
 		evt->arg2 =  new DWORD(helper->arg2);
		
		script->FireEvent(evt);
	}
	return true;
}

void MouseMoveEvent(POINT pt)
{
	if (pt.x < 1 || pt.y < 1)
		return;
	DoubleArgHelper helper = {pt.x, pt.y};
	ScriptEngine::ForEachScript(MouseMoveCallback, &helper, 2);
}

bool __fastcall BCastEventCallback(Script* script, void* argv, uint argc) 
{
	BCastEventHelper* helper = (BCastEventHelper*)argv;
	
	if(script->IsRunning() && script->IsListenerRegistered("scriptmsg"))
	{
		Event* evt = new Event;
		evt->owner = script;
		evt->argc = argc;
		evt->arg1 = new DWORD(argc);
		evt->name ="scriptmsg"; 
		evt->argv = new JSAutoStructuredCloneBuffer*;
 		for(uintN i = 0; i < argc; i++)
		{
			evt->argv[i] = new JSAutoStructuredCloneBuffer;
			evt->argv[i]->write(helper->cx, helper->argv[i]);
		}
		
		script->FireEvent(evt);
	}
	return true;
}

void ScriptBroadcastEvent(JSContext* cx ,uint argc, jsval* args)
{
	BCastEventHelper helper = {cx, args, argc};
	ScriptEngine::ForEachScript(BCastEventCallback, &helper, argc);
}



bool __fastcall ChatEventCallback(Script* script, void* argv, uint argc)
{
	ChatEventHelper* helper = (ChatEventHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered(helper->event))
	{
		Event* evt = new Event;
		evt->owner = script;
		evt->argc = argc;
		evt->name = strdup(helper->event); 
		evt->arg1 = strdup(helper->nick);
 		evt->arg2 = strdup(helper->msg);
		
		script->FireEvent(evt);
		
	}
	std::string evtname = helper->event;
	evtname = evtname + "blocker";
	bool block = false;

	if(script->IsRunning() && script->IsListenerRegistered(evtname.c_str()))
	{
		Event* evt = new Event;
		evt->owner = script;
		evt->argc = argc;
		evt->name = strdup(evtname.c_str()); 
		evt->arg1 = strdup(helper->nick);
 		evt->arg2 = strdup(helper->msg);
		
		evt->arg5 =  CreateEvent(nullptr, false, false, nullptr);

		script->FireEvent(evt);
		if(WaitForSingleObjectEx(evt->arg5, 5000, true) == WAIT_TIMEOUT)
			return false;
		block = (bool*) evt->arg4;
		delete evt->name;
		delete evt->arg1;
		delete evt->arg2;
		delete evt;

	}
	return block;
}

bool ChatEvent(char* lpszNick, char* lpszMsg)
{
	ChatEventHelper helper = {"chatmsg", lpszNick, lpszMsg};
	return ScriptEngine::ForEachScript(ChatEventCallback, &helper, 2);
}

bool WhisperEvent(char* lpszNick, char* lpszMsg)
{
	ChatEventHelper helper = {"whispermsg", lpszNick, lpszMsg};
	return ScriptEngine::ForEachScript(ChatEventCallback, &helper, 2);
}

bool __fastcall CopyDataCallback(Script* script, void* argv, uint argc)
{
	CopyDataHelper* helper = (CopyDataHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("copydata"))
	{
		Event* evt = new Event;
		evt->owner = script;
		evt->argc = argc;
		evt->name = "copydata";
		evt->arg1 =  new DWORD(helper->mode);
 		evt->arg2 = strdup(helper->msg);
		
		script->FireEvent(evt);
	}
	return true;
}

void CopyDataEvent(DWORD dwMode, char* lpszMsg)
{
	CopyDataHelper helper = {dwMode, lpszMsg};
	ScriptEngine::ForEachScript(CopyDataCallback, &helper, 1);
}

bool __fastcall ItemEventCallback(Script* script, void* argv, uint argc)
{
	ItemEventHelper* helper = (ItemEventHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("itemaction"))
	{
		Event* evt = new Event;
		evt->owner = script;
		evt->argc = argc;
		evt->name = "itemaction";
		evt->arg1 =  new DWORD(helper->id);
 		evt->arg2 = strdup(helper->code);
		evt->arg3 = new DWORD(helper->mode);
		evt->arg4 = new bool(helper->global);

		script->FireEvent(evt);
	}
	return true;
}

void ItemActionEvent(DWORD GID, char* Code, BYTE Mode, bool Global)
{
	ItemEventHelper helper = {GID, Code, Mode, Global};
	ScriptEngine::ForEachScript(ItemEventCallback, &helper, 4);
}

bool __fastcall GameActionEventCallback(Script* script, void* argv, uint argc)
{
	GameActionEventHelper* helper = (GameActionEventHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("gameevent"))
	{
		Event* evt = new Event;
		evt->owner = script;
		evt->argc = argc;
		evt->name = "gameevent";
		evt->arg1 = new BYTE(helper->mode);
		evt->arg2 = new DWORD(helper->param1);
		evt->arg3 = new DWORD(helper->param2);
		evt->arg4 = strdup(helper->name1);
 		evt->arg5 = strdup(helper->name2);

		script->FireEvent(evt);
	}
	return true;
}

void GameActionEvent(BYTE mode, DWORD param1, DWORD param2, char* name1, char* name2)
{
	GameActionEventHelper helper = {mode, param1, param2, name1, name2};
	ScriptEngine::ForEachScript(GameActionEventCallback, &helper, 5);
}
bool __fastcall GamePacketCallback(Script* script, void* argv, uint argc)
{
	GamePacketHelper* helper = (GamePacketHelper*)argv;
	
	if(script->IsRunning() && script->IsListenerRegistered("gamepacket"))
	{
		Event* evt = new Event;
		evt->owner = script;
		evt->argc = argc;
		evt->arg2 = new DWORD(helper->dwSize);
		evt->arg1 = new BYTE[helper->dwSize];
		memcpy(evt->arg1, helper->pPacket, helper->dwSize);
		evt->name = "gamepacket";
		evt->arg5 =  CreateEvent(nullptr, false, false, nullptr);

		script->FireEvent(evt);
		if(WaitForSingleObjectEx(evt->arg5, 5000, true) == WAIT_TIMEOUT)
			return false;
		if (*(DWORD*) evt->arg4 )
		{
			delete evt->arg1;
			delete evt->arg2;
			delete evt->arg4;
			CloseHandle(evt->arg5);
			delete evt;
			return true;
		}
		else 
		{
			delete evt->arg1;
			delete evt->arg2;
			delete evt->arg4;
			CloseHandle(evt->arg5);
			delete evt;
		}
	}
	return false;
}

bool GamePacketEvent(BYTE* pPacket, DWORD dwSize)
{
	GamePacketHelper helper = {pPacket, dwSize};
	return ScriptEngine::ForEachScript(GamePacketCallback, &helper, 2);
}
