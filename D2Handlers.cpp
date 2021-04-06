#include <vector>
#include <algorithm>
#include <string>

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
#include "CommandLine.h"
#include "Control.h"

using namespace std;

bool __fastcall UpdatePlayerGid(Script* script, void*, uint) {
    script->UpdatePlayerGid();
    return true;
}

DWORD WINAPI D2Thread(LPVOID lpParam) {
    sLine* command;
    bool beginStarter = true;
    bool bInGame = false;

    if (!InitHooks()) {
        wcscpy_s(Vars.szPath, MAX_PATH, L"common");
        Log(L"D2BS Engine startup failed. %s", Vars.szCommandLine);
        Print(L"\u00FFc2D2BS\u00FFc0 :: Engine startup failed!");
        return FALSE;
    }

    ParseCommandLine(Vars.szCommandLine);

    command = GetCommand(L"-handle");

    if (command) {
        Vars.hHandle = (HWND)_wtoi(command->szText);
    }

    command = GetCommand(L"-mpq");

    if (command) {
        char* mpq = UnicodeToAnsi(command->szText);
        LoadMPQ(mpq);
        delete[] mpq;
    }

    command = GetCommand(L"-profile");

    if (command) {
        const wchar_t* profile = command->szText;
        if (SwitchToProfile(profile))
            Print(L"\u00FFc2D2BS\u00FFc0 :: Switched to profile %s", profile);
        else
            Print(L"\u00FFc2D2BS\u00FFc0 :: Profile %s not found", profile);
    }

    Log(L"D2BS Engine startup complete. %s", D2BS_VERSION);
    Print(L"\u00FFc2D2BS\u00FFc0 :: Engine startup complete!");

    while (Vars.bActive) {
        switch (ClientState()) {
        case ClientStateInGame: {
            if (bInGame) {
                if ((Vars.dwMaxGameTime && Vars.dwGameTime && (GetTickCount() - Vars.dwGameTime) > Vars.dwMaxGameTime) ||
                    (!D2COMMON_IsTownByLevelNo(GetPlayerArea()) && (Vars.nChickenHP && Vars.nChickenHP >= GetUnitHP(D2CLIENT_GetPlayerUnit())) ||
                     (Vars.nChickenMP && Vars.nChickenMP >= GetUnitMP(D2CLIENT_GetPlayerUnit()))))
                    D2CLIENT_ExitGame();
            } else {
                Vars.dwGameTime = GetTickCount();

                Sleep(500);

                D2CLIENT_InitInventory();
                ScriptEngine::ForEachScript(UpdatePlayerGid, NULL, 0);
                ScriptEngine::UpdateConsole();
                Vars.bQuitting = false;
                GameJoined();

                bInGame = true;
            }
            break;
        }
        case ClientStateMenu: {
            while (Vars.bUseProfileScript) {
                Sleep(100);
            }
            MenuEntered(beginStarter);
            beginStarter = false;
            if (bInGame) {
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
        /*	EnterCriticalSection(&ScriptEngine::lock);
                for(ScriptMap::iterator it = ScriptEngine::scripts.begin(); it != ScriptEngine::scripts.end(); it++)
                        JS_TriggerOperationCallback(it->second->GetContext());
                LeaveCriticalSection(&ScriptEngine::lock);  */
    }

    ScriptEngine::Shutdown();

    return NULL;
}

DWORD __fastcall GameInput(wchar_t* wMsg) {
    bool send = true;

    if (Vars.bDontCatchNextMsg)
        Vars.bDontCatchNextMsg = false;
    else {
        send = !((wMsg[0] == L'.' && ProcessCommand(wMsg + 1, false)) || ChatInputEvent(wMsg));
    }

    return send ? 0 : -1; // -1 means block, 0 means send
}

DWORD __fastcall ChannelInput(wchar_t* wMsg) {
    bool send = true;

    if (Vars.bDontCatchNextMsg)
        Vars.bDontCatchNextMsg = false;
    else {
        send = !((wMsg[0] == L'.' && ProcessCommand(wMsg + 1, false)) || ChatInputEvent(wMsg));
    }

    return send; // false means ignore, true means send
}

DWORD __fastcall GamePacketReceived(BYTE* pPacket, DWORD dwSize) {
    switch (pPacket[0]) {
    case 0xAE:
        Log(L"Warden activity detected! Terminating Diablo to ensure your safety :)");
        TerminateProcess(GetCurrentProcess(), 0);
        break;
    case 0x15:
        return !GamePacketEvent(pPacket, dwSize) && ReassignPlayerHandler(pPacket, dwSize);
    case 0x26:
        return !GamePacketEvent(pPacket, dwSize) && ChatEventHandler(pPacket, dwSize);
    case 0x2A:
        return !GamePacketEvent(pPacket, dwSize) && NPCTransactionHandler(pPacket, dwSize);
    case 0x5A:
        return !GamePacketEvent(pPacket, dwSize) && EventMessagesHandler(pPacket, dwSize);
    case 0x18:
    case 0x95:
        return !GamePacketEvent(pPacket, dwSize) && HPMPUpdateHandler(pPacket, dwSize);
    case 0x9C:
    case 0x9D:
        return !GamePacketEvent(pPacket, dwSize) && ItemActionHandler(pPacket, dwSize);
    case 0xA7:
        return !GamePacketEvent(pPacket, dwSize) && DelayedStateHandler(pPacket, dwSize);
    }

    return !GamePacketEvent(pPacket, dwSize);
}

DWORD __fastcall GamePacketSent(BYTE* pPacket, DWORD dwSize) {
    return !GamePacketSentEvent(pPacket, dwSize);
}

LONG WINAPI GameEventHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    COPYDATASTRUCT* pCopy;
    switch (uMsg) {
    case WM_COPYDATA:
        pCopy = (COPYDATASTRUCT*)lParam;

        if (pCopy) {
            wchar_t* lpwData = AnsiToUnicode((const char*)pCopy->lpData);
            if (pCopy->dwData == 0x1337) // 0x1337 = Execute Script
            {
                while (!Vars.bActive || (ScriptEngine::GetState() != Running)) {
                    Sleep(100);
                }
                ScriptEngine::RunCommand(lpwData);
            } else if (pCopy->dwData == 0x31337) // 0x31337 = Set Profile
                if (SwitchToProfile(lpwData))
                    Print(L"\u00FFc2D2BS\u00FFc0 :: Switched to profile %s", lpwData);
                else
                    Print(L"\u00FFc2D2BS\u00FFc0 :: Profile %s not found", lpwData);
            else
                CopyDataEvent(pCopy->dwData, lpwData);
            delete[] lpwData;
        }

        return TRUE;
    }

    return (LONG)CallWindowProcA(Vars.oldWNDPROC, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK KeyPress(int code, WPARAM wParam, LPARAM lParam) {
    if (code >= HC_ACTION) {
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

        if (Vars.bBlockKeys)
            return 1;

        if (wParam == VK_HOME && !(chatBoxOpen || escMenuOpen)) {
            if (isDown && !isRepeat && code == HC_ACTION) {
                if (!altState)
                    Console::ToggleBuffer();
                else
                    Console::TogglePrompt();

                return CallNextHookEx(NULL, code, wParam, lParam);
            }
        } else if (wParam == VK_ESCAPE && Console::IsVisible()) {
            if (isDown && !isRepeat && code == HC_ACTION) {
                Console::Hide();
                return 1;
            }
            return CallNextHookEx(NULL, code, wParam, lParam);
        } else if (Console::IsEnabled()) {
            BYTE layout[256] = {0};
            WORD out[2] = {0};
            switch (wParam) {
            case VK_TAB:
                if (isUp)
                    for (int i = 0; i < 5; i++)
                        Console::AddKey(' ');
                break;
            case VK_RETURN:
                if (isUp && !isRepeat && !escMenuOpen)
                    Console::ExecuteCommand();
                break;
            case VK_BACK:
                if (isDown)
                    Console::RemoveLastKey();
                break;
            case VK_UP:
                if (isUp && !isRepeat)
                    Console::PrevCommand();
                break;
            case VK_DOWN:
                if (isUp && !isRepeat)
                    Console::NextCommand();
                break;
            case VK_NEXT:
                if (isDown)
                    Console::ScrollDown();
                break;
            case VK_PRIOR:
                if (isDown)
                    Console::ScrollUp();
                break;
            case VK_MENU: // alt
                // Send the alt to the scripts to fix sticky alt. There may be a better way.
                KeyDownUpEvent(wParam, isUp);
                return CallNextHookEx(NULL, code, wParam, lParam);
                break;
            default:
                if (isDown) {
                    if (GetKeyboardState(layout) && ToAscii(wParam, (lParam & 0xFF0000), layout, out, 0) != 0) {
                        for (int i = 0; i < repeatCount; i++)
                            Console::AddKey(out[0]);
                    }
                }
                break;
            }
            return 1;
        } else if (code == HC_ACTION && !isRepeat && !(chatBoxOpen || escMenuOpen))
            if (KeyDownUpEvent(wParam, isUp))
                return 1;
    }
    return CallNextHookEx(NULL, code, wParam, lParam);
}

LRESULT CALLBACK MouseMove(int code, WPARAM wParam, LPARAM lParam) {
    MOUSEHOOKSTRUCT* mouse = (MOUSEHOOKSTRUCT*)lParam;
    POINT pt = mouse->pt;
    ScreenToClient(mouse->hwnd, &pt);

    // filter out clicks on the window border
    if (code == HC_ACTION && (pt.x < 0 || pt.y < 0))
        return CallNextHookEx(NULL, code, wParam, lParam);
    Vars.pMouseCoords = pt;
    if (Vars.bBlockMouse)
        return 1;

    if (code == HC_ACTION) {
        bool clicked = false;

        HookClickHelper helper = {-1, {pt.x, pt.y}};
        switch (wParam) {
        case WM_LBUTTONDOWN:
            MouseClickEvent(0, pt, false);
            helper.button = 0;
            if (Genhook::ForEachVisibleHook(ClickHook, &helper, 1))
                clicked = true;
            break;
        case WM_LBUTTONUP:
            MouseClickEvent(0, pt, true);
            break;
        case WM_RBUTTONDOWN:
            MouseClickEvent(1, pt, false);
            helper.button = 1;
            if (Genhook::ForEachVisibleHook(ClickHook, &helper, 1))
                clicked = true;
            break;
        case WM_RBUTTONUP:
            MouseClickEvent(1, pt, true);
            break;
        case WM_MBUTTONDOWN:
            MouseClickEvent(2, pt, false);
            helper.button = 2;
            if (Genhook::ForEachVisibleHook(ClickHook, &helper, 1))
                clicked = true;
            break;
        case WM_MBUTTONUP:
            MouseClickEvent(2, pt, true);
            break;
        case WM_MOUSEMOVE:
            // would be nice to enable these events but they bog down too much
            MouseMoveEvent(pt);
            // Genhook::ForEachVisibleHook(HoverHook, &helper, 1);
            break;
        }

        return clicked ? 1 : CallNextHookEx(NULL, code, wParam, lParam);
    }

    return CallNextHookEx(NULL, code, wParam, lParam);
}

void FlushPrint() {
    if (!TryEnterCriticalSection(&Vars.cPrintSection))
        return;

    if (Vars.qPrintBuffer.empty()) {
        LeaveCriticalSection(&Vars.cPrintSection);
        return;
    }

    std::queue<std::wstring> clean;
    std::swap(Vars.qPrintBuffer, clean);
    LeaveCriticalSection(&Vars.cPrintSection);

    while (!clean.empty()) {
        std::wstring str = clean.front();

        // Break into lines through \n.
        list<wstring> lines;
        wstring temp;
        wstringstream ss(str);

        if (Vars.bUseGamePrint && ClientState() == ClientStateInGame) {
            while (getline(ss, temp)) {
                SplitLines(temp.c_str(), Console::MaxWidth() - 100, L' ', lines);
                Console::AddLine(temp);
            }

            // Convert and send every line.
            for (list<wstring>::iterator it = lines.begin(); it != lines.end(); ++it) {
                D2CLIENT_PrintGameString((wchar_t*)it->c_str(), 0);
            }
            /*} else if (Vars.bUseGamePrint && ClientState() == ClientStateMenu && findControl(4, (const wchar_t*)NULL, -1, 28, 410, 354, 298)) {
                while (getline(ss, temp))
                    SplitLines(temp, Console::MaxWidth() - 100, ' ', lines);
                    // TODO: Double check this function, make sure it is working as intended.
                    for (list<string>::iterator it = lines.begin(); it != lines.end(); ++it)
                        D2MULTI_PrintChannelText((char*)it->c_str(), 0);*/
        } else {
            while (getline(ss, temp))
                Console::AddLine(temp);
        }

        clean.pop();
    }
}

void GameDraw(void) {
    if (Vars.bActive && ClientState() == ClientStateInGame) {
        FlushPrint();
        Genhook::DrawAll(IG);
        DrawLogo();
        Console::Draw();
    }
    if (Vars.bTakeScreenshot) {
        Vars.bTakeScreenshot = false;
        D2WIN_TakeScreenshot();
    }
    if (Vars.SectionCount) {
        if (Vars.bGameLoopEntered)
            LeaveCriticalSection(&Vars.cGameLoopSection);
        else
            Vars.bGameLoopEntered = true;
        Sleep(0);
        EnterCriticalSection(&Vars.cGameLoopSection);
    } else
        Sleep(10);
}

void GameDrawOOG(void) {
    D2WIN_DrawSprites();
    if (Vars.bActive && ClientState() == ClientStateMenu) {
        FlushPrint();
        Genhook::DrawAll(OOG);
        DrawLogo();
        Console::Draw();
    }
    if (Vars.bTakeScreenshot) {
        Vars.bTakeScreenshot = false;
        D2WIN_TakeScreenshot();
    }
    Sleep(10);
}

void SetMaxDiff(void) {
    if (D2CLIENT_GetDifficulty() == 1 && *p_D2CLIENT_ExpCharFlag) {
        BnetData* pData = *p_D2LAUNCH_BnData;
        if (pData)
            pData->nMaxDiff = 10;
    }
}

void __stdcall AddUnit(UnitAny* lpUnit) {
    //	EnterCriticalSection(&Vars.cUnitListSection);
    //	Vars.vUnitList.push_back(make_pair<DWORD, DWORD>(lpUnit->dwUnitId, lpUnit->dwType));
    //	LeaveCriticalSection(&Vars.cUnitListSection);
}

void __stdcall RemoveUnit(UnitAny* lpUnit) {
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

void __fastcall WhisperHandler(char* szAcc, char* szText) {
    if (!Vars.bDontCatchNextMsg) {
        wchar_t* szwText = AnsiToUnicode(szText, CP_ACP);
        WhisperEvent(szAcc, szwText);
        delete[] szwText;
    } else
        Vars.bDontCatchNextMsg = FALSE;
}

DWORD __fastcall GameAttack(UnitInteraction* pAttack) {
    if (!pAttack || !pAttack->lpTargetUnit || pAttack->lpTargetUnit->dwType != UNIT_MONSTER)
        return (DWORD)-1;

    if (pAttack->dwMoveType == ATTACKTYPE_UNITLEFT)
        pAttack->dwMoveType = ATTACKTYPE_SHIFTLEFT;

    if (pAttack->dwMoveType == ATTACKTYPE_RIGHT)
        pAttack->dwMoveType = ATTACKTYPE_SHIFTRIGHT;

    return NULL;
}

void __fastcall GamePlayerAssignment(UnitAny* pPlayer) {
    if (!pPlayer)
        return;

    PlayerAssignEvent(pPlayer->dwUnitId);
}

void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    if (Vars.bGameLoopEntered)
        LeaveCriticalSection(&Vars.cGameLoopSection);
    else {
        Vars.bGameLoopEntered = true;
        Vars.dwGameThreadId = GetCurrentThreadId();
    }
    if (Vars.SectionCount)
        Sleep(5);

    EnterCriticalSection(&Vars.cGameLoopSection);
}

void GameLeave(void) {
    //	if(Vars.bGameLoopEntered)
    //	LeaveCriticalSection(&Vars.cGameLoopSection);
    //	else
    //		Vars.bGameLoopEntered = true;

    /*EnterCriticalSection(&ScriptEngine::lock);
    std::vector<Script*> list;
    for(ScriptMap::iterator it = ScriptEngine::scripts.begin(); it != ScriptEngine::scripts.end(); it++)
            if(it->second->GetState() == InGame)
                    it->second->Stop(true);

    LeaveCriticalSection(&ScriptEngine::lock); */
    Vars.bQuitting = false;
    ScriptEngine::ForEachScript(StopIngameScript, NULL, 0);
    ActMap::ClearCache();

    //	EnterCriticalSection(&Vars.cGameLoopSection);
}

BOOL __fastcall RealmPacketRecv(BYTE* pPacket, DWORD dwSize) {
    return !RealmPacketEvent(pPacket, dwSize);
}

BOOL __fastcall ChatPacketRecv(BYTE* pPacket, int len) {
    bool blockPacket = false;

    if (pPacket[1] == 0xF) {
        DWORD mode = pPacket[4];
        char* who = (char*)pPacket + 28;
        char* said = (char*)pPacket + 29 + strlen(who);
        wchar_t* wsaid = AnsiToUnicode(said, CP_ACP);

        switch (pPacket[4]) {
        case 0x02: // channel join
            ChatEvent(who, L"joined the channel");
            break;
        case 0x03: // channel leave
            ChatEvent(who, L"left the channel");
            break;
        case 0x04: // whispers
        case 0x0A:
            WhisperEvent(who, wsaid);
            break;
        case 0x05: // normal text
        case 0x12: // info blue text
        case 0x13: // error message
        case 0x17: // emoted text
            ChatEvent(who, wsaid);
            break;
        default:
            break;
        }
        delete[] wsaid;
        // ChannelEvent(mode,who,said);
    }

    return !blockPacket;
}
