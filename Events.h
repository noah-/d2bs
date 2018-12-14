#pragma once

#include <windows.h>

bool ChatEvent(char* lpszNick, wchar_t* lpszMsg);
bool ChatInputEvent(wchar_t* lpszMsg);
void LifeEvent(DWORD dwLife);
void ManaEvent(DWORD dwMana);
void CopyDataEvent(DWORD dwMode, wchar_t* lpszMsg);
void GameMsgEvent(char* lpszMsg);
void GameActionEvent(BYTE mode, DWORD param1, DWORD param2, char* name1, char* name2);
bool WhisperEvent(char* lpszNick, wchar_t* lpszMsg);
bool KeyDownUpEvent(WPARAM bByte, BYTE bUp);
void PlayerAssignEvent(DWORD dwUnitId);
void MouseClickEvent(int button, POINT pt, bool bUp);
void MouseMoveEvent(POINT pt);
void ScriptBroadcastEvent(JSContext* cx, uint argc, jsval* argv);
void GoldDropEvent(DWORD GID, BYTE Mode);
void ItemActionEvent(DWORD GID, char* Code, BYTE Mode, bool Global);
bool GamePacketEvent(BYTE* pPacket, DWORD dwSize);
bool GamePacketSentEvent(BYTE* pPacket, DWORD dwSize);
bool RealmPacketEvent(BYTE* pPacket, DWORD dwSize);
void ReleaseGameLock(void);

void TakeGameLock(void);

struct ChatEventHelper {
    char *name, *nick;
    wchar_t* msg;
};

struct CopyDataHelper {
    DWORD mode;
    wchar_t* msg;
};

struct ItemEventHelper {
    DWORD id;
    char* code;
    WORD mode;
    bool global;
};

struct KeyEventHelper {
    BOOL up;
    WPARAM key;
};

struct GameActionEventHelper {
    BYTE mode;
    DWORD param1, param2;
    char *name1, *name2;
};

struct SingleArgHelper {
    DWORD arg1;
};

struct DoubleArgHelper {
    DWORD arg1, arg2;
};

struct TripleArgHelper {
    DWORD arg1, arg2, arg3;
};
struct QuadArgHelper {
    DWORD arg1, arg2, arg3, arg4;
};

struct BCastEventHelper {
    JSContext* cx;
    jsval* argv;
    uint argc;
};
struct PacketEventHelper {
    char* name;
    BYTE* pPacket;
    DWORD dwSize;
};
