#ifndef __D2BS_H__
#define __D2BS_H__

#pragma once

#define XP_WIN

#define D2BS_VERSION "1.5.1732"

#include <windows.h>
#include <vector>

#include "ScreenHook.h"

struct Variables;

#define ArraySize(x) (sizeof((x)) / sizeof((x)[0]))

#define PRIVATE_UNIT		1
#define PRIVATE_ITEM		3

struct Private { DWORD dwPrivateType; };

struct Module
{	
	union {
		HMODULE hModule;
		DWORD dwBaseAddress;
	};
	DWORD _1;
	char szPath[MAX_PATH];
};

struct Variables
{
	int		nChickenHP;
	int		nChickenMP;
	DWORD	dwInjectTime;
	DWORD	dwGameTime;
	BOOL	bDontCatchNextMsg;
	BOOL	bClickAction;
	BOOL	bNeedShutdown;
	BOOL	bUseGamePrint;
	BOOL	bShutdownFromDllMain;
	BOOL	bChangedAct;
	BOOL	bGameLoopEntered;

	DWORD	dwMaxGameTime;
	DWORD	dwGameTimeout;
	BOOL	bQuitOnError;
	BOOL	bQuitOnHostile;
	BOOL	bStartAtMenu;
	BOOL	bActive;
	BOOL	bBlockKeys;
	BOOL	bBlockMouse;
	BOOL	bDisableCache;
	BOOL	bUseProfileScript;
	BOOL	bLoadedWithCGuard;
	BOOL	bLogConsole;
	BOOL	bEnableUnsupported;
	BOOL	bUseRawCDKey;
	int		dwMemUsage;

	Module* pModule;
	HMODULE hModule;

	char	szPath[_MAX_PATH];
	char	szScriptPath[_MAX_PATH];
	char	szProfile[256];
	char	szStarter[_MAX_FNAME];
	char	szDefault[_MAX_FNAME];
	char	szClassic[30];
	char	szLod[30];

	WNDPROC oldWNDPROC;
	HHOOK hMouseHook;
	HHOOK hKeybHook;

	UINT_PTR uTimer;
	long SectionCount;

	std::map<unsigned __int32, CellFile*> mCachedCellFiles;
	std::vector<std::pair<DWORD, DWORD> > vUnitList;
	std::list<Event*> EventList;
	CRITICAL_SECTION	cEventSection;
	CRITICAL_SECTION	cRoomSection;
	CRITICAL_SECTION	cMiscSection;
	CRITICAL_SECTION	cScreenhookSection;
	CRITICAL_SECTION	cPrintSection;
	CRITICAL_SECTION	cTextHookSection;
	CRITICAL_SECTION	cImageHookSection;
	CRITICAL_SECTION	cBoxHookSection;
	CRITICAL_SECTION	cFrameHookSection;
	CRITICAL_SECTION	cLineHookSection;
	CRITICAL_SECTION	cFlushCacheSection;
	CRITICAL_SECTION	cConsoleSection;
	CRITICAL_SECTION	cGameLoopSection;
	CRITICAL_SECTION	cUnitListSection;

	DWORD dwSelectedUnitId;
	DWORD dwSelectedUnitType;
	POINT pMouseCoords;
};

extern Variables Vars;

BOOL Startup(void);
void Shutdown(void);

#endif
