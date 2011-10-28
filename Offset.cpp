#define _DEFINE_VARS
#include "D2Ptrs.h"
#include "Patch.h"

#ifndef ArraySize
#define ArraySize(x) (sizeof((x)) / sizeof((x)[0]))
#endif

void DefineOffsets()
{
	DWORD *p = (DWORD *)&_D2PTRS_START;
	do {
		*p = GetDllOffset(*p);
	} while(++p <= (DWORD *)&_D2PTRS_END);
}

DWORD GetDllOffset(char *DllName, int Offset)
{
	HMODULE hMod = GetModuleHandle(DllName);

	if(!hMod)
		hMod = LoadLibrary(DllName);

	if(!hMod)
		return 0;

	if(Offset < 0)
		return (DWORD)GetProcAddress(hMod, (LPCSTR)(-Offset));
	
	return ((DWORD)hMod)+Offset;
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
