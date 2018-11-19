#pragma once

#include <windows.h>
#include "D2Structs.h"

DWORD WINAPI D2Thread(LPVOID lpParam);
DWORD __fastcall GameInput(wchar_t* wMsg);
LONG WINAPI GameEventHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseMove(int code, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyPress(int code, WPARAM wParam, LPARAM lParam);
void GameDraw(void);
DWORD __fastcall GamePacketReceived(BYTE* pPacket, DWORD dwSize);
DWORD __fastcall GamePacketSent(BYTE* pPacket, DWORD dwSize);
void __fastcall GameResetFrames(UnitAny* pOwner);
void GameDrawOOG(void);
void SetMaxDiff(void);
void __stdcall AddUnit(UnitAny* lpUnit);
void __stdcall RemoveUnit(UnitAny* lpUnit);
void __fastcall WhisperHandler(char* szAcc, char* szText);
DWORD __fastcall ChannelInput(wchar_t* wMsg);
DWORD __fastcall GameAttack(UnitInteraction* pAttack);
void __fastcall GamePlayerAssignment(UnitAny* pPlayer);
void GameLeave(void);
void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
VOID __fastcall ClassicSTUB();
VOID __fastcall LodSTUB();
void FailToJoin();
HMODULE __stdcall Multi(LPSTR Class, LPSTR Window);
HANDLE __stdcall Windowname(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                            HINSTANCE hInstance, LPVOID lpParam);
HANDLE __stdcall CacheFix(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                          DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
BOOL __fastcall RealmPacketRecv(BYTE* pPacket, DWORD dwSize);
BOOL __fastcall ChatPacketRecv(BYTE* pPacket, int len);
LONG WINAPI MyUnhandledExceptionFilter(_In_ struct _EXCEPTION_POINTERS* ExceptionInfo);
void FogException();
char __fastcall ErrorReportLaunch(const char* crash_file, int a2);