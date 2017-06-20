// Diablo II Botting System Core

#include <shlwapi.h>
#include <io.h>
#include <fcntl.h>

#include "dde.h"
#include "Offset.h"
#include "ScriptEngine.h"
#include "Helpers.h"
#include "D2Handlers.h"
#include "Console.h"
#include "D2BS.h"
#include "D2Ptrs.h"

#ifdef _MSVC_DEBUG
#include "D2Loader.h"
#endif

static HANDLE hD2Thread = INVALID_HANDLE_VALUE;
static HANDLE hEventThread = INVALID_HANDLE_VALUE;
BOOL WINAPI DllMain(HINSTANCE hDll, DWORD dwReason, LPVOID lpReserved)
{
	switch(dwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			DisableThreadLibraryCalls(hDll);
			if(lpReserved != NULL)
			{
				Vars.pModule = (Module*)lpReserved;

				if(!Vars.pModule)
					return FALSE;

				strcpy_s(Vars.szPath, MAX_PATH, Vars.pModule->szPath);
				Vars.bLoadedWithCGuard = TRUE;
			}
			else
			{
				Vars.hModule = hDll;
				GetModuleFileName(hDll, Vars.szPath, MAX_PATH);
				PathRemoveFileSpec(Vars.szPath);
				strcat_s(Vars.szPath, MAX_PATH, "\\");
				Vars.bLoadedWithCGuard = FALSE;
			}

#if 0
			char errlog[516] = "";
			sprintf_s(errlog, 516, "%sd2bs.log", Vars.szPath);
			AllocConsole();
			int handle = _open_osfhandle((long)GetStdHandle(STD_ERROR_HANDLE), _O_TEXT);
			FILE* f = _fdopen(handle, "wt");
			*stderr = *f;
			setvbuf(stderr, NULL, _IONBF, 0);
			freopen_s(&f, errlog, "a+t", f);
#endif

			Vars.bShutdownFromDllMain = FALSE;
			SetUnhandledExceptionFilter(ExceptionHandler);
			if(!Startup())
				return FALSE;
		}
		break;
		case DLL_PROCESS_DETACH:
			if(Vars.bNeedShutdown)
			{
				Vars.bShutdownFromDllMain = TRUE;
				Shutdown();
			}
		break;
	}

	return TRUE;
}

BOOL Startup(void)
{
	InitializeCriticalSection(&Vars.cEventSection);
	InitializeCriticalSection(&Vars.cRoomSection);
	InitializeCriticalSection(&Vars.cMiscSection);
	InitializeCriticalSection(&Vars.cScreenhookSection);
	InitializeCriticalSection(&Vars.cPrintSection);
	InitializeCriticalSection(&Vars.cBoxHookSection);
	InitializeCriticalSection(&Vars.cFrameHookSection);
	InitializeCriticalSection(&Vars.cLineHookSection);
	InitializeCriticalSection(&Vars.cImageHookSection);
	InitializeCriticalSection(&Vars.cTextHookSection);
	InitializeCriticalSection(&Vars.cFlushCacheSection);
	InitializeCriticalSection(&Vars.cConsoleSection);
	InitializeCriticalSection(&Vars.cGameLoopSection);
	InitializeCriticalSection(&Vars.cUnitListSection);
	InitializeCriticalSection(&Vars.cFileSection);

	Vars.bNeedShutdown = TRUE;	
	Vars.bChangedAct = FALSE;
	Vars.bGameLoopEntered = FALSE;

	Vars.SectionCount = 0;

	//MessageBox(NULL, "qwe", "qwe", MB_OK);
	Genhook::Initialize();
	DefineOffsets();
	InstallPatches();
	CreateDdeServer();

	if((hD2Thread = CreateThread(NULL, NULL, D2Thread, NULL, NULL, NULL)) == NULL)
		return FALSE;
//	hEventThread = CreateThread(0, 0, EventThread, NULL, 0, 0);
	return TRUE;
}

void Shutdown(void)
{
	if(!Vars.bNeedShutdown)
		return;

	Vars.bActive = FALSE;
	if(!Vars.bShutdownFromDllMain)
		WaitForSingleObject(hD2Thread, INFINITE);

	SetWindowLong(D2GFX_GetHwnd(),GWL_WNDPROC,(LONG)Vars.oldWNDPROC);

	RemovePatches();
	Genhook::Destroy();
	ShutdownDdeServer();

	KillTimer(D2GFX_GetHwnd(), Vars.uTimer);

	UnhookWindowsHookEx(Vars.hMouseHook);
	UnhookWindowsHookEx(Vars.hKeybHook);

	DeleteCriticalSection(&Vars.cRoomSection);
	DeleteCriticalSection(&Vars.cMiscSection);
	DeleteCriticalSection(&Vars.cScreenhookSection);
	DeleteCriticalSection(&Vars.cPrintSection);
	DeleteCriticalSection(&Vars.cBoxHookSection);
	DeleteCriticalSection(&Vars.cFrameHookSection);
	DeleteCriticalSection(&Vars.cLineHookSection);
	DeleteCriticalSection(&Vars.cImageHookSection);
	DeleteCriticalSection(&Vars.cTextHookSection);
	DeleteCriticalSection(&Vars.cFlushCacheSection);
	DeleteCriticalSection(&Vars.cConsoleSection);
	DeleteCriticalSection(&Vars.cGameLoopSection);
	DeleteCriticalSection(&Vars.cUnitListSection);
	DeleteCriticalSection(&Vars.cFileSection);

	Log("D2BS Shutdown complete.");
	Vars.bNeedShutdown = false;
}
