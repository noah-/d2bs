#pragma once

#ifndef __HELPERS_H__
#define __HELPERS_H__

#include "Script.h"

bool GetStackWalk( std::string &outWalk);
wchar_t* AnsiToUnicode(const char* str, UINT codepage = CP_UTF8);
char* UnicodeToAnsi(const wchar_t* str, UINT codepage = CP_UTF8);
void StringToLower(char* p);
void StringToLower(wchar_t* p);
bool StringToBool(const char* str);
void StringReplace(char* str, const char find, const char replace, size_t buflen);
void StringReplace(wchar_t* str, const wchar_t find, const wchar_t replace, size_t buflen);
bool SwitchToProfile(const wchar_t* profile);
void InitSettings(void);
bool InitHooks(void);
bool ExecCommand(const char* command);
bool StartScript(const wchar_t* script, ScriptState state);
void Reload(void);
bool ProcessCommand(const wchar_t* command, bool unprocessedIsCommand);

void GameJoined(void);
void MenuEntered(bool beginStarter);

char* DllLoadAddrStrs();
LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS* ptrs);
int __cdecl _purecall(void);
#endif
