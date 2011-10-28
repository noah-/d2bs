#pragma once

#include <windows.h>

void ChatEvent(char* lpszNick, char* lpszMsg);
void LifeEvent(DWORD dwLife);
void ManaEvent(DWORD dwMana);
void CopyDataEvent(DWORD dwMode, char* lpszMsg);
void GameMsgEvent(char* lpszMsg);
void GameActionEvent(BYTE mode, DWORD param1, DWORD param2, char* name1, char* name2);
void WhisperEvent(char* lpszNick, char* lpszMsg);
void KeyDownUpEvent(WPARAM bByte, BYTE bUp);
void PlayerAssignEvent(DWORD dwUnitId);
void MouseClickEvent(int button, POINT pt, bool bUp);
void MouseMoveEvent(POINT pt);
void ScriptBroadcastEvent(uintN argc, jsval* argv);
void GoldDropEvent(DWORD GID, BYTE Mode);
void ItemActionEvent(DWORD GID, char* Code, BYTE Mode, bool Global);
