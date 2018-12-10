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
#include "Profile.h"
#include <wctype.h>

wchar_t* AnsiToUnicode(const char* str, UINT codepage) {
    wchar_t* buf = NULL;
    int len = MultiByteToWideChar(codepage, 0, str, -1, buf, 0);
    buf = new wchar_t[len];
    MultiByteToWideChar(codepage, 0, str, -1, buf, len);
    return buf;
}

char* UnicodeToAnsi(const wchar_t* str, UINT codepage) {
    char* buf = NULL;
    int len = WideCharToMultiByte(codepage, 0, str, -1, buf, 0, (codepage ? NULL : "?"), NULL);
    buf = new char[len];
    WideCharToMultiByte(codepage, 0, str, -1, buf, len, (codepage ? NULL : "?"), NULL);
    return buf;
}

void StringToLower(char* p) {
    for (; *p; ++p)
        *p = tolower(*p);
}
bool StringToBool(const char* str) {
    switch (tolower(str[0])) {
    case 't':
    case '1':
        return true;
    case 'f':
    case '0':
    default:
        return false;
    }
}

bool StringToBool(const wchar_t* str) {
    switch (tolower(str[0])) {
    case 't':
    case '1':
        return true;
    case 'f':
    case '0':
    default:
        return false;
    }
}

void StringReplace(char* str, const char find, const char replace, size_t buflen) {
    for (size_t i = 0; i < buflen; i++) {
        if (str[i] == find)
            str[i] = replace;
    }
}

void StringReplace(wchar_t* str, const wchar_t find, const wchar_t replace, size_t buflen) {
    for (size_t i = 0; i < buflen; i++) {
        if (str[i] == find)
            str[i] = replace;
    }
}

bool SwitchToProfile(const wchar_t* profile) {
    if (Vars.bUseProfileScript != TRUE || !Profile::ProfileExists(profile))
        return false;

    wchar_t file[_MAX_FNAME + _MAX_PATH] = L"", defaultStarter[_MAX_FNAME] = L"", defaultConsole[_MAX_FNAME] = L"", defaultGame[_MAX_FNAME] = L"", scriptPath[_MAX_PATH] = L"";
    swprintf_s(file, _MAX_FNAME + _MAX_PATH, L"%sd2bs.ini", Vars.szPath);

    GetPrivateProfileStringW(profile, L"ScriptPath", L"scripts", scriptPath, _MAX_PATH, file);
    GetPrivateProfileStringW(profile, L"DefaultConsoleScript", L"", defaultConsole, _MAX_FNAME, file);
    GetPrivateProfileStringW(profile, L"DefaultGameScript", L"", defaultGame, _MAX_FNAME, file);
    GetPrivateProfileStringW(profile, L"DefaultStarterScript", L"", defaultStarter, _MAX_FNAME, file);

    wcscpy_s(Vars.szProfile, 256, profile);
    swprintf_s(Vars.szScriptPath, _MAX_PATH, L"%s%s", Vars.szPath, scriptPath);
    if (wcslen(defaultConsole) > 0)
        wcscpy_s(Vars.szConsole, _MAX_FNAME, defaultConsole);
    if (wcslen(defaultGame) > 0)
        wcscpy_s(Vars.szDefault, _MAX_FNAME, defaultGame);
    if (wcslen(defaultStarter) > 0)
        wcscpy_s(Vars.szStarter, _MAX_FNAME, defaultStarter);

    Vars.bUseProfileScript = FALSE;
    // Reload();
    return true;
}

void InitSettings(void) {
    wchar_t fname[_MAX_FNAME + MAX_PATH], scriptPath[_MAX_PATH], defaultStarter[_MAX_FNAME], defaultGame[_MAX_FNAME], defaultConsole[_MAX_FNAME], debug[6], quitOnHostile[6],
        quitOnError[6], maxGameTime[6], gameTimeout[6], startAtMenu[6], disableCache[6], memUsage[6], gamePrint[6], useProfilePath[6], logConsole[6],
        enableUnsupported[6], forwardMessageBox[6], consoleFont[6];

    swprintf_s(fname, sizeof(fname), L"%sd2bs.ini", Vars.szPath);

    GetPrivateProfileStringW(L"settings", L"ScriptPath", L"scripts", scriptPath, _MAX_PATH, fname);
    GetPrivateProfileStringW(L"settings", L"DefaultConsoleScript", L"", defaultConsole, _MAX_FNAME, fname);
    GetPrivateProfileStringW(L"settings", L"DefaultGameScript", L"default.dbj", defaultGame, _MAX_FNAME, fname);
    GetPrivateProfileStringW(L"settings", L"DefaultStarterScript", L"starter.dbj", defaultStarter, _MAX_FNAME, fname);
    GetPrivateProfileStringW(L"settings", L"MaxGameTime", L"0", maxGameTime, 6, fname);
    GetPrivateProfileStringW(L"settings", L"Debug", L"false", debug, 6, fname);
    GetPrivateProfileStringW(L"settings", L"QuitOnHostile", L"false", quitOnHostile, 6, fname);
    GetPrivateProfileStringW(L"settings", L"QuitOnError", L"false", quitOnError, 6, fname);
    GetPrivateProfileStringW(L"settings", L"StartAtMenu", L"true", startAtMenu, 6, fname);
    GetPrivateProfileStringW(L"settings", L"DisableCache", L"true", disableCache, 6, fname);
    GetPrivateProfileStringW(L"settings", L"MemoryLimit", L"100", memUsage, 6, fname);
    GetPrivateProfileStringW(L"settings", L"UseGamePrint", L"false", gamePrint, 6, fname);
    GetPrivateProfileStringW(L"settings", L"GameReadyTimeout", L"5", gameTimeout, 6, fname);
    GetPrivateProfileStringW(L"settings", L"UseProfileScript", L"false", useProfilePath, 6, fname);
    GetPrivateProfileStringW(L"settings", L"LogConsoleOutput", L"false", logConsole, 6, fname);
    GetPrivateProfileStringW(L"settings", L"EnableUnsupported", L"false", enableUnsupported, 6, fname);
    GetPrivateProfileStringW(L"settings", L"ForwardMessageBox", L"false", forwardMessageBox, 6, fname);
    GetPrivateProfileStringW(L"settings", L"ConsoleFont", L"0", consoleFont, 6, fname);
    swprintf_s(Vars.szScriptPath, _MAX_PATH, L"%s%s", Vars.szPath, scriptPath);
    wcscpy_s(Vars.szStarter, _MAX_FNAME, defaultStarter);
    wcscpy_s(Vars.szConsole, _MAX_FNAME, defaultConsole);
    wcscpy_s(Vars.szDefault, _MAX_FNAME, defaultGame);

    Vars.dwGameTime = GetTickCount();
    Vars.dwMaxGameTime = abs(_wtoi(maxGameTime) * 1000);
    Vars.dwGameTimeout = abs(_wtoi(gameTimeout) * 1000);

    Vars.bQuitOnHostile = StringToBool(quitOnHostile);
    Vars.bQuitOnError = StringToBool(quitOnError);
    Vars.bStartAtMenu = StringToBool(startAtMenu);
    Vars.bDisableCache = StringToBool(disableCache);
    Vars.bUseGamePrint = StringToBool(gamePrint);
    Vars.bUseProfileScript = StringToBool(useProfilePath);
    Vars.bLogConsole = StringToBool(logConsole);
    Vars.bEnableUnsupported = StringToBool(enableUnsupported);
    Vars.bForwardMessageBox = StringToBool(forwardMessageBox);
    Vars.eventSignal = CreateEventA(false, true, false, nullptr);
    Vars.dwMemUsage = abs(_wtoi(memUsage));
    Vars.dwConsoleFont = abs(_wtoi(consoleFont));
    if (Vars.dwMemUsage < 1)
        Vars.dwMemUsage = 50;
    Vars.dwMemUsage *= 1024 * 1024;
    Vars.oldWNDPROC = NULL;
}

bool InitHooks(void) {
    int i = 0;
    while (!Vars.bActive) {
        if (i >= 300) {
            MessageBox(0, "Failed to set hooks, exiting!", "D2BS", 0);
            return false;
        }

        if (D2GFX_GetHwnd() && (ClientState() == ClientStateMenu || ClientState() == ClientStateInGame)) {
            if (!Vars.oldWNDPROC)
                Vars.oldWNDPROC = (WNDPROC)SetWindowLong(D2GFX_GetHwnd(), GWL_WNDPROC, (LONG)GameEventHandler);
            if (!Vars.oldWNDPROC)
                continue;

            Vars.uTimer = SetTimer(D2GFX_GetHwnd(), 1, 0, TimerProc);

            DWORD mainThread = GetWindowThreadProcessId(D2GFX_GetHwnd(), 0);
            if (mainThread) {
                if (!Vars.hKeybHook)
                    Vars.hKeybHook = SetWindowsHookEx(WH_KEYBOARD, KeyPress, NULL, mainThread);
                if (!Vars.hMouseHook)
                    Vars.hMouseHook = SetWindowsHookEx(WH_MOUSE, MouseMove, NULL, mainThread);
            }
        } else
            continue;

        if (Vars.hKeybHook && Vars.hMouseHook) {
            if (!ScriptEngine::Startup())
                return false;

            Vars.bActive = TRUE;

            if (ClientState() == ClientStateMenu && Vars.bStartAtMenu)
                clickControl(*p_D2WIN_FirstControl);
        }
        Sleep(50);
        i++;
    }
    return true;
}

const wchar_t* GetStarterScriptName(void) {
    return (ClientState() == ClientStateInGame ? Vars.szDefault : ClientState() == ClientStateMenu ? Vars.szStarter : NULL);
}

ScriptState GetStarterScriptState(void) {
    // the default return is InGame because that's the least harmful of the options
    return (ClientState() == ClientStateInGame ? InGame : ClientState() == ClientStateMenu ? OutOfGame : InGame);
}

bool ExecCommand(const char* command) {
    ScriptEngine::RunCommand(command);
    return true;
}

bool StartScript(const wchar_t* scriptname, ScriptState state) {
    wchar_t file[_MAX_FNAME + _MAX_PATH];
    swprintf_s(file, _MAX_FNAME + _MAX_PATH, L"%s\\%s", Vars.szScriptPath, scriptname);
    Script* script = ScriptEngine::CompileFile(file, state);
    return (script && script->BeginThread(ScriptThread));
}

void Reload(void) {
    if (ScriptEngine::GetCount() > 0)
        Print("ÿc2D2BSÿc0 :: Stopping all scripts");
    ScriptEngine::StopAll();

    if (Vars.bDisableCache != TRUE)
        Print("ÿc2D2BSÿc0 :: Flushing the script cache");
    ScriptEngine::FlushCache();

    // wait for things to catch up
    Sleep(500);

    if (!Vars.bUseProfileScript) {
        const wchar_t* script = GetStarterScriptName();
        if (StartScript(script, GetStarterScriptState()))
            Print("ÿc2D2BSÿc0 :: Started %s", script);
        else
            Print("ÿc2D2BSÿc0 :: Failed to start %s", script);
    }
}

bool ProcessCommand(const wchar_t* command, bool unprocessedIsCommand) {
    bool result = false;
    wchar_t* buf = _wcsdup(command);
    wchar_t* next_token1 = NULL;
    wchar_t* argv = wcstok_s(buf, L" ", &next_token1);

    // no command?
    if (argv == NULL)
        return false;

    if (_wcsicmp(argv, L"start") == 0) {
        const wchar_t* script = GetStarterScriptName();
        if (StartScript(script, GetStarterScriptState()))
            Print("ÿc2D2BSÿc0 :: Started %s", script);
        else
            Print("ÿc2D2BSÿc0 :: Failed to start %s", script);
        result = true;
    } else if (_wcsicmp(argv, L"stop") == 0) {
        if (ScriptEngine::GetCount() > 0)
            Print("ÿc2D2BSÿc0 :: Stopping all scripts");
        ScriptEngine::StopAll();
        result = true;
    } else if (_wcsicmp(argv, L"flush") == 0) {
        if (Vars.bDisableCache != TRUE)
            Print("ÿc2D2BSÿc0 :: Flushing the script cache");
        ScriptEngine::FlushCache();
        result = true;
    } else if (_wcsicmp(argv, L"load") == 0) {
        const wchar_t* script = command + 5;
        if (StartScript(script, GetStarterScriptState()))
            Print("ÿc2D2BSÿc0 :: Started %ls", script);
        else
            Print("ÿc2D2BSÿc0 :: Failed to start %ls", script);
        result = true;
    } else if (_wcsicmp(argv, L"reload") == 0) {
        Reload();
        result = true;
    }
#if DEBUG
    else if (_strcmpi(argv, "crash") == 0) {
        DWORD zero = 0;
        double value = 1 / zero;
        Print("%d", value);
    } else if (_strcmpi(argv, "profile") == 0) {
        const char* profile = command + 8;
        if (SwitchToProfile(profile))
            Print("ÿc2D2BSÿc0 :: Switched to profile %s", profile);
        else
            Print("ÿc2D2BSÿc0 :: Profile %s not found", profile);
        result = true;
    }
#endif
    else if (_wcsicmp(argv, L"exec") == 0 && !unprocessedIsCommand) {
        char* utf8Cmd = UnicodeToAnsi(command + 5);
        ExecCommand(utf8Cmd);
        delete[] utf8Cmd;
        result = true;
    } else if (unprocessedIsCommand) {
        char* utf8Cmd = UnicodeToAnsi(command);
        ExecCommand(utf8Cmd);
        delete[] utf8Cmd;
        result = true;
    }
    free(buf);
    return result;
}

void GameJoined(void) {
    if (!Vars.bUseProfileScript) {
        const wchar_t* starter = GetStarterScriptName();
        if (starter != NULL) {
            Print("ÿc2D2BSÿc0 :: Starting %ls", starter);
            if (StartScript(starter, GetStarterScriptState()))
                Print("ÿc2D2BSÿc0 :: %ls running.", starter);
            else
                Print("ÿc2D2BSÿc0 :: Failed to start %ls!", starter);
        }
    }
}

void MenuEntered(bool beginStarter) {
    if (beginStarter && !Vars.bUseProfileScript) {
        const wchar_t* starter = GetStarterScriptName();
        char* starterN = UnicodeToAnsi(starter);
        if (starter != NULL) {
            Print("ÿc2D2BSÿc0 :: Starting %s", starterN);
            if (StartScript(starter, GetStarterScriptState()))
                Print("ÿc2D2BSÿc0 :: %s running.", starterN);
            else
                Print("ÿc2D2BSÿc0 :: Failed to start %s!", starterN);
        }
        delete[] starterN;
    }
}

SYMBOL_INFO* GetSymFromAddr(HANDLE hProcess, DWORD64 addr) {
    char* symbols = new char[sizeof(SYMBOL_INFO) + 512];
    memset(symbols, 0, sizeof(SYMBOL_INFO) + 512);

    SYMBOL_INFO* sym = (SYMBOL_INFO*)(symbols);
    sym->SizeOfStruct = sizeof(SYMBOL_INFO);
    sym->MaxNameLen = 512;

    DWORD64 dummy;
    bool success = SymFromAddr(hProcess, addr, &dummy, sym) == TRUE ? true : false;
    if (!success) {
        delete[] symbols;
        sym = NULL;
    }

    return sym;
}

IMAGEHLP_LINE64* GetLineFromAddr(HANDLE hProcess, DWORD64 addr) {
    IMAGEHLP_LINE64* line = new IMAGEHLP_LINE64;
    line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    DWORD dummy;
    bool success = SymGetLineFromAddr64(hProcess, addr, &dummy, line) == TRUE ? true : false;
    if (!success) {
        delete line;
        line = NULL;
    }
    return line;
}

char* DllLoadAddrStrs() {
    const char* dlls[] = {"D2Client.DLL", "D2Common.DLL", "D2Gfx.DLL",    "D2Lang.DLL", "D2Win.DLL", "D2Net.DLL",  "D2Game.DLL",
                          "D2Launch.DLL", "Fog.DLL",      "BNClient.DLL", "Storm.DLL",  "D2Cmp.DLL", "D2Multi.DLL"};
    size_t strMaxLen;
    char* result;
    char lineBuf[80];
    unsigned int i;

    strMaxLen = sizeof(lineBuf) * sizeof(dlls) / sizeof(dlls[0]);
    result = (char*)malloc(strMaxLen);

    result[0] = '\0';

    for (i = 0; i < sizeof(dlls) / sizeof(dlls[0]); ++i) {
        sprintf_s(lineBuf, sizeof(lineBuf), "%s loaded at: 0x%08x.", dlls[i], GetModuleHandle(dlls[i]));
        strcat_s(result, strMaxLen, lineBuf);
        if (i != (sizeof(dlls) / sizeof(dlls[0]) - 1)) {
            strcat_s(result, strMaxLen, "\n");
        }
    }

    return result;
}

LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS* ptrs) {
    std::string debug;
    GetStackWalk(debug);
    Log("Stack Walk: %s", debug.c_str());

    EXCEPTION_RECORD* rec = ptrs->ExceptionRecord;
    CONTEXT* ctx = ptrs->ContextRecord;
    DWORD base = Vars.pModule ? Vars.pModule->dwBaseAddress : (DWORD)Vars.hModule;

    char path[MAX_PATH + _MAX_FNAME] = "";
    sprintf_s(path, sizeof(path), "%s\\D2BS.bin", Vars.szPath);

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, GetCurrentProcessId());
    HANDLE hThread = GetCurrentThread();
    CONTEXT context = *ctx;

    unsigned int i;
    int len;
    char* szString;
    char* dllAddrs;

    SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_FAIL_CRITICAL_ERRORS | SYMOPT_NO_PROMPTS | SYMOPT_DEFERRED_LOADS);
    SymInitializeW(hProcess, Vars.szPath, TRUE);
    SymLoadModule64(hProcess, NULL, path, NULL, base, 0);

    STACKFRAME64 stack;
    stack.AddrPC.Offset = context.Eip;
    stack.AddrPC.Mode = AddrModeFlat;
    stack.AddrStack.Offset = context.Esp;
    stack.AddrStack.Mode = AddrModeFlat;
    stack.AddrFrame.Offset = context.Ebp;
    stack.AddrFrame.Mode = AddrModeFlat;

    std::string trace;

    for (i = 0; i < 64; i++) {
        if (!StackWalk64(IMAGE_FILE_MACHINE_I386, hProcess, hThread, &stack, &context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
            break;

        // infinite loop
        if (stack.AddrReturn.Offset == stack.AddrPC.Offset)
            break;
        // jump to 0
        if (stack.AddrPC.Offset == 0)
            break;

        SYMBOL_INFO* sym = GetSymFromAddr(hProcess, stack.AddrPC.Offset);

        if (sym) {
            char msg[1024];
            ULONG64 base2 = (sym->Address - sym->ModBase);

            IMAGEHLP_LINE64* line = GetLineFromAddr(hProcess, stack.AddrPC.Offset);
            if (line)
                sprintf_s(msg, 1024, "\t%s+0x%08x, File: %s line %d\n", sym->Name, base2, strrchr(line->FileName, '\\') + 1, line->LineNumber);
            else
                sprintf_s(msg, 1024, "\t%s+0x%08x\n", sym->Name, base2);

            trace.append(msg);
            delete line;
        } else {
            char addr[100];
            sprintf_s(addr, sizeof(addr), "\t0x%08x\n", stack.AddrFrame.Offset);
            trace.append(addr);
        }

        delete[](char*) sym;
    }

    SYMBOL_INFO* sym = GetSymFromAddr(hProcess, (DWORD64)rec->ExceptionAddress);

    IMAGEHLP_LINE64* line = GetLineFromAddr(hProcess, (DWORD64)rec->ExceptionAddress);

    len = _scprintf("EXCEPTION!\n*** 0x%08x at 0x%08x (%s in %s line %d)\n"
                    "D2BS loaded at: 0x%08x\n"
                    "Registers:\n"
                    "\tEIP: 0x%08x, ESP: 0x%08x\n"
                    "\tCS: 0x%04x, DS: 0x%04x, ES: 0x%04x, SS: 0x%04x, FS: 0x%04x, GS: 0x%04x\n"
                    "\tEAX: 0x%08x, EBX: 0x%08x, ECX: 0x%08x, EDX: 0x%08x, ESI: 0x%08x, EDI: 0x%08x, EBP: 0x%08x, FLG: 0x%08x\n"
                    "Stack Trace:\n%s\nEnd of stack trace.",
                    rec->ExceptionCode, rec->ExceptionAddress, sym != NULL ? sym->Name : "Unknown", line != NULL ? strrchr(line->FileName, '\\') + 1 : "Unknown",
                    line != NULL ? line->LineNumber : 0, base, ctx->Eip, ctx->Esp, ctx->SegCs, ctx->SegDs, ctx->SegEs, ctx->SegSs, ctx->SegFs, ctx->SegGs, ctx->Eax,
                    ctx->Ebx, ctx->Ecx, ctx->Edx, ctx->Esi, ctx->Edi, ctx->Ebp, ctx->EFlags, trace.c_str());
    dllAddrs = DllLoadAddrStrs();

    szString = new char[len + 1];
    sprintf_s(szString, len + 1,
              "EXCEPTION!\n*** 0x%08x at 0x%08x (%s in %s line %d)\n"
              "D2BS loaded at: 0x%08x\n"
              "Registers:\n"
              "\tEIP: 0x%08x, ESP: 0x%08x\n"
              "\tCS: 0x%04x, DS: 0x%04x, ES: 0x%04x, SS: 0x%04x, FS: 0x%04x, GS: 0x%04x\n"
              "\tEAX: 0x%08x, EBX: 0x%08x, ECX: 0x%08x, EDX: 0x%08x, ESI: 0x%08x, EDI: 0x%08x, EBP: 0x%08x, FLG: 0x%08x\n"
              "Stack Trace:\n%s\nEnd of stack trace.",
              rec->ExceptionCode, rec->ExceptionAddress, sym != NULL ? sym->Name : "Unknown", line != NULL ? strrchr(line->FileName, '\\') + 1 : "Unknown",
              line != NULL ? line->LineNumber : 0, base, ctx->Eip, ctx->Esp, ctx->SegCs, ctx->SegDs, ctx->SegEs, ctx->SegSs, ctx->SegFs, ctx->SegGs, ctx->Eax, ctx->Ebx,
              ctx->Ecx, ctx->Edx, ctx->Esi, ctx->Edi, ctx->Ebp, ctx->EFlags, trace.c_str());

    Log("%s\n%s", szString, dllAddrs);

    free(dllAddrs);

    delete[] szString;
    delete[](char*) sym;
    delete line;

    SymCleanup(hProcess);

    return EXCEPTION_EXECUTE_HANDLER;
}

int __cdecl _purecall(void)
{
    std::string debug;
    GetStackWalk(debug);
    Log("Stack Walk: %s", debug.c_str());
    return 0;
}

bool GetStackWalk(std::string &outWalk)
{
    // Set up the symbol options so that we can gather information from the current
    // executable's PDB files, as well as the Microsoft symbol servers.  We also want
    // to undecorate the symbol names we're returned.  If you want, you can add other
    // symbol servers or paths via a semi-colon separated list in SymInitialized.
    ::SymSetOptions( SYMOPT_DEFERRED_LOADS | SYMOPT_INCLUDE_32BIT_MODULES | SYMOPT_UNDNAME );
    if (!::SymInitialize( ::GetCurrentProcess(), "http://msdl.microsoft.com/download/symbols", TRUE )) return false;
 
    // Capture up to 25 stack frames from the current call stack.  We're going to
    // skip the first stack frame returned because that's the GetStackWalk function
    // itself, which we don't care about.
    PVOID addrs[ 25 ] = { 0 };
    USHORT frames = CaptureStackBackTrace( 1, 25, addrs, NULL );
 
    for (USHORT i = 0; i < frames; i++) {
        // Allocate a buffer large enough to hold the symbol information on the stack and get 
        // a pointer to the buffer.  We also have to set the size of the symbol structure itself
        // and the number of bytes reserved for the name.
        ULONG64 buffer[ (sizeof( SYMBOL_INFO ) + 1024 + sizeof( ULONG64 ) - 1) / sizeof( ULONG64 ) ] = { 0 };
        SYMBOL_INFO *info = (SYMBOL_INFO *)buffer;
        info->SizeOfStruct = sizeof( SYMBOL_INFO );
        info->MaxNameLen = 1024;
 
        // Attempt to get information about the symbol and add it to our output parameter.
        DWORD64 displacement = 0;
        if (::SymFromAddr( ::GetCurrentProcess(), (DWORD64)addrs[ i ], &displacement, info )) {
            outWalk.append( info->Name, info->NameLen );
            outWalk.append( "\n" );
        }
    }
 
        ::SymCleanup( ::GetCurrentProcess() );
 
    return true;
}

