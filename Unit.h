#pragma once

#include "D2Ptrs.h"

UnitAny* GetUnit(char* szName, DWORD dwClassId, DWORD dwType, DWORD dwMode, DWORD dwUnitId);
UnitAny* GetNextUnit(UnitAny* pUnit, char* szName, DWORD dwClassId, DWORD dwType, DWORD dwMode);
UnitAny* GetInvUnit(UnitAny* pOwner, char* szName, DWORD dwClassId, DWORD dwMode, DWORD dwUnitId);
UnitAny* GetInvNextUnit(UnitAny* pUnit, UnitAny* pOwner, char* szName, DWORD dwClassId, DWORD dwMode);
BOOL CheckUnit(UnitAny* pUnit, char* szName, DWORD dwClassId, DWORD dwType, DWORD dwMode, DWORD dwUnitId);
int GetUnitHP(UnitAny* pUnit);
int GetUnitMP(UnitAny* pUnit);
