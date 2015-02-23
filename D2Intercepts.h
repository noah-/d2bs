#ifndef D2INTERCEPTS_H
#define D2INTERCEPTS_H

#include "D2Structs.h"

void RealmPacketRecv_Interception();
void GameDraw_Intercept();
void GameInput_Intercept();
void GamePacketReceived_Intercept();
void GamePacketSent_Interception();
UnitAny* GetSelectedUnit_Intercept(void);
void Whisper_Intercept();
void GameAttack_Intercept();
void PlayerAssignment_Intercept();
void GameCrashFix_Intercept();
void GameDrawOOG_Intercept(void);
void GameActChange_Intercept(void);
void GameActChange2_Intercept(void);
void GameLeave_Intercept(void);
void ChannelInput_Intercept(void);
void ChatPacketRecv_Interception();
void AddUnit_Intercept(UnitAny* lpUnit);
void RemoveUnit_Intercept(UnitAny* lpUnit);
int WINAPI LogMessageBoxA_Intercept(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption,
    UINT uType);

#endif
