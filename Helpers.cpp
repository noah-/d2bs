#include "D2BS.h"
#include "Core.h"
#include "Script.h"
#include "ScriptEngine.h"
#include "string.h"
#include "D2Handlers.h"
#include "Control.h"
#include "D2Ptrs.h"
#include "Helpers.h"
#include "DbgHelp.h"

wchar_t* AnsiToUnicode(const char* str)
{
	wchar_t* buf = NULL;
	int len = MultiByteToWideChar(CP_ACP, 0, str, -1, buf, 0);
	buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, str, -1, buf, len);
	return buf;
}

char* UnicodeToAnsi(const wchar_t* str)
{
	char* buf = NULL;
	int len = WideCharToMultiByte(CP_ACP, 0, str, -1, buf, 0, "?", NULL);
	buf = new char[len];
	WideCharToMultiByte(CP_ACP, 0, str, -1, buf, len, "?", NULL);
	return buf;
}

bool StringToBool(const char* str)
{
	switch(tolower(str[0]))
	{
		case 't': case '1':
			return true;
		case 'f': case '0': default:
			return false;
	}
}

void StringReplace(char* str, const char find, const char replace, size_t buflen)
{
	for(size_t i = 0; i < buflen; i++)
	{
		if(str[i] == find)
			str[i] = replace;
	}
}

bool SwitchToProfile(const char* profile)
{
	if(Vars.bUseProfileScript != TRUE || !ProfileExists(profile))
		return false;

	char file[_MAX_FNAME+_MAX_PATH] = "",
		 defaultStarter[_MAX_FNAME] = "",
		 defaultGame[_MAX_FNAME] = "",
		 scriptPath[_MAX_PATH] = "";
	sprintf_s(file, sizeof(file), "%sd2bs.ini", Vars.szPath);

	GetPrivateProfileString(profile, "ScriptPath", "scripts", scriptPath, _MAX_PATH, file);
	GetPrivateProfileString(profile, "DefaultGameScript", "", defaultGame, _MAX_FNAME, file);
	GetPrivateProfileString(profile, "DefaultStarterScript", "", defaultStarter, _MAX_FNAME, file);

	strcpy_s(Vars.szProfile, 256, profile);
	sprintf_s(Vars.szScriptPath, _MAX_PATH, "%s%s", Vars.szPath, scriptPath);
	if(strlen(defaultGame) > 0)
		strcpy_s(Vars.szDefault, _MAX_FNAME, defaultGame);
	if(strlen(defaultStarter) > 0)
		strcpy_s(Vars.szStarter, _MAX_FNAME, defaultStarter);

	Vars.bUseProfileScript = FALSE;
	Reload();
	return true;
}

bool ProfileExists(const char *profile)
{
	char file[_MAX_FNAME+_MAX_PATH], profiles[65535] = "";
	sprintf_s(file, sizeof(file), "%sd2bs.ini", Vars.szPath);

	int count = GetPrivateProfileString(NULL, NULL, NULL, profiles, 65535, file);
	if(count > 0)
	{
		int i = 0;
		while(i < count)
		{
			if(_strcmpi(profiles+i, profile) == 0)
				return true;

			i += strlen(profiles+i)+1;
		}
	}
	return false;
}

void InitSettings(void)
{
	char fname[_MAX_FNAME+MAX_PATH], scriptPath[_MAX_PATH], defaultStarter[_MAX_FNAME], defaultGame[_MAX_FNAME],
		 debug[6], quitOnHostile[6], quitOnError[6], maxGameTime[6], gameTimeout[6],
		 startAtMenu[6], disableCache[6], memUsage[6], gamePrint[6], useProfilePath[6], logConsole[6];

	sprintf_s(fname, sizeof(fname), "%sd2bs.ini", Vars.szPath);

	GetPrivateProfileString("settings", "ScriptPath", "scripts", scriptPath, _MAX_PATH, fname);
	GetPrivateProfileString("settings", "DefaultGameScript", "default.dbj", defaultGame, _MAX_FNAME, fname);
	GetPrivateProfileString("settings", "DefaultStarterScript", "starter.dbj", defaultStarter, _MAX_FNAME, fname);
	GetPrivateProfileString("settings", "MaxGameTime", "0", maxGameTime, 6, fname);
	GetPrivateProfileString("settings", "Debug", "false", debug, 6, fname);
	GetPrivateProfileString("settings", "QuitOnHostile", "false", quitOnHostile, 6, fname);
	GetPrivateProfileString("settings", "QuitOnError", "false", quitOnError, 6, fname);
	GetPrivateProfileString("settings", "StartAtMenu", "true", startAtMenu, 6, fname);
	GetPrivateProfileString("settings", "DisableCache", "true", disableCache, 6, fname);
	GetPrivateProfileString("settings", "MemoryLimit", "50", memUsage, 6, fname);
	GetPrivateProfileString("settings", "UseGamePrint", "false", gamePrint, 6, fname);
	GetPrivateProfileString("settings", "GameReadyTimeout", "5", gameTimeout, 6, fname);
	GetPrivateProfileString("settings", "UseProfileScript", "false", useProfilePath, 6, fname);
	GetPrivateProfileString("settings", "LogConsoleOutput", "false", logConsole, 6, fname);

	sprintf_s(Vars.szScriptPath, _MAX_PATH, "%s%s", Vars.szPath, scriptPath);
	strcpy_s(Vars.szStarter, _MAX_FNAME, defaultStarter);
	strcpy_s(Vars.szDefault, _MAX_FNAME, defaultGame);

	Vars.dwGameTime = GetTickCount();
	Vars.dwMaxGameTime = abs(atoi(maxGameTime) * 1000);
	Vars.dwGameTimeout = abs(atoi(gameTimeout) * 1000);

	Vars.bQuitOnHostile = StringToBool(quitOnHostile);
	Vars.bQuitOnError = StringToBool(quitOnError);
	Vars.bStartAtMenu = StringToBool(startAtMenu);
	Vars.bDisableCache = StringToBool(disableCache);
	Vars.bUseGamePrint = StringToBool(gamePrint);
	Vars.bUseProfileScript = StringToBool(useProfilePath);
	Vars.bLogConsole = StringToBool(logConsole);

	Vars.dwMemUsage = abs(atoi(memUsage));
	if(Vars.dwMemUsage < 1)
		Vars.dwMemUsage = 50;
	Vars.dwMemUsage *= 1024*1024;
	Vars.oldWNDPROC = NULL;
}

bool InitHooks(void)
{
	int i = 0;
	while(!Vars.bActive)
	{
		if(i >= 300)
		{
			MessageBox(0, "Failed to set hooks, exiting!", "D2BS", 0);
			return false;
		}

		if(D2GFX_GetHwnd() && (ClientState() == ClientStateMenu || ClientState() == ClientStateInGame))
		{
			Vars.oldWNDPROC = (WNDPROC)SetWindowLong(D2GFX_GetHwnd(), GWL_WNDPROC, (LONG)GameEventHandler);
			if(!Vars.oldWNDPROC)
				continue;

			Vars.uTimer = SetTimer(D2GFX_GetHwnd(), 1, 0, TimerProc);

			DWORD mainThread = GetWindowThreadProcessId(D2GFX_GetHwnd(),0);
			if(mainThread)
			{
				if(!Vars.hKeybHook)
					Vars.hKeybHook = SetWindowsHookEx(WH_KEYBOARD, KeyPress, NULL, mainThread);
				if(!Vars.hMouseHook)
					Vars.hMouseHook = SetWindowsHookEx(WH_MOUSE, MouseMove, NULL, mainThread);
			}
		}
		else
			continue;

		if(Vars.hKeybHook && Vars.hMouseHook)
		{
			if(!ScriptEngine::Startup())
				return false;

			Vars.bActive = TRUE;

			if(ClientState() == ClientStateMenu && Vars.bStartAtMenu)
				clickControl(*p_D2WIN_FirstControl);
		}
		Sleep(50);
		i++;
	}
	return true;
}

const char* GetStarterScriptName(void)
{
	return (ClientState() == ClientStateInGame ? Vars.szDefault : ClientState() == ClientStateMenu ? Vars.szStarter : NULL);
}

ScriptState GetStarterScriptState(void)
{
	// the default return is InGame because that's the least harmful of the options
	return (ClientState() == ClientStateInGame ? InGame : ClientState() == ClientStateMenu ? OutOfGame : InGame);
}

bool ExecCommand(const char* command)
{
	ScriptEngine::RunCommand(command);
	return true;
}

bool StartScript(const char* scriptname, ScriptState state)
{
	char file[_MAX_FNAME+_MAX_PATH];
	sprintf_s(file, _MAX_FNAME+_MAX_PATH, "%s\\%s", Vars.szScriptPath, scriptname);
	Script* script = ScriptEngine::CompileFile(file, state);
	return (script && CreateThread(0, 0, ScriptThread, script, 0, 0) != INVALID_HANDLE_VALUE);
}

void Reload(void)
{
	if(ScriptEngine::GetCount() > 0)
		Print("ÿc2D2BSÿc0 :: Stopping all scripts");
	ScriptEngine::StopAll();

	if(Vars.bDisableCache != TRUE)
		Print("ÿc2D2BSÿc0 :: Flushing the script cache");
	ScriptEngine::FlushCache();

	// wait for things to catch up
	Sleep(500);

	if(!Vars.bUseProfileScript)
	{
		const char* script = GetStarterScriptName();
		if(StartScript(script, GetStarterScriptState()))
			Print("ÿc2D2BSÿc0 :: Started %s", script);
		else
			Print("ÿc2D2BSÿc0 :: Failed to start %s", script);
	}
}

bool ProcessCommand(const char* command, bool unprocessedIsCommand)
{
	bool result = false;
	char* buf = _strdup(command);
	char* next_token1 = NULL;
	char* argv = strtok_s(buf, " ", &next_token1);

	// no command?
	if(argv == NULL)
		return false;

	if(_strcmpi(argv, "start") == 0)
	{
		const char* script = GetStarterScriptName();
		if(StartScript(script, GetStarterScriptState()))
			Print("ÿc2D2BSÿc0 :: Started %s", script);
		else
			Print("ÿc2D2BSÿc0 :: Failed to start %s", script);
		result = true;
	}
	else if(_strcmpi(argv, "stop") == 0)
	{
		if(ScriptEngine::GetCount() > 0)
			Print("ÿc2D2BSÿc0 :: Stopping all scripts");
		ScriptEngine::StopAll();
		result = true;
	}
	else if(_strcmpi(argv, "flush") == 0)
	{
		if(Vars.bDisableCache != TRUE)
			Print("ÿc2D2BSÿc0 :: Flushing the script cache");
		ScriptEngine::FlushCache();
		result = true;
	}
	else if(_strcmpi(argv, "load") == 0)
	{
		const char* script = command+5;
		if(StartScript(script, GetStarterScriptState()))
			Print("ÿc2D2BSÿc0 :: Started %s", script);
		else
			Print("ÿc2D2BSÿc0 :: Failed to start %s", script);
		result = true;
	}
	else if(_strcmpi(argv, "reload") == 0)
	{
		Reload();
		result = true;
	}
#if DEBUG
	else if(_strcmpi(argv, "crash") == 0)
	{
		DWORD zero = 0;
		double value = 1/zero;
		Print("%d", value);
	}
	else if(_strcmpi(argv, "profile") == 0)
	{
		const char* profile = command+8;
		if(SwitchToProfile(profile))
			Print("ÿc2D2BSÿc0 :: Switched to profile %s", profile);
		else
			Print("ÿc2D2BSÿc0 :: Profile %s not found", profile);
		result = true;
	}
#endif
	else if(_strcmpi(argv, "exec") == 0 && !unprocessedIsCommand)
	{
		ExecCommand(command+5);
		result = true;
	}
	else if(unprocessedIsCommand)
	{
		ExecCommand(command);
		result = true;
	}
	return result;
}

void GameJoined(void)
{
	if(!Vars.bUseProfileScript)
	{
		const char* starter = GetStarterScriptName();
		if(starter != NULL)
		{
			Print("ÿc2D2BSÿc0 :: Starting %s", starter);
			if(StartScript(starter, GetStarterScriptState()))
				Print("ÿc2D2BSÿc0 :: %s running.", starter);
			else
				Print("ÿc2D2BSÿc0 :: Failed to start %s!", starter);
		}
	}
}

void MenuEntered(bool beginStarter)
{
	if(beginStarter && !Vars.bUseProfileScript)
	{
		const char* starter = GetStarterScriptName();
		if(starter != NULL)
		{
			Print("ÿc2D2BSÿc0 :: Starting %s", starter);
			if(StartScript(starter, GetStarterScriptState()))
				Print("ÿc2D2BSÿc0 :: %s running.", starter);
			else
				Print("ÿc2D2BSÿc0 :: Failed to start %s!", starter);
		}
	}
}

SYMBOL_INFO* GetSymFromAddr(HANDLE hProcess, DWORD64 addr)
{
	char* symbols = new char[sizeof(SYMBOL_INFO) + 512];
	memset(symbols, 0, sizeof(SYMBOL_INFO) + 512);

	SYMBOL_INFO* sym = (SYMBOL_INFO*)(symbols);
	sym->SizeOfStruct = sizeof(SYMBOL_INFO);
	sym->MaxNameLen = 512;

	DWORD64 dummy;
	bool success = SymFromAddr(hProcess, addr, &dummy, sym) == TRUE ? true : false;
	if(!success)
	{
		delete[] symbols;
		sym = NULL;
	}

	return sym;
}

IMAGEHLP_LINE64* GetLineFromAddr(HANDLE hProcess, DWORD64 addr)
{
	IMAGEHLP_LINE64* line = new IMAGEHLP_LINE64;
	line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);

	DWORD dummy;
	bool success = SymGetLineFromAddr64(hProcess, addr, &dummy, line) == TRUE ? true : false;
	if(!success)
	{
		delete line;
		line = NULL;
	}
	return line;
}

LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS* ptrs)
{
	EXCEPTION_RECORD* rec = ptrs->ExceptionRecord;
	CONTEXT* ctx = ptrs->ContextRecord;
	DWORD base = Vars.pModule ? Vars.pModule->dwBaseAddress : (DWORD)Vars.hModule;

	char path[MAX_PATH+_MAX_FNAME] = "";
	sprintf_s(path, sizeof(path), "%s\\D2BS.bin", Vars.szPath);

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, GetCurrentProcessId());
	HANDLE hThread = GetCurrentThread();
	CONTEXT context = *ctx;

	SymSetOptions(SYMOPT_LOAD_LINES|SYMOPT_FAIL_CRITICAL_ERRORS|SYMOPT_NO_PROMPTS|SYMOPT_DEFERRED_LOADS);
	SymInitialize(hProcess, Vars.szPath, TRUE);
	SymLoadModule64(hProcess, NULL, path, NULL, base, 0);

	STACKFRAME64 stack;
	stack.AddrPC.Offset = context.Eip;
	stack.AddrPC.Mode = AddrModeFlat;
	stack.AddrStack.Offset = context.Esp;
	stack.AddrStack.Mode = AddrModeFlat;
	stack.AddrFrame.Offset = context.Ebp;
	stack.AddrFrame.Mode = AddrModeFlat;

	std::string trace;

	for(int i = 0; i < 64; i++)
	{
		if(!StackWalk64(IMAGE_FILE_MACHINE_I386, hProcess, hThread, &stack, &context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
			break;

		// infinite loop
		if(stack.AddrReturn.Offset == stack.AddrPC.Offset)
			break;
		// jump to 0
		if(stack.AddrPC.Offset == 0)
			break;

		SYMBOL_INFO* sym = GetSymFromAddr(hProcess, stack.AddrPC.Offset);

		if(sym)
		{
			char msg[1024];
			ULONG64 base2 = (sym->Address - sym->ModBase);

			IMAGEHLP_LINE64* line = GetLineFromAddr(hProcess, stack.AddrPC.Offset);
			if(line)
				sprintf_s(msg, 1024, "\t%s+0x%08x, File: %s line %d\n",
						sym->Name, base2, strrchr(line->FileName, '\\')+1, line->LineNumber);
			else
				sprintf_s(msg, 1024, "\t%s+0x%08x\n", sym->Name, base2);

			trace.append(msg);
			delete line;
		}
		else
		{
			char addr[100];
			sprintf_s(addr, sizeof(addr), "\t0x%08x\n", stack.AddrFrame.Offset);
			trace.append(addr);
		}

		delete[] (char*)sym;
	}

	SYMBOL_INFO* sym = GetSymFromAddr(hProcess, (DWORD64)rec->ExceptionAddress);

	IMAGEHLP_LINE64* line = GetLineFromAddr(hProcess, (DWORD64)rec->ExceptionAddress);

	Log("EXCEPTION!\n*** 0x%08x at 0x%08x (%s in %s line %d)\n"
		"D2BS loaded at: 0x%08x\n"
		"Registers:\n"
		"\tEIP: 0x%08x, ESP: 0x%08x\n"
		"\tCS: 0x%04x, DS: 0x%04x, ES: 0x%04x, SS: 0x%04x, FS: 0x%04x, GS: 0x%04x\n"
		"\tEAX: 0x%08x, EBX: 0x%08x, ECX: 0x%08x, EDX: 0x%08x, ESI: 0x%08x, EDI: 0x%08x, EBP: 0x%08x, FLG: 0x%08x\n"
		"Stack Trace:\n%s\nEnd of stack trace.",
			rec->ExceptionCode, rec->ExceptionAddress,
			sym != NULL ? sym->Name : "Unknown", line != NULL ? strrchr(line->FileName, '\\')+1 : "Unknown", line != NULL ? line->LineNumber : 0,
			base,
			ctx->Eip, ctx->Esp,
			ctx->SegCs, ctx->SegDs, ctx->SegEs, ctx->SegSs, ctx->SegFs, ctx->SegGs,
			ctx->Eax, ctx->Ebx, ctx->Ecx, ctx->Edx, ctx->Esi, ctx->Edi, ctx->Ebp, ctx->EFlags,
			trace.c_str());

	delete[] (char*)sym;
	delete line;

	SymCleanup(hProcess);

	return EXCEPTION_EXECUTE_HANDLER;
}
