// This file is for debugging purposes only.
// You must #define _MSVC_DEBUG in order to enable it.
// It enables loading from D2Loader as a plugin,
// making debugging easier.

#ifdef _MSVC_DEBUG

#include <windows.h>

typedef DWORD ( __stdcall * PluginEntryFunc)(DWORD dwReason, LPVOID lpData);
DWORD __stdcall PluginEntry(DWORD dwReason, LPVOID lpData) { return TRUE; }
typedef struct { DWORD dwMagicword; DWORD dwVersion; LPCSTR szDescription; PluginEntryFunc fpEntry; } PLUGIN_INTERFACE, * LPPLUGIN_INTERFACE;
PLUGIN_INTERFACE Interface = {0x44320000,0x01000912,"D2BS",PluginEntry};

#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllexport) LPPLUGIN_INTERFACE __cdecl QueryInterface(void) { return &Interface; }
#ifdef __cplusplus
}
#endif

#endif