#define _DEFINE_VARS
#define __D2PTRS_ONCE_
#include "D2Ptrs.h"
#undef __D2PTRS_H__
#define __D2PTRS_LIST_
#include "D2Ptrs.h"
#include "Patch.h"
#include "D2BS.h"

#ifndef ArraySize
#define ArraySize(x) (sizeof((x)) / sizeof((x)[0]))
#endif

void DefineOffsets()
{
	DWORD **p = (DWORD **)d2ptrs_list;
	do {
		**p = GetDllOffset(**p);
	} while(ptrdiff_t(++p) < ((ptrdiff_t)d2ptrs_list) + sizeof(d2ptrs_list));
}

DWORD GetDllOffset(char *DllName, int Offset)
{
	HMODULE hMod = GetModuleHandle(NULL);

	if (Offset < 0)
		return (DWORD)GetProcAddress(hMod, (LPCSTR)(-Offset));

	return ((DWORD)hMod) + Offset;
}

DWORD GetDllOffset(int num)
{
	static char *dlls[] = {"D2Client.DLL", "D2Common.DLL", "D2Gfx.DLL", "D2Lang.DLL", 
			       "D2Win.DLL", "D2Net.DLL", "D2Game.DLL", "D2Launch.DLL", "Fog.DLL", "BNClient.DLL",
					"Storm.DLL", "D2Cmp.DLL", "D2Multi.DLL"};
	if((num&0xff) > 12)
			return 0;
	return GetDllOffset(dlls[num&0xff], num>>8);
}

void InstallPatches()
{
	
	for(int x = 0; x < ArraySize(Patches); x++)
	{
		Patches[x].bOldCode = new BYTE[Patches[x].dwLen];
		::ReadProcessMemory(GetCurrentProcess(), (void*)Patches[x].dwAddr, Patches[x].bOldCode, Patches[x].dwLen, NULL);
		Patches[x].pFunc(Patches[x].dwAddr, Patches[x].dwFunc, Patches[x].dwLen);
	}
	
}

void RemovePatches()
{
	
	for(int x = 0; x < ArraySize(Patches); x++)
	{
		WriteBytes((void*)Patches[x].dwAddr, Patches[x].bOldCode, Patches[x].dwLen);
		delete[] Patches[x].bOldCode;
	}
	
}

void InstallConditional()
{
	if (Vars.bUseRawCDKey == 1)
	{
		for (int x = 0; x < 2; x++)
		{
			Conditional[x].bOldCode = new BYTE[Conditional[x].dwLen];
			::ReadProcessMemory(GetCurrentProcess(), (void*)Conditional[x].dwAddr, Conditional[x].bOldCode, Conditional[x].dwLen, NULL);
			Conditional[x].pFunc(Conditional[x].dwAddr, Conditional[x].dwFunc, Conditional[x].dwLen);
		}
	}

	if (Vars.bMulti)
	{
		for (int x = 2; x < 5; x++)
		{
			Conditional[x].bOldCode = new BYTE[Conditional[x].dwLen];
			::ReadProcessMemory(GetCurrentProcess(), (void*)Conditional[x].dwAddr, Conditional[x].bOldCode, Conditional[x].dwLen, NULL);
			Conditional[x].pFunc(Conditional[x].dwAddr, Conditional[x].dwFunc, Conditional[x].dwLen);
		}
	}

	if (Vars.bCacheFix)
	{
		for (int x = 5; x < 7; x++)
		{
			Conditional[x].bOldCode = new BYTE[Conditional[x].dwLen];
			::ReadProcessMemory(GetCurrentProcess(), (void*)Conditional[x].dwAddr, Conditional[x].bOldCode, Conditional[x].dwLen, NULL);
			Conditional[x].pFunc(Conditional[x].dwAddr, Conditional[x].dwFunc, Conditional[x].dwLen);
		}
	}

	if (Vars.bSleepy)
	{
		for (int x = 7; x < 9; x++)
		{
			Conditional[x].bOldCode = new BYTE[Conditional[x].dwLen];
			::ReadProcessMemory(GetCurrentProcess(), (void*)Conditional[x].dwAddr, Conditional[x].bOldCode, Conditional[x].dwLen, NULL);
			Conditional[x].pFunc(Conditional[x].dwAddr, Conditional[x].dwFunc, Conditional[x].dwLen);
		}
	}
}

void RemoveConditional()
{
	if(Vars.bUseRawCDKey)
	{
	
		for(int x = 0; x < ArraySize(Conditional); x++)
		{
			WriteBytes((void*)Conditional[x].dwAddr, Conditional[x].bOldCode, Conditional[x].dwLen);
			delete[] Conditional[x].bOldCode;
		}

	}
}

BOOL WriteBytes(void *pAddr, void *pData, DWORD dwLen)
{
	DWORD dwOld;

	if(!VirtualProtect(pAddr, dwLen, PAGE_READWRITE, &dwOld))
		return FALSE;

	::memcpy(pAddr, pData, dwLen);
	return VirtualProtect(pAddr, dwLen, dwOld, &dwOld);
}

void FillBytes(void *pAddr, BYTE bFill, DWORD dwLen)
{
	BYTE *bCode = new BYTE[dwLen];
	::memset(bCode, bFill, dwLen);

	WriteBytes(pAddr, bCode, dwLen);

	delete[] bCode;
}

void InterceptLocalCode(BYTE bInst, DWORD pAddr, DWORD pFunc, DWORD dwLen)
{
	BYTE *bCode = new BYTE[dwLen];
	::memset(bCode, 0x90, dwLen);
	DWORD dwFunc = pFunc - (pAddr + 5);

	bCode[0] = bInst;
	*(DWORD *)&bCode[1] = dwFunc;
	WriteBytes((void*)pAddr, bCode, dwLen);

	delete[] bCode;
}

void PatchCall(DWORD dwAddr, DWORD dwFunc, DWORD dwLen)
{
	InterceptLocalCode(INST_CALL, dwAddr, dwFunc, dwLen);
}

void PatchJmp(DWORD dwAddr, DWORD dwFunc, DWORD dwLen)
{
	InterceptLocalCode(INST_JMP, dwAddr, dwFunc, dwLen);
}

void PatchBytes(DWORD dwAddr, DWORD dwValue, DWORD dwLen)
{
	BYTE *bCode = new BYTE[dwLen];
	::memset(bCode, (BYTE)dwValue, dwLen);

	WriteBytes((LPVOID)dwAddr, bCode, dwLen);

	delete[] bCode;
}

PatchHook *RetrievePatchHooks(PINT pBuffer)
{
	*pBuffer = ArraySize(Patches);
	return &Patches[0];
}
