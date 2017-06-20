
#include "D2Handlers.h"
#include "D2Ptrs.h"
#include "D2BS.h"
#include "Helpers.h"
#include <shlwapi.h>

void __declspec(naked) RealmPacketRecv_Interception()
{
	__asm
	{
		LEA ECX,DWORD PTR SS:[ESP+4]
		PUSHAD
		CALL RealmPacketRecv
		CMP EAX, 0
		POPAD
		JE Block
		CALL EAX
Block:
		RETN
	}
}

void __declspec(naked) GamePacketReceived_Intercept()
{
	__asm
	{
		pushad;

		call GamePacketReceived;
		test eax, eax;

		popad;
		jnz OldCode;

		mov edx, 0;

	OldCode:
		mov eax, [esp + 4];
		push eax;
		call D2NET_ReceivePacket_I;

		ret 4;
	}

}

void __declspec(naked) GamePacketSent_Interception() 
{
	__asm
	{
		pushf
		pushad;

		mov ecx, [esp + 0x22 + 4 + 0xC];
		mov edx, [esp + 0x22 + 4 + 0x4];
		call GamePacketSent;
		test eax, eax;

		popad;
		jnz OldCode;

		mov[esp + 0x4 + 4 + 2], 0;

	OldCode:
		popf
			jnz good
			xor eax, eax
			pop ebp
			ret 0Ch
			good :
		jmp D2CLIENT_SendPacket_II
	}
}

void __declspec(naked) GameDraw_Intercept()
{
	__asm
	{
		call GameDraw;
		ret

		POP ESI
		POP EBX
		POP ECX
		RETN 4
	}
}

void __declspec(naked) GameInput_Intercept()
{
	__asm {
		pushad
		mov ecx, ebx
		call GameInput
		cmp eax, -1
		popad
		je BlockIt
		call D2CLIENT_InputCall_I
		ret

BlockIt:

		xor eax,eax
		ret
	}
}

UnitAny* GetSelectedUnit_Intercept(void)
{
	if(Vars.bClickAction)
	{
		if(Vars.dwSelectedUnitId)
		{
			UnitAny* pUnit = D2CLIENT_FindUnit(Vars.dwSelectedUnitId, Vars.dwSelectedUnitType);

			return pUnit;
		}

		return NULL;
	}

	return D2CLIENT_GetSelectedUnit();
}

void __declspec(naked) Whisper_Intercept()
{
	__asm
	{
		mov [esp-674h-4+4], edi
		pop edi
		sub esp, 678h
		pushad
		mov ecx, ebx
		mov edx, [ebp+8]
		call WhisperHandler
		popad
		//jmp D2MULTI_WhisperIntercept_Jump
		jmp edi
	}
}
VOID __declspec(naked) ChatPacketRecv_Interception()
{
        __asm
        {
				mov     edx, [ebp + 8]
				mov     ecx, ebx
                pushad
				sub ecx, 4
				add edx, 4

                call ChatPacketRecv
                test eax, eax
                popad
 
                je Block
				call    edi
Block:
                ret
   }
}
void __declspec(naked) GameAttack_Intercept()
{
	__asm 
	{
		pushad
		mov ecx, esi // attack struct
		call GameAttack
		cmp eax, -1
		popad
		je OldCode

		call D2CLIENT_GetSelectedUnit
		
		cmp eax, 0
		je OldCode

		mov [esp+0x08+0x4+0x4], 1 // bool unit

OldCode:
		mov eax, [p_D2CLIENT_MouseY]
		mov eax, [eax]
		retn
	}
}

void __declspec(naked) PlayerAssignment_Intercept()
{
	__asm
	{
		FNOP
		CALL D2CLIENT_AssignPlayer_I
		MOV ECX, EAX
		CALL GamePlayerAssignment
		RETN
	}
}

void __declspec(naked) GameCrashFix_Intercept()
{
	__asm
	{
		CMP ECX, 0
		JE Skip
		MOV DWORD PTR DS:[ECX+0x10],EDX
Skip:
		MOV DWORD PTR DS:[EAX+0xC],0
		RETN
	}
}

void GameDrawOOG_Intercept(void)
{
	GameDrawOOG();
}

void __declspec(naked) GameActChange_Intercept(void)
{
	__asm
	{
		POP EAX
		PUSH ESI
		XOR ESI, ESI
		CMP [Vars.bChangedAct], 0
		MOV [Vars.bChangedAct], 0
		JMP EAX
	}
}

void __declspec(naked) GameActChange2_Intercept(void)
{
	__asm
	{
		MOV ESP, EBP
		POP EBP
		MOV [Vars.bChangedAct], 1
		retn
	}
}

void __declspec(naked) GameLeave_Intercept(void)
{
	__asm
	{
		call GameLeave
		jmp D2CLIENT_GameLeave_I
	}
}

void __declspec(naked) ChannelInput_Intercept(void)
{
	__asm
	{
		push ecx
		mov ecx, esi

		call ChannelInput

		test eax, eax
		pop ecx

		jz SkipInput
		call D2MULTI_ChannelInput_I

SkipInput:
		ret
	}
}

void __declspec(naked) AddUnit_Intercept(UnitAny* lpUnit)
{
	__asm
	{
		call [D2CLIENT_GameAddUnit_I]
		pushad
		push esi
		call AddUnit
		popad
		retn
	}
}

void __declspec(naked) RemoveUnit_Intercept(UnitAny* lpUnit)
{
	__asm
	{
		pushad
		push dword ptr ds:[esi+edx*4]
		call RemoveUnit
		popad
		mov eax,dword ptr ds:[ecx+0xE4]
		mov DWORD PTR ds:[esi+edx*4], eax
		retn
	}
}


VOID __declspec(naked) __fastcall ClassicSTUB()
{
	*p_BNCLIENT_ClassicKey = Vars.szClassic;
	__asm{
		
		jmp BNCLIENT_DClass;
	}
	
}

VOID __declspec(naked) __fastcall LodSTUB()
{ 
	*p_BNCLIENT_XPacKey = Vars.szLod;
	__asm{
		
		jmp BNCLIENT_DLod;
	}
	
}

void __declspec(naked) FailToJoin()
{
	__asm
	{
		cmp esi, 4000;
		ret;
	}
}

int EraseCacheFiles()
{

	CHAR path[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, path);

	CHAR szSearch[MAX_PATH];
	memset(szSearch, 0x00, MAX_PATH);

	strcpy_s(szSearch, path);
	PathAppendA(szSearch, "\\*.dat");

	WIN32_FIND_DATAA FindFileData;
	HANDLE  hFind = FindFirstFileA(szSearch, &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			char FilePath[MAX_PATH];
			memset(FilePath, 0x00, MAX_PATH);

			strcpy_s(FilePath, path);
			PathAppendA(FilePath, FindFileData.cFileName);
			DeleteFileA(FilePath);

		} while (FindNextFileA(hFind, &FindFileData));

		FindClose(hFind);
	}

	return 0;
}

HMODULE __stdcall Multi(LPSTR Class, LPSTR Window) { return 0; }

HANDLE __stdcall Windowname(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	CHAR szWindowName[100] = "D2";

		if (strlen(Vars.szTitle) > 1)
			strcpy_s(szWindowName, sizeof(szWindowName), Vars.szTitle);

	return CreateWindowExA(dwExStyle, lpClassName, szWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}


HANDLE __stdcall CacheFix(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	EraseCacheFiles();
	CHAR path[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, path);

	if (Vars.bCacheFix)
	{
		char szTitle[128];
		GetWindowText(D2GFX_GetHwnd(), szTitle, 128);
		CHAR Def[100] = "";

		if (strlen(szTitle) > 1)
		{
			sprintf_s(Def, "\\bncache%d.dat", szTitle);
			strcat_s(path, Def);
		}
		else
		{
			srand(GetTickCount());
			sprintf_s(Def, "\\bncache%d.dat", rand() % 0x2000);

			strcat_s(path, Def);
		}
	}
	else
		strcat_s(path, "\\bncache.dat");

	return CreateFileA(path, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

WINUSERAPI
int
WINAPI
MessageBoxA(
    __in_opt HWND hWnd,
    __in_opt LPCSTR lpText,
    __in_opt LPCSTR lpCaption,
    __in UINT uType);

int WINAPI LogMessageBoxA_Intercept(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption,
    UINT uType)
{
	char* dllAddrs;

	Log("Error message box, caption: \"%s\", message:\n%s\n%s", lpCaption,
		lpText, dllAddrs = DllLoadAddrStrs());

	free(dllAddrs);

	return MessageBoxA(hWnd, lpText, lpCaption, uType);
}
#include <DbgHelp.h>
LONG WINAPI MyUnhandledExceptionFilter(_In_ struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	// NOT WORKING ONE, WORKING ONE IS IN Helpers.cpp
	MessageBox(NULL, "QWE", "QWE", MB_OK);
	HANDLE hFile = INVALID_HANDLE_VALUE;
	for (int i = 0; hFile == INVALID_HANDLE_VALUE; ++i)
	{
		char fname[100];
		sprintf(fname, "Crash%03d.dump", i);
		hFile = CreateFile(fname, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	DWORD ProcessId = GetCurrentProcessId();
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS/*READ_CONTROL | PROCESS_VM_READ*/, TRUE, ProcessId);
	MINIDUMP_EXCEPTION_INFORMATION ExceptionParam;
	ExceptionParam.ThreadId = GetCurrentThreadId();
	ExceptionParam.ExceptionPointers = ExceptionInfo;
	ExceptionParam.ClientPointers = TRUE;
	MiniDumpWriteDump(hProcess, ProcessId, hFile, MiniDumpNormal, &ExceptionParam, NULL, NULL);
	CloseHandle(hFile);
	exit(0);
	return EXCEPTION_EXECUTE_HANDLER;
}

//  FogException(6, (int)&Default, a3, "Unrecoverable internal error %08x", a2);
void FogException()
{
	__try {
		RaiseException(
			1,                    // exception code 
			0,                    // continuable exception 
			0, NULL);
	}
	__except(UnhandledExceptionFilter(GetExceptionInformation())){
		exit(0);
		exit(0);
	}
}

char __fastcall ErrorReportLaunch(const char *crash_file, int a2)
{
	Log("Crash File: %s\n", crash_file);
	exit(0);
}