#pragma once

#include <windows.h>
#include <string>
#include <list>

#include "D2Structs.h"

bool SplitLines(const std::string & str, size_t maxlen, const char delim, std::list<std::string> & lst);
void Print(const char * szFormat, ...);
void Say(const char* szMessage);
bool ClickMap(DWORD dwClickType, int wX, int wY, BOOL bShift, UnitAny* pUnit);
void LoadMPQ(const char* mpq);
