#include "MPQStats.h"
#include "D2Ptrs.h"
#include "Core.h"
MPQTable BaseStatTable[] = {
	//DWORD dwEntry, DWORD dwMaxEntriesOffset, BinField* pTable, char szTableName, WORD wTableSize, WORD wUnknown
	{0x8426F0,		0x8426EC,	itemTable,			"items",		ARRAYSIZE(itemTable),			0xFFFF},
	{0xA78,			0xA80,		monstatsTable,		"monstats",		ARRAYSIZE(monstatsTable),		0xFFFF},
	{0xB8C,			0xB94,		skilldescTable,		"skilldesc",	ARRAYSIZE(skilldescTable),		0xFFFF},
	{0xB98,			0xBA0,		skillsTable,		"skills",		ARRAYSIZE(skillsTable),			0xFFFF},
	{0x96D470,		0x96D474,	objectsTable,		"objects",		ARRAYSIZE(objectsTable),		0xFFFF},
	{0xB64,			0xB6C,		missilesTable,		"missiles",		ARRAYSIZE(missilesTable),		0xFFFF},
	{0xA90,			0xA98,		monstats2Table,		"monstats2",	ARRAYSIZE(monstats2Table),		0xFFFF},
	{0xBCC,			0xBD4,		itemstatcostTable,	"itemstatcost",	ARRAYSIZE(itemstatcostTable),	0xFFFF},
	{0xC58,			0xC5C,		levelsTable,		"levels",		ARRAYSIZE(levelsTable),			0xFFFF},
	{0xC60,			0x00,		leveldefsTable,		"leveldefs",	ARRAYSIZE(leveldefsTable),		0xFFFF},
	{0x96CA08,		0x96CA0C,	lvlmazeTable,		"lvlmaze",		ARRAYSIZE(lvlmazeTable),		0xFFFF},
	{0x96CA10,		0x96CA14,	lvlsubTable,		"lvlsub",		ARRAYSIZE(lvlsubTable),			0xFFFF}, // v
	{0x96CA00,		0x96CA04,	lvlwarpTable,		"lvlwarp",		ARRAYSIZE(lvlwarpTable),		0xFFFF}, // v
	{0xC64,			0xC68,		lvlprestTable,		"lvlprest",		ARRAYSIZE(lvlprestTable),		0xFFFF},
	{0x96C9F0,		0x96C9FC,	lvltypesTable,		"lvltypes",		ARRAYSIZE(lvltypesTable),		0xFFFF}, // v - Fixed to fully dump the same as d2jsp - TechnoHunter
	{0xBC4,			0xBC8,		charstatsTable,		"charstats",	ARRAYSIZE(charstatsTable),		0xFFFF},
	{0xC18,			0xC1C,		setitemsTable,		"setitems",		ARRAYSIZE(setitemsTable),		0xFFFF},
	{0xC24,			0xC28,		uniqueitemsTable,	"uniqueitems",	ARRAYSIZE(uniqueitemsTable),	0xFFFF},
	{0xC0C,			0xC10,		setsTable,			"sets",			ARRAYSIZE(setsTable),			0xFFFF},
	{0xBF8,			0xBFC,		itemtypesTable,		"itemtypes",	ARRAYSIZE(itemtypesTable),		0xFFFF},
	{0x96CA9C,		0x96CA98,	runesTable,			"runes",		ARRAYSIZE(runesTable),			0xFFFF}, // v
	{0x96D628,		0x96D62C,	cubemainTable,		"cubemain",		ARRAYSIZE(cubemainTable),		0xFFFF}, //v
	{0x96CA94,		0x96CA90,	gemsTable,			"gems",			ARRAYSIZE(gemsTable),			0xFFFF},
	{0x96C8A8,		0x0,		experienceTable,	"experience",	ARRAYSIZE(experienceTable),		0xFFFF}, // v - doesnt tap the last 2 levels of exp, ends at level 97 - TechnoHunter
	{0xBE8,			0xBF0,		pettypeTable,		"pettable",		ARRAYSIZE(pettypeTable),		0xFFFF},
	{0xAD4,			0xADC,		superuniquesTable,	"superuniques",	ARRAYSIZE(superuniquesTable),	0xFFFF},
	{0}
};

DWORD GetBaseTable(int table, int row)
{
	DWORD dwResult = NULL;
	DWORD dwD2MPQTable = NULL;

	if(table < sizeof(BaseStatTable))
	{
		DWORD dwTableOffset = BaseStatTable[table].dwEntry;

		if(dwTableOffset <= 0xFFFF)
			dwD2MPQTable = (*p_D2COMMON_sgptDataTable);
		else 
		{
			dwD2MPQTable = NULL;  //d2common loading at a diffrent address crash fix
			dwTableOffset = DWORD ((dwTableOffset - 0x400000 ) + d2CommonOff);
		}
		
		DWORD dwMaxEntriesOffset = BaseStatTable[table].dwMaxEntriesOffset;
		if (dwMaxEntriesOffset > 0xFFFF)
			dwMaxEntriesOffset = DWORD ((dwMaxEntriesOffset - 0x400000 ) + d2CommonOff);

		DWORD dwMaxEntries;
		if(dwMaxEntriesOffset)
			dwMaxEntries = *(DWORD*)(dwMaxEntriesOffset + dwD2MPQTable);
		else dwMaxEntries = 0xFF;

		if((DWORD)row < dwMaxEntries)
		{
			DWORD dwMultiplicator = BaseStatTable[table].pTable[BaseStatTable[table].wTableSize-1].dwFieldOffset;
			DWORD dwTable = row * dwMultiplicator;
			dwResult = *(DWORD*)(dwTableOffset + dwD2MPQTable) + dwTable;
		}
	}

	return dwResult;
}

bool FillBaseStat(char* szTable, int row, char* szStat, void* result, size_t size)
{
	int table = -1;
	for(int i = 0; BaseStatTable[i].pTable != NULL; i++)
		if(!_strcmpi(szTable, BaseStatTable[i].szTableName))
		{
			table = i;
			break;
		}

	if(table == -1)
		return false;

	return FillBaseStat(table, row, szStat, result, size);
}

bool FillBaseStat(char* szTable, int row, int column, void* result, size_t size)
{
	int table = -1;
	for(int i = 0; BaseStatTable[i].pTable != NULL; i++)
		if(!_strcmpi(szTable, BaseStatTable[i].szTableName))
		{
			table = i;
			break;
		}

	if(table == -1)
		return false;

	return FillBaseStat(table, row, column, result, size);
}

bool FillBaseStat(int table, int row, char* szStat, void* result, size_t size)
{
	BinField* pTable = BaseStatTable[table].pTable;

	int column = -1;
	for(int i = 0; i < BaseStatTable[table].wTableSize; i++)
		if(!_strcmpi(szStat, pTable[i].szFieldName))
		{
			column = i;
			break;
		}

	if(column == -1)
		return false;

	return FillBaseStat(table, row, column, result, size);
}

bool FillBaseStat(int table, int row, int column, void* result, size_t size)
{
	BinField* pTable = BaseStatTable[table].pTable;
	DWORD dwRetValue = GetBaseTable(table, row);

	if(!dwRetValue || column > BaseStatTable[table].wTableSize)
		return false;

	DWORD dwHelperSize = pTable[column+1].dwFieldOffset - pTable[column].dwFieldOffset;
	if(dwHelperSize > 4)
		dwHelperSize = 4;
	switch(pTable[column].eFieldType)
	{
		case FIELDTYPE_DATA_ASCII:
			if(size < pTable[column].dwFieldLength)
				return false;
			memcpy_s(result, pTable[column].dwFieldLength, (BYTE*)(dwRetValue+pTable[column].dwFieldOffset), pTable[column].dwFieldLength);
			break;
		case FIELDTYPE_DATA_DWORD:
			memcpy(result, (LPVOID)(dwRetValue+pTable[column].dwFieldOffset), sizeof(DWORD));
			break;
		case FIELDTYPE_CALC_TO_DWORD:
		case FIELDTYPE_NAME_TO_DWORD:
		case FIELDTYPE_DATA_DWORD_2:
			memcpy(result, (LPVOID)(dwRetValue+pTable[column].dwFieldOffset), sizeof(DWORD));
			break;
		case FIELDTYPE_UNKNOWN_11:
			memcpy(result, (LPVOID)(dwRetValue+pTable[column].dwFieldOffset), sizeof(DWORD));
			break;
		case FIELDTYPE_NAME_TO_INDEX_2:
		case FIELDTYPE_NAME_TO_WORD_2:
			memcpy(result, (LPVOID)(dwRetValue+pTable[column].dwFieldOffset), sizeof(WORD));
			if(((WORD)result) >= 0xFFFF)
				*(WORD*)result = (((WORD)result) - 0xFFFF) * -1;
			break;
		case FIELDTYPE_NAME_TO_INDEX:
		case FIELDTYPE_NAME_TO_WORD:
		case FIELDTYPE_KEY_TO_WORD:
		case FIELDTYPE_DATA_WORD:
		case FIELDTYPE_CODE_TO_WORD:
			memcpy(result, (LPVOID)(dwRetValue+pTable[column].dwFieldOffset), sizeof(WORD));
			break;
		case FIELDTYPE_CODE_TO_BYTE:
		case FIELDTYPE_DATA_BYTE_2:
		case FIELDTYPE_DATA_BYTE:
			memcpy(result, (LPVOID)(dwRetValue+pTable[column].dwFieldOffset), dwHelperSize);
			break;
		case FIELDTYPE_DATA_BIT:
			memcpy(result, (LPVOID)(dwRetValue+pTable[column].dwFieldOffset), sizeof(DWORD));
			*(BOOL*)result = (*(BOOL*)result & (1 << pTable[column].dwFieldLength)) ? 1 : 0;
			break;
		case FIELDTYPE_ASCII_TO_CODE:
		case FIELDTYPE_DATA_RAW:
			if(size != 5)
				return false;
			memcpy(result, (LPVOID)(dwRetValue+pTable[column].dwFieldOffset), size-1);
			break;
		case FIELDTYPE_MONSTER_COMPS:
			// ..? :E
			return false;
	}

	return true;
}

DWORD FillBaseStat(JSContext* cx, jsval *argv, int table, int row, int column, char* szTable, char* szStat)
{
	if(szTable)
	{
		table = -1;
		for(int i = 0; BaseStatTable[i].pTable != NULL; i++)
			if(!_strcmpi(szTable, BaseStatTable[i].szTableName))
			{
				table = i;
				break;
			}

		if(table == -1)
			return false;
	}

	BinField* pTable = BaseStatTable[table].pTable;

	if(szStat)
	{
		column = -1;
		for(int i = 0; i < BaseStatTable[table].wTableSize; i++)
			if(!_strcmpi(szStat, pTable[i].szFieldName))
			{
				column = i;
				break;
			}

		if(column == -1)
			return false;
	}

	if(column > BaseStatTable[table].wTableSize)
		return FALSE;

	DWORD dwBuffer = 0;
	WORD wBuffer = 0;
	char* szBuffer = NULL;
	DWORD dwHelperSize = pTable[column+1].dwFieldOffset - pTable[column].dwFieldOffset;
	if(dwHelperSize > 4)
		dwHelperSize = 4;

	switch(pTable[column].eFieldType)
	{
		case FIELDTYPE_DATA_ASCII:
			szBuffer = new char[(pTable[column].dwFieldLength + 1)];
			memset(szBuffer, NULL, pTable[column].dwFieldLength + 1);
			if(!FillBaseStat(table, row, column, szBuffer, pTable[column].dwFieldLength+1))
				(*argv) = JSVAL_VOID;
			else
				(*argv) = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, szBuffer));
			delete[] szBuffer;
			return TRUE;

		case FIELDTYPE_DATA_DWORD:
			if(!FillBaseStat(table, row, column, &dwBuffer, sizeof(DWORD)))
				(*argv) = JSVAL_VOID;
			else
				*argv = JS_NumberValue((jsdouble)dwBuffer);
			return TRUE;

		case FIELDTYPE_CALC_TO_DWORD:
		case FIELDTYPE_NAME_TO_DWORD:
		case FIELDTYPE_DATA_DWORD_2:
		case FIELDTYPE_UNKNOWN_11:
			if(!FillBaseStat(table, row, column, &dwBuffer, sizeof(DWORD)))
				(*argv) = JSVAL_VOID;
			else
				*argv = JS_NumberValue((long)dwBuffer);
			return TRUE;

		case FIELDTYPE_NAME_TO_INDEX_2:
		case FIELDTYPE_NAME_TO_WORD_2:
		case FIELDTYPE_NAME_TO_INDEX:
		case FIELDTYPE_NAME_TO_WORD:
		case FIELDTYPE_KEY_TO_WORD:
		case FIELDTYPE_DATA_WORD:
		case FIELDTYPE_CODE_TO_WORD:
			if(!FillBaseStat(table, row, column, &wBuffer, sizeof(WORD)))
				(*argv) = JSVAL_VOID;
			else
				(*argv) = INT_TO_JSVAL(wBuffer);
			return TRUE;

		case FIELDTYPE_CODE_TO_BYTE:
		case FIELDTYPE_DATA_BYTE_2:
		case FIELDTYPE_DATA_BYTE:
			if(!FillBaseStat(table, row, column, &dwBuffer, dwHelperSize))
				(*argv) = JSVAL_VOID;
			else
				(*argv) = INT_TO_JSVAL(dwBuffer);
			return TRUE;

		case FIELDTYPE_DATA_BIT:
			if(!FillBaseStat(table, row, column, &dwBuffer, sizeof(DWORD)))
				(*argv) = JSVAL_VOID;
			else
				(*argv) = INT_TO_JSVAL(dwBuffer);
				//(*argv) = (BOOLEAN_TO_JSVAL(!!dwBuffer) ? 1 : 0);
			return TRUE;

		case FIELDTYPE_ASCII_TO_CODE:
		case FIELDTYPE_DATA_RAW:
			szBuffer = new char[5];
			memset(szBuffer, NULL, 5);
			if(!FillBaseStat(table, row, column, szBuffer, 5))
				(*argv) = JSVAL_VOID;
			else
				(*argv) = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, szBuffer));
			delete[] szBuffer;
			return TRUE;

		case FIELDTYPE_MONSTER_COMPS:
			// ..? :E
			return FALSE;
	}
	return FALSE;
}
