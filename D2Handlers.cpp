#include <vector>
#include <algorithm>

#include "D2Handlers.h"
#include "D2NetHandlers.h"
#include "Script.h"
#include "ScreenHook.h"
#include "Unit.h"
#include "Helpers.h"
#include "Core.h"
#include "Constants.h"
#include "Events.h"
#include "ScriptEngine.h"
#include "Console.h"
#include "D2BS.h"
#include "MapHeader.h"
#include "Offset.h"

using namespace std;

bool __fastcall UpdatePlayerGid(Script* script, void*, uint) { script->UpdatePlayerGid(); return true; }

DWORD WINAPI D2Thread(LPVOID lpParam)
{
	bool beginStarter = true;
	bool bInGame = false;
	POINT pMouse = {0,0};
	int mouseEventMod = 0;
	Vars.bUseRawCDKey = 0; 
	InitSettings();
	if(InitHooks())
	{
		Log("D2BS Engine startup complete. %s", D2BS_VERSION);
		Print("ÿc2D2BSÿc0 :: Engine startup complete!");
	}
	else
	{
		Log("D2BS Engine startup failed.");
		Print("ÿc2D2BSÿc0 :: Engine startup failed!");
		return FALSE;
	}

	while(Vars.bActive)
	{
		switch(ClientState())
		{
			case ClientStateInGame:
			{
				if(bInGame)
				{
					if((Vars.dwMaxGameTime && Vars.dwGameTime && 
							(GetTickCount() - Vars.dwGameTime) > Vars.dwMaxGameTime) ||
							(!D2COMMON_IsTownByLevelNo(GetPlayerArea()) &&
							(Vars.nChickenHP && Vars.nChickenHP >= GetUnitHP(D2CLIENT_GetPlayerUnit())) ||
							(Vars.nChickenMP && Vars.nChickenMP >= GetUnitMP(D2CLIENT_GetPlayerUnit()))))
					D2CLIENT_ExitGame();
				}
				else
				{
					Sleep(500);
					
					Vars.dwGameTime = GetTickCount();
					D2CLIENT_InitInventory();
					ScriptEngine::ForEachScript(UpdatePlayerGid, NULL, 0);
					ScriptEngine::UpdateConsole();

					GameJoined();

					bInGame = true;
				}
				break;
			}
			case ClientStateMenu:
			{
				while(Vars.bUseProfileScript){
					Sleep(100);
				}
				MenuEntered(beginStarter);
				beginStarter = false;
				if(bInGame)
				{
					Vars.dwGameTime = NULL;
					bInGame = false;
				}
				break;
			}
			case ClientStateBusy:
			case ClientStateNull:
				break;
		}
		Sleep(50);			
	
	}

	ScriptEngine::Shutdown();

	return NULL;
}

DWORD __fastcall GameInput(wchar_t* wMsg)
{
	if(Vars.bDontCatchNextMsg)
	{
		Vars.bDontCatchNextMsg = FALSE;
		return 0;
	}

	bool result = false;

	if(wMsg[0] == L'.')
	{
		char* szBuffer = UnicodeToAnsi(wMsg);
		result = ProcessCommand(szBuffer+1, false);
		delete[] szBuffer;
	}

	return result == true ? -1 : 0;
}

DWORD __fastcall ChannelInput(wchar_t* wMsg)
{
	if(Vars.bDontCatchNextMsg)
	{
		Vars.bDontCatchNextMsg = FALSE;
		return false;
	}

	bool result = false;
	if(wMsg[0] == L'.')
	{
		char* szBuffer = UnicodeToAnsi(wMsg);
		result = ProcessCommand(szBuffer+1, false);
		//FIXME
		//D2WIN_SetControlText(*p_D2WIN_ChatInputBox, L"");
		delete[] szBuffer;
	}

	// false means ignore, true means send
	return result ? false : true;
}

DWORD __fastcall GamePacketReceived(BYTE* pPacket, DWORD dwSize)
{
	switch(pPacket[0])
	{
		case 0xAE:
			Log("Warden activity detected! Terminating Diablo to ensure your safety:)");
			TerminateProcess(GetCurrentProcess(), 0);
		break;
		case 0x15: return ReassignPlayerHandler(pPacket, dwSize);
		case 0x26: return ChatEventHandler(pPacket, dwSize);
		case 0x2A: return NPCTransactionHandler(pPacket, dwSize);
		case 0x5A: return EventMessagesHandler(pPacket, dwSize);
		case 0x18:
		case 0x95: return HPMPUpdateHandler(pPacket, dwSize);
		case 0x9C:
		case 0x9D: return ItemActionHandler(pPacket, dwSize);
		case 0xA7: return DelayedStateHandler(pPacket, dwSize);
	}

	return TRUE;
}

LONG WINAPI GameEventHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	COPYDATASTRUCT* pCopy;
	switch(uMsg)
	{
		case WM_COPYDATA:
			pCopy = (COPYDATASTRUCT*)lParam;

			if(pCopy)
			{
				if(pCopy->dwData == 0x1337) // 0x1337 = Execute Script
				{
					while(!Vars.bActive || (ScriptEngine::GetState() != Running))
					{
						Sleep(100);
					}
					ScriptEngine::RunCommand((char*)pCopy->lpData);
				}
				else if(pCopy->dwData == 0x31337) // 0x31337 = Set Profile
				{
					const char* profile = (char*)pCopy->lpData;
					if(SwitchToProfile(profile))
						Print("ÿc2D2BSÿc0 :: Switched to profile %s", profile);
					else
						Print("ÿc2D2BSÿc0 :: Profile %s not found", profile);
				}
				else if(pCopy->dwData == 0xDEAD)
				{	
					Vars.bUseRawCDKey = 1; 
					InstallConditional();
					const char *keys = (char*)pCopy->lpData;
					int len = (strchr(keys,'|')-keys)*sizeof(char);
					strncpy(Vars.szClassic, keys, len);
					strcpy(Vars.szLod, keys+len+1);
				}
				else CopyDataEvent(pCopy->dwData, (char*)pCopy->lpData);
			}
			return TRUE;
	}

	return (LONG)CallWindowProcA(Vars.oldWNDPROC, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK KeyPress(int code, WPARAM wParam, LPARAM lParam)
{
	if(code >= HC_ACTION)
	{
		WORD repeatCount = LOWORD(lParam);
		bool altState = !!(HIWORD(lParam) & KF_ALTDOWN);
		bool previousState = !!(HIWORD(lParam) & KF_REPEAT);
		bool transitionState = !!(HIWORD(lParam) & KF_UP);
		bool isRepeat = !transitionState && repeatCount != 1;
		bool isDown = !(previousState && transitionState);
		bool isUp = previousState && transitionState;

		bool gameState = ClientState() == ClientStateInGame;
		bool chatBoxOpen = gameState ? !!D2CLIENT_GetUIState(UI_CHAT_CONSOLE) : false;
		bool escMenuOpen = gameState ? !!D2CLIENT_GetUIState(UI_ESCMENU_MAIN) : false;

		if (altState && wParam == VK_F4)
			return CallNextHookEx(NULL, code, wParam, lParam);

		if(Vars.bBlockKeys)
			return 1;
		
		if(wParam == VK_HOME && !(chatBoxOpen || escMenuOpen))
		{
			if(isDown && !isRepeat && code == HC_ACTION)
			{
				if(!altState)
					Console::ToggleBuffer();
				else
					Console::TogglePrompt();

				return CallNextHookEx(NULL, code, wParam, lParam);
			}
		}			
		else if(wParam == VK_ESCAPE && Console::IsVisible())
		{
			if(isDown && !isRepeat && code == HC_ACTION )
			{
				Console::Hide();
				return 1;
			}
			return CallNextHookEx(NULL, code, wParam, lParam);
		}
		else if(Console::IsEnabled())
		{
			BYTE layout[256] = {0};
			WORD out[2] = {0};
			switch(wParam)
			{
				case VK_TAB:
					if(isUp)
						for(int i = 0; i < 5; i++)
							Console::AddKey(' ');
					break;
				case VK_RETURN:
					if(isUp && !isRepeat && !escMenuOpen)
						Console::ExecuteCommand();
					break;
				case VK_BACK:
					if(isDown)
						Console::RemoveLastKey();
					break;
				case VK_UP:
					if(isUp && !isRepeat)
						Console::PrevCommand();
					break;
				case VK_DOWN:
					if(isUp && !isRepeat)
						Console::NextCommand();
					break;
				case VK_NEXT:
					if(isDown)
						Console::ScrollDown();
					break;
				case VK_PRIOR:
					if(isDown )
						Console::ScrollUp();
					break;
				case VK_MENU: // alt
					// Send the alt to the scripts to fix sticky alt. There may be a better way.
					KeyDownUpEvent(wParam, isUp);
					return CallNextHookEx(NULL, code, wParam, lParam);
					break;
				default:
					if(isDown)
					{
						if(GetKeyboardState(layout) && ToAscii(wParam, (lParam & 0xFF0000), layout, out, 0) != 0)
						{
							for(int i = 0; i < repeatCount; i++)
								Console::AddKey(out[0]);
						}
					}
					break;
			}			
			return 1;
		}
		else if(code == HC_ACTION && !isRepeat && !(chatBoxOpen || escMenuOpen))
			if(KeyDownUpEvent(wParam, isUp))
				return 1;
	}
	return CallNextHookEx(NULL, code, wParam, lParam);
}

LRESULT CALLBACK MouseMove(int code, WPARAM wParam, LPARAM lParam)
{
	MOUSEHOOKSTRUCT* mouse = (MOUSEHOOKSTRUCT*)lParam;
	POINT pt = mouse->pt;
	ScreenToClient(mouse->hwnd, &pt);
	
	// filter out clicks on the window border
	if(code == HC_ACTION && (pt.x < 0 || pt.y < 0))
		return CallNextHookEx(NULL, code, wParam, lParam);
	Vars.pMouseCoords = pt;
	if(Vars.bBlockMouse)
		return 1;

	if(code == HC_ACTION)
	{
		bool clicked = false;

		HookClickHelper helper = {-1, {pt.x, pt.y}};
		switch(wParam)
		{
			case WM_LBUTTONDOWN:
				MouseClickEvent(0, pt, false);
				helper.button = 0;
				if(Genhook::ForEachVisibleHook(ClickHook, &helper, 1))
					clicked = true;
				break;
			case WM_LBUTTONUP:
				MouseClickEvent(0, pt, true);
				break;
			case WM_RBUTTONDOWN:
				MouseClickEvent(1, pt, false);
				helper.button = 1;
				if(Genhook::ForEachVisibleHook(ClickHook, &helper, 1))
					clicked = true;
				break;
			case WM_RBUTTONUP:
				MouseClickEvent(1, pt, true);
				break;
			case WM_MBUTTONDOWN:
				MouseClickEvent(2, pt, false);
				helper.button = 2;
				if(Genhook::ForEachVisibleHook(ClickHook, &helper, 1))
					clicked = true;
				break;
			case WM_MBUTTONUP:
				MouseClickEvent(2, pt, true);
				break;
			case WM_MOUSEMOVE:
				// would be nice to enable these events but they bog down too much
				MouseMoveEvent(pt);
				//Genhook::ForEachVisibleHook(HoverHook, &helper, 1);
				break;
		}

		return clicked ? 1 : CallNextHookEx(NULL, code, wParam, lParam);
	}

	return CallNextHookEx(NULL, code, wParam, lParam);
}

void GameDraw(void)
{
	if(Vars.bActive && ClientState() == ClientStateInGame)
	{
		Genhook::DrawAll(IG);
		DrawLogo();
		Console::Draw();
	}
	Sleep(10);
}

void GameDrawOOG(void)
{
	D2WIN_DrawSprites();
	if(Vars.bActive && ClientState() == ClientStateMenu)
	{
		Genhook::DrawAll(OOG);
		DrawLogo();
		Console::Draw();
	}
	Sleep(10);
}

void __stdcall AddUnit(UnitAny* lpUnit)
{
//	EnterCriticalSection(&Vars.cUnitListSection);
//	Vars.vUnitList.push_back(make_pair<DWORD, DWORD>(lpUnit->dwUnitId, lpUnit->dwType));
//	LeaveCriticalSection(&Vars.cUnitListSection);
}

void __stdcall RemoveUnit(UnitAny* lpUnit)
{
//	EnterCriticalSection(&Vars.cUnitListSection);
	// no need to check the return--it has to be there or the real game would have bigger issues with it
//	for(vector<pair<DWORD, DWORD> >::iterator it = Vars.vUnitList.begin(); it != Vars.vUnitList.end(); it++)
//	{
//		if(it->first == lpUnit->dwUnitId && it->second == lpUnit->dwType)
//		{
//			Vars.vUnitList.erase(it);
//			break;
//		}
//	}
//	LeaveCriticalSection(&Vars.cUnitListSection);
}

void __fastcall WhisperHandler(char* szAcc, char* szText)
{
	if(!Vars.bDontCatchNextMsg)
		WhisperEvent(szAcc, szText);
	else
		Vars.bDontCatchNextMsg = FALSE;
}

void __fastcall ChannelWhisperHandler(char* szAcc, char* szText)
{
	if(!Vars.bDontCatchNextMsg)
		WhisperEvent(szAcc, szText);
	else
		Vars.bDontCatchNextMsg = FALSE;
}

void __fastcall ChannelChatHandler(char* szAcc, char* szText)
{
	if(!Vars.bDontCatchNextMsg)
		ChatEvent(szAcc, szText);
	else
		Vars.bDontCatchNextMsg = FALSE;
}

DWORD __fastcall GameAttack(UnitInteraction* pAttack)
{
	if(!pAttack || !pAttack->lpTargetUnit || pAttack->lpTargetUnit->dwType != UNIT_MONSTER)
		return (DWORD)-1;

	if(pAttack->dwMoveType == ATTACKTYPE_UNITLEFT)
		pAttack->dwMoveType = ATTACKTYPE_SHIFTLEFT;

	if(pAttack->dwMoveType == ATTACKTYPE_RIGHT)
		pAttack->dwMoveType = ATTACKTYPE_SHIFTRIGHT;

	return NULL;
}

void __fastcall GamePlayerAssignment(UnitAny* pPlayer)
{
	if(!pPlayer)
		return;

	PlayerAssignEvent(pPlayer->dwUnitId);
}

void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	if(Vars.bGameLoopEntered)
		LeaveCriticalSection(&Vars.cGameLoopSection);
	else
		Vars.bGameLoopEntered = true;

	while(Vars.SectionCount)
		Sleep(0);

	EnterCriticalSection(&Vars.cGameLoopSection);	
}


void GameLeave(void)
{
	if(Vars.bGameLoopEntered)
		LeaveCriticalSection(&Vars.cGameLoopSection);
	else
		Vars.bGameLoopEntered = true;

	ScriptEngine::ForEachScript(StopIngameScript, NULL, 0);
	ActMap::ClearCache();

	EnterCriticalSection(&Vars.cGameLoopSection);
}