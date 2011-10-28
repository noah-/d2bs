#include <vector>

#include "Unit.h"
#include "Constants.h"
#include "D2Helpers.h"
#include "D2BS.h"

#define HAS_BIT(value, bit) ((((value) >> (bit)) & 0x1) == 0x1)

using namespace std;

// TODO: If UnitId is the unique id of the unit, we can just look up that
// location in the table
static UnitAny* GetUnitFromTables(UnitHashTable* unitTables, DWORD dwTypeLow,
	DWORD dwTypeHigh, char* szName, DWORD dwClassId, DWORD dwType, DWORD dwMode,
	DWORD dwUnitId)
{
	unsigned int i, j;
	unsigned int hashLow, hashHigh;
	UnitAny* tmpUnit;

	if(dwUnitId != -1)
		hashLow = hashHigh = dwUnitId & 0x7F;	// % 128
	else
	{
		hashLow = 0;
		hashHigh = 127;
	}

	// Go through all the types
	for(i = dwTypeLow; i <= dwTypeHigh; ++i)
	{
		// Go through all the hash values
		for(j = hashLow; j <= hashHigh; ++j)
		{
			// Go through all the units in a given hash value
			for(tmpUnit = unitTables[i].table[j]; tmpUnit != NULL;
					tmpUnit = tmpUnit->pListNext)
				// Check if it matches
				if(CheckUnit(tmpUnit, szName, dwClassId, dwType, dwMode,
							dwUnitId))
					return tmpUnit;
		}
	}

	return NULL;
}

UnitAny* GetUnit(char* szName, DWORD dwClassId, DWORD dwType, DWORD dwMode,
	DWORD dwUnitId)
{
	if(ClientState() != ClientStateInGame)
		return NULL;

	// If we have a valid type, just check that value, other wise, check all
	// values. There are 6 valid types, 0-5
	if(dwType == 3)
		return GetUnitFromTables(p_D2CLIENT_ClientSideUnitHashTables, dwType,
				dwType, szName, dwClassId, dwType, dwMode, dwUnitId);

	if(dwType >= 0 && dwType <= 5)
		return GetUnitFromTables(p_D2CLIENT_ServerSideUnitHashTables, dwType,
				dwType, szName, dwClassId, dwType, dwMode, dwUnitId);
	else
		return GetUnitFromTables(p_D2CLIENT_ServerSideUnitHashTables, 0, 5,
				szName, dwClassId, dwType, dwMode, dwUnitId);
	

/*	EnterCriticalSection(&Vars.cUnitListSection);

	UnitAny* result = NULL;
	for(vector<pair<DWORD, DWORD> >::iterator it = Vars.vUnitList.begin(); it != Vars.vUnitList.end(); it++)
	{
		UnitAny* unit = D2CLIENT_FindUnit(it->first, it->second);
		if(unit != NULL && CheckUnit(unit, szName, dwClassId, dwType, dwMode, dwUnitId))
		{
			result = unit;
			break;
		}
	}

	LeaveCriticalSection(&Vars.cUnitListSection);

	return result;*/

	// First off, check for near units
/*	UnitAny* player = D2CLIENT_GetPlayerUnit();
	if(player && player->pPath && player->pPath->pRoom1 && player->pPath->pRoom1->pRoom2 && 
		player->pPath->pRoom1->pRoom2->pLevel)
	{
		Room2* ptRoomOther = player->pPath->pRoom1->pRoom2->pLevel->pRoom2First;

		for(;ptRoomOther; ptRoomOther = ptRoomOther->pRoom2Next)
		{
			if(!ptRoomOther->pRoom1)
				continue;
			for(UnitAny* lpUnit = ptRoomOther->pRoom1->pUnitFirst; lpUnit; lpUnit = lpUnit->pListNext)
			{
				if(CheckUnit(lpUnit, szName, dwClassId, dwType, dwMode, dwUnitId))
					return lpUnit;
			}
		}
	}

	return NULL;*/
}

static DWORD dwMax(DWORD a, DWORD b)
{
	return a > b ? a : b;
}

static UnitAny* GetNextUnitFromTables(UnitAny* curUnit,
	UnitHashTable* unitTables, DWORD dwTypeLow, DWORD dwTypeHigh, char* szName,
	DWORD dwClassId, DWORD dwType, DWORD dwMode)
{
	unsigned int i, j;
	UnitAny* tmpUnit;

	// If we're looking for the same type unit, or any type then finish off the
	// current inner iterations
	if(dwType == -1 || dwType == curUnit->dwType)
	{
		i = curUnit->dwType;

		// Finish off the current linked list
		for(tmpUnit = curUnit->pListNext; tmpUnit != NULL; tmpUnit = tmpUnit->pListNext)
			// Check if it matches
			if(CheckUnit(tmpUnit, szName, dwClassId, dwType, dwMode, (DWORD)-1))
				return tmpUnit;

		// Finish off the current hash table
		for(j = (curUnit->dwUnitId & 0x7f) + 1; j <= 127; ++j)
			// Go through all the units in this linked list
			for(tmpUnit = unitTables[i].table[j]; tmpUnit != NULL;
					tmpUnit = tmpUnit->pListNext)
				// Check if it matches
				if(CheckUnit(tmpUnit, szName, dwClassId, dwType, dwMode,
							(DWORD)-1))
					return tmpUnit;
	}

	// Go through all the remaining types
	for(i = dwMax(dwTypeLow, curUnit->dwType + 1); i <= dwTypeHigh; ++i)
	{
		// Go through all the hash values
		for(j = 0; j < 127; ++j)
		{
			// Go through all the units in a given hash value
			for(tmpUnit = unitTables[i].table[j]; tmpUnit != NULL;
					tmpUnit = tmpUnit->pListNext)
				// Check if it matches
				if(CheckUnit(tmpUnit, szName, dwClassId, dwType, dwMode,
							(DWORD)-1))
					return tmpUnit;
		}
	}

	return NULL;
}

UnitAny* GetNextUnit(UnitAny* pUnit, char* szName, DWORD dwClassId,
	DWORD dwType, DWORD dwMode)
{
	if(ClientState() != ClientStateInGame)
		return NULL;

	if(!pUnit)
		return NULL;

	if(dwType >= 0 && dwType <= 5)
		return GetNextUnitFromTables(pUnit, p_D2CLIENT_ServerSideUnitHashTables,
				dwType, dwType, szName, dwClassId, dwType, dwMode);
	else
		return GetNextUnitFromTables(pUnit, p_D2CLIENT_ServerSideUnitHashTables,
				0, 5, szName, dwClassId, dwType, dwMode);

/*	EnterCriticalSection(&Vars.cUnitListSection);

	UnitAny* result = NULL;
	// find where we left off
	vector<pair<DWORD, DWORD> >::iterator it = Vars.vUnitList.begin();
	for(; it != Vars.vUnitList.end(); it++)
	{
		if(it->first == pUnit->dwUnitId && it->second == pUnit->dwType)
			// this is where we left off
			break;
	}

	if(it != Vars.vUnitList.end())
	{
		it++;

		for(; it != Vars.vUnitList.end(); it++)
		{
			UnitAny* unit = D2CLIENT_FindUnit(it->first, it->second);
			if(unit != NULL && CheckUnit(unit, szName, dwClassId, dwType, dwMode, (DWORD)-1))
			{
				result = unit;
				break;
			}
		}
	}

	LeaveCriticalSection(&Vars.cUnitListSection);

	return result;*/

/*	UnitAny* lpUnit = pUnit->pListNext;
	Room1* ptRoom = D2COMMON_GetRoomFromUnit(pUnit);
	Room2* ptRoomOther = NULL;

	if(ptRoom)
	{
		ptRoomOther = ptRoom->pRoom2;

		if(!lpUnit && ptRoomOther)
			ptRoomOther = ptRoomOther->pRoom2Next;

		for(; ptRoomOther; ptRoomOther = ptRoomOther->pRoom2Next)
		{
			if(ptRoomOther->pRoom1)
			{
				if(!lpUnit)
					lpUnit = ptRoomOther->pRoom1->pUnitFirst;

				for(; lpUnit; lpUnit = lpUnit->pListNext)
				{
					if(CheckUnit(lpUnit, szName, dwClassId, dwType, dwMode, (DWORD)-1))
						return lpUnit;
				}
			}
		}
	}

	return NULL;*/
}

UnitAny* GetInvUnit(UnitAny* pOwner, char* szName, DWORD dwClassId, DWORD dwMode, DWORD dwUnitId)
{
	for(UnitAny* pItem = D2COMMON_GetItemFromInventory(pOwner->pInventory); pItem; pItem = D2COMMON_GetNextItemFromInventory(pItem))
	{
		if(CheckUnit(pItem, szName, dwClassId, 4, dwMode, dwUnitId))
			return pItem;
	}

	return NULL;
}

UnitAny* GetInvNextUnit(UnitAny* pUnit, UnitAny* pOwner, char* szName, DWORD dwClassId, DWORD dwMode)
{
	if(pUnit->dwType == UNIT_ITEM)
	{
		// Check first if it belongs to a person
		if(pUnit->pItemData && pUnit->pItemData->pOwnerInventory && pUnit->pItemData->pOwnerInventory == pOwner->pInventory)
		{
			// Get the next matching unit from the owner's inventory
			for(UnitAny* pItem = D2COMMON_GetNextItemFromInventory(pUnit); pItem; pItem = D2COMMON_GetNextItemFromInventory(pItem))
			{
				if(CheckUnit(pItem, szName, dwClassId, 4, dwMode, (DWORD)-1))
					return pItem;
			}

		}
	}

	return NULL;
}

BOOL CheckUnit(UnitAny* pUnit, char* szName, DWORD dwClassId, DWORD dwType, DWORD dwMode, DWORD dwUnitId)
{
	if((dwUnitId != -1 && pUnit->dwUnitId != dwUnitId) ||
		(dwType != -1 && pUnit->dwType != dwType) ||
		(dwClassId != -1 && pUnit->dwTxtFileNo != dwClassId))
		return FALSE;

	if(dwMode != -1)
	{
		if(dwMode >= 100 && pUnit->dwType == UNIT_ITEM)
		{
			if(pUnit->pItemData && dwMode-100 != pUnit->pItemData->ItemLocation)
				return FALSE;
		}
		else
		{
			if(HAS_BIT(dwMode, 29))
			{
				bool result = false;
				// mode is a mask
				for(unsigned int i = 0; i < 28; i++)
					if(HAS_BIT(dwMode, i) && pUnit->dwMode == i)
						result = true;
				if(!result)
					return FALSE;
			}
			else if(pUnit->dwMode != dwMode)
				return FALSE;
		}
	}

	if(szName && szName[0])
	{
		char szBuf[512] = "";

		if(dwType == UNIT_ITEM)
			GetItemCode(pUnit, szBuf);
		else
			GetUnitName(pUnit, szBuf, 512);
		if(!!_stricmp(szBuf, szName))
			return FALSE;
	}

	return TRUE;
}

int GetUnitHP(UnitAny* pUnit)
{
	return (int)(D2COMMON_GetUnitStat(pUnit, STAT_HP, 0) >> 8);
}

int GetUnitMP(UnitAny* pUnit)
{
	return (int)(D2COMMON_GetUnitStat(pUnit, STAT_MANA, 0) >> 8);
}
