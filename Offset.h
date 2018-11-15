#ifndef _OFFSET_H
#define _OFFSET_H

#define INST_INT3	0xCC
#define INST_CALL	0xE8
#define INST_NOP	0x90
#define INST_JMP	0xE9
#define INST_RET	0xC3

typedef struct PatchHook_t
{
	void (*pFunc)(DWORD, DWORD, DWORD);
	DWORD dwAddr;
	DWORD dwFunc;
	DWORD dwLen;
	BYTE *bOldCode;
} PatchHook;


void DefineOffsets();
DWORD GetDllOffset(int num);
DWORD GetDllOffset(char *DllName, int Offset);

PatchHook *RetrievePatchHooks(PINT pBuffer);
void PatchBytes(DWORD dwAddr, DWORD dwValue, DWORD dwLen);
void PatchJmp(DWORD dwAddr, DWORD dwFunc, DWORD dwLen);
void PatchCall(DWORD dwAddr, DWORD dwFunc, DWORD dwLen);
void InterceptLocalCode(BYTE bInst, DWORD pAddr, DWORD pFunc, DWORD dwLen);
void FillBytes(void *pAddr, BYTE bFill, DWORD dwLen);
BOOL WriteBytes(void *pAddr, void *pData, DWORD dwLen);
void RemovePatches();
void InstallPatches();
void InstallConditional();
void RemoveConditional();


#endif