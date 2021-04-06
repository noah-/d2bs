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

bool SplitLines(const std::wstring& str, size_t maxWidth, const wchar_t delim, std::list<std::wstring>& lst) {
    using namespace std;
    wstring tmp(str);

    if (str.length() < 1 || maxWidth < 40)
        return false;

    // base case
    if (CalculateTextLen(str.c_str(), Vars.dwConsoleFont).x < maxWidth) {
        lst.push_back(tmp);
        return true;
    }

    int byteIdx = MaxLineFit(str, 0, str.length() + 1, maxWidth);
    wstring ts = str.substr(0, byteIdx);
    uint cmdsize = CalculateTextLen(ts.c_str(), Vars.dwConsoleFont).x;
    int numchars = ts.length();
    int sizechar = (cmdsize + numchars - 1) / numchars;
    int maxLength = (maxWidth + sizechar - 1) / sizechar - 1;

    // byteIdx-1 since std::string::npos indexes from 0
    int pos = tmp.find_last_of(delim, byteIdx - 1);
    if (!pos || pos == string::npos) {
        // Target delimiter was not found, breaking at byteIdx
        wstring ts = tmp.substr(0, byteIdx);
        lst.push_back(ts);
        tmp.erase(0, byteIdx);
    } else {
        // We found the last delimiter before byteIdx
        wstring ts = tmp.substr(0, pos);
        lst.push_back(ts);
        tmp.erase(0, pos);
    }

    return SplitLines(tmp, maxWidth, delim, lst);
}

void Print(const wchar_t* szFormat, ...) {
    va_list vaArgs;
    va_start(vaArgs, szFormat);

    int len = _vscwprintf(szFormat, vaArgs);
    wchar_t* str = new wchar_t[len + 1];
    vswprintf_s(str, len + 1, szFormat, vaArgs);
    va_end(vaArgs);

    EnterCriticalSection(&Vars.cPrintSection);
    Vars.qPrintBuffer.push(std::wstring(str));
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

void __fastcall Say(const wchar_t* szFormat, ...) {
    va_list vaArgs;
    va_start(vaArgs, szFormat);
    int len = _vscwprintf(szFormat, vaArgs);
    wchar_t* szBuffer = new wchar_t[len + 1];
    vswprintf_s(szBuffer, len + 1, szFormat, vaArgs);
    va_end(vaArgs);

    Vars.bDontCatchNextMsg = TRUE;

    if (*p_D2CLIENT_PlayerUnit) {
        memcpy((wchar_t*)p_D2CLIENT_ChatMsg, szBuffer, (len + 1) * sizeof(wchar_t));

        MSG* aMsg = new MSG;
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
        char* lBuffer = UnicodeToAnsi(szBuffer, CP_ACP);
        memcpy((char*)p_D2MULTI_ChatBoxMsg, lBuffer, strlen(lBuffer) + 1);
        D2MULTI_DoChat();
        delete[] lBuffer;
    }

    delete[] szBuffer;
}

bool ClickMap(DWORD dwClickType, int wX, int wY, BOOL bShift, UnitAny* pUnit) {
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

    if (pUnit && pUnit != D2CLIENT_GetPlayerUnit() /* && D2CLIENT_FindUnit(pUnit->dwUnitId, pUnit->dwType) && D2CLIENT_UnitTestSelect(pUnit, 0, 0, 0)*/) {
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

void LoadMPQ(const char* mpq) {
    D2WIN_InitMPQ(mpq, 0, 0, 3000);
    *p_BNCLIENT_XPacKey = *p_BNCLIENT_ClassicKey = *p_BNCLIENT_KeyOwner = NULL;
    // BNCLIENT_DecodeAndLoadKeys();
}

void LoadMPQ(const wchar_t* mpq) {
    char* path = UnicodeToAnsi(mpq);
    LoadMPQ(path);
    delete[] path;
    // BNCLIENT_DecodeAndLoadKeys();
}

int UTF8FindByteIndex(std::string str, int maxutf8len) {
    int utf8len = 0, byteIndex = 0;
    const char* tstr = str.c_str();
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
    const char* tmp = str.c_str();
    int len = 0;
    while (*tmp)
        len += (*tmp++ & 0xc0) != 0x80;

    return len;
}

int MeasureText(const std::wstring& str, int index) {
    return CalculateTextLen(str.substr(0, index).c_str(), Vars.dwConsoleFont).x;
}

int MaxLineFit(const std::wstring& str, int start_idx, int end_idx, int maxWidth) {
    if (start_idx == end_idx) {
        return MeasureText(str, start_idx) <= maxWidth ? start_idx : -1;
    }

    int mid_idx = start_idx + (end_idx - start_idx) / 2;

    if (maxWidth < MeasureText(str, mid_idx))
        return MaxLineFit(str, start_idx, mid_idx, maxWidth);

    int ret = MaxLineFit(str, mid_idx + 1, end_idx, maxWidth);
    return ret == -1 ? mid_idx : ret;
}