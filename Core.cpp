#include <string>
#include <sstream>
#include <algorithm>
#include <list>

#include "Core.h"
#include "D2Ptrs.h"
//#include "D2Helpers.h"
#include "Helpers.h"
#include "Control.h"
#include "CriticalSections.h"
#include "Console.h"

bool SplitLines(const std::string &str, size_t maxlen, const char delim, std::list<std::string> &lst) {
    using namespace std;

    if (str.length() < 1 || maxlen < 2)
        return false;

    size_t pos;
    string tmp(str);

    // Try getting appropriate maxlength for UTF8 string
    double ratio = (double)tmp.size() / UTF8Length(tmp);
    int maxLength = (int)((int)maxlen - ((ratio - 1.0) * 20));

    while (UTF8Length(tmp) > maxLength) {
        // Get byte index for UTF8 string
        int byteIdx = UTF8FindByteIndex(tmp, maxLength);
        // byteIdx-1 since std::string::npos indexes from 0
        pos = tmp.find_last_of(delim, byteIdx - 1);
        if (!pos || pos == string::npos) {
            // Target delimiter was not found, breaking at byteIdx
            lst.push_back(tmp.substr(0, byteIdx));
            tmp.erase(0, byteIdx);
            continue;
        }
        pos = tmp.find_last_of(delim, byteIdx - 1);
        if (pos && pos != string::npos) {
            // We found the last delimiter before byteIdx
            lst.push_back(tmp.substr(0, pos) + delim);
            tmp.erase(0, pos);
        } else
            DebugBreak();
    }
    if (!tmp.length())
        DebugBreak();

    if (tmp.length())
        lst.push_back(tmp);

    return true;
}

void Print(const char *szFormat, ...) {
    va_list vaArgs;
    va_start(vaArgs, szFormat);
    int len = _vscprintf(szFormat, vaArgs);
    char *str = new char[len + 1];
    vsprintf_s(str, len + 1, szFormat, vaArgs);
    va_end(vaArgs);

    EnterCriticalSection(&Vars.cPrintSection);
    Vars.qPrintBuffer.push(std::string(str));
    LeaveCriticalSection(&Vars.cPrintSection);

    delete[] str;
}

void __declspec(naked) __fastcall Say_ASM(DWORD dwPtr) {
    __asm
    {
		POP EAX
		PUSH ECX
		PUSH EAX
		PUSH EBP
		MOV EBP, ESP
		SUB ESP,0x110
		PUSH EBX
		PUSH ESI
		PUSH EDI
		MOV EBX, [EBP+8]
      // MOV EBP, [D2LANG_Say_II]
		JMP D2CLIENT_Say_I
    }
}

void __fastcall Say(const char *szFormat, ...) {
    va_list vaArgs;
    va_start(vaArgs, szFormat);
    int len = _vscprintf(szFormat, vaArgs);
    char *szBuffer = new char[len + 1];
    vsprintf_s(szBuffer, len + 1, szFormat, vaArgs);
    va_end(vaArgs);

    Vars.bDontCatchNextMsg = TRUE;

    if (*p_D2CLIENT_PlayerUnit) {
        wchar_t *wBuffer = AnsiToUnicode(szBuffer);
        memcpy((wchar_t *)p_D2CLIENT_ChatMsg, wBuffer, (wcslen(wBuffer) + 1) * sizeof(wchar_t));
        delete[] wBuffer;
        wBuffer = NULL;

        MSG *aMsg = new MSG;
        aMsg->hwnd = D2GFX_GetHwnd();
        aMsg->message = WM_CHAR;
        aMsg->wParam = VK_RETURN;
        aMsg->lParam = 0x11C0001;
        aMsg->time = NULL;
        aMsg->pt.x = 0x79;
        aMsg->pt.y = 0x1;

        Say_ASM((DWORD)&aMsg);
        delete aMsg;
        aMsg = NULL;

        /*
                        Vars.bDontCatchNextMsg = FALSE;
                        int len = 6+strlen(szMessage);
                        BYTE* pPacket = new BYTE[6+strlen(szMessage)];
                        memset(pPacket, 0, len);
                        pPacket[0] = 0x15;
                        pPacket[1] = 0x01;
                        memcpy(pPacket+3, szMessage, len-6);
                        D2CLIENT_SendGamePacket(len, pPacket);
                        delete [] pPacket;
        */
    }
    // help button and ! ok msg for disconnected
    else if (findControl(CONTROL_BUTTON, 5308, -1, 187, 470, 80, 20) && (!findControl(CONTROL_BUTTON, 5102, -1, 351, 337, 96, 32))) {
        memcpy((char *)p_D2MULTI_ChatBoxMsg, szBuffer, strlen(szBuffer) + 1);
        D2MULTI_DoChat();
    }

    delete[] szBuffer;
}

bool ClickMap(DWORD dwClickType, int wX, int wY, BOOL bShift, UnitAny *pUnit) {
    if (ClientState() != ClientStateInGame)
        return false;

    POINT Click = {wX, wY};
    if (pUnit) {
        Click.x = D2CLIENT_GetUnitX(pUnit);
        Click.y = D2CLIENT_GetUnitY(pUnit);
    }

    D2COMMON_MapToAbsScreen(&Click.x, &Click.y);

    Click.x -= *p_D2CLIENT_ViewportX;
    Click.y -= *p_D2CLIENT_ViewportY;

    POINT OldMouse = {0, 0};
    OldMouse.x = *p_D2CLIENT_MouseX;
    OldMouse.y = *p_D2CLIENT_MouseY;
    *p_D2CLIENT_MouseX = 0;
    *p_D2CLIENT_MouseY = 0;

    if (pUnit && pUnit != D2CLIENT_GetPlayerUnit() /* && D2CLIENT_UnitTestSelect(pUnit, 0, 0, 0)*/) {
        Vars.dwSelectedUnitId = pUnit->dwUnitId;
        Vars.dwSelectedUnitType = pUnit->dwType;

        Vars.bClickAction = TRUE;

        D2CLIENT_ClickMap(dwClickType, Click.x, Click.y, bShift ? 0x0C : (*p_D2CLIENT_AlwaysRun ? 0x08 : 0));
        D2CLIENT_SetSelectedUnit(NULL);

        Vars.bClickAction = FALSE;
        Vars.dwSelectedUnitId = NULL;
        Vars.dwSelectedUnitType = NULL;
    } else {
        Vars.dwSelectedUnitId = NULL;
        Vars.dwSelectedUnitType = NULL;

        Vars.bClickAction = TRUE;
        D2CLIENT_ClickMap(dwClickType, Click.x, Click.y, bShift ? 0x0C : (*p_D2CLIENT_AlwaysRun ? 0x08 : 0));
        Vars.bClickAction = FALSE;
    }

    *p_D2CLIENT_MouseX = OldMouse.x;
    *p_D2CLIENT_MouseY = OldMouse.y;
    return TRUE;
}

void LoadMPQ(const char *mpq) {
    D2WIN_InitMPQ(mpq, 0, 0, 3000);
    *p_BNCLIENT_XPacKey = *p_BNCLIENT_ClassicKey = *p_BNCLIENT_KeyOwner = NULL;
    // BNCLIENT_DecodeAndLoadKeys();
}

int UTF8FindByteIndex(std::string str, int maxutf8len) {
    int utf8len = 0, byteIndex = 0;
    const char *tstr = str.c_str();
    size_t strlen = str.size();

    for (byteIndex = 0; byteIndex < strlen; byteIndex++) {
        if ((tstr[byteIndex] & 0xc0) != 0x80)
            utf8len += 1;

        if (utf8len >= maxutf8len)
            break;
    }

    return byteIndex;
}

int UTF8Length(std::string str) {
    const char *tmp = str.c_str();
    int len = 0;
    while (*tmp)
        len += (*tmp++ & 0xc0) != 0x80;

    return len;
}