/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

*/

#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>
#include <time.h>
#include "D2Helpers.h"
#include "D2BS.h"
#include "Offset.h"
#include "File.h"
#include "CommandLine.h"
#include "ArrayEx.h"
#define ArraySize(x) (sizeof((x)) / sizeof((x)[0]))

CArrayEx<sLine*, sLine*> aCommand;

// Commands.
sLine CLine[] = {{L"-d2c", 0}, {L"-d2x", 0}, {L"-title", 0}, {L"-mpq", 0}, {L"-profile", 0}, {L"-handle", 0}, {L"-multi", 0}, {L"-sleepy", 0}, {L"-cachefix", 0}};

DWORD ParseStringForText(LPWSTR Source, LPWSTR text) {
    WCHAR BUF[4059];
    memset(BUF, 0x00, 4059);

    for (unsigned int x = 0; x < wcslen(Source); x++) {
        if (wcslen(text) + x > wcslen(Source))
            break;

        for (unsigned int y = 0; y < wcslen(text); y++) {
            INT cC = Source[x + y];
            memcpy(BUF + wcslen(BUF), (LPWSTR)&cC, sizeof(cC));
        }
        if (!_wcsicmp(BUF, text))
            return x;

        memset(BUF, 0x00, 4059);
    }
    return -1;
}

VOID CopyCommandLine(LPWSTR Command) {
	//Log(L"Command original: %s", Command);
	memcpy(Vars.szConsoleCopy, Command, sizeof(wchar_t) * wcslen(Command));
	//Log(L"Command copy: %s", Vars.szConsoleCopy);
	Vars.bConsoleCopied = true;
	LPWSTR szCheat = L"C:\\Program Files (x86)\\Diablo II\\Game.exe -w";
	memcpy(Command, szCheat, sizeof(typeid(szCheat)) * wcslen(szCheat));
	//Log(L"Command overwriten: %s", Command);
}

VOID ParseCommandLine(LPWSTR Command) {
    for (int x = 0; x < ArraySize(CLine); x++) {
        DWORD id = ParseStringForText(Command, CLine[x].Param);
        if (id == -1)
            continue;

        WCHAR szText[200];
        BOOL bStart = false;

        memset(szText, 0x00, 200);

        if (!CLine[x].isBool) {
            for (unsigned int y = (id + (wcslen(CLine[x].Param))); y < wcslen(Command); y++) {
                if (Command[y] == L'"')
                    if (bStart) {
                        bStart = false;
                        break;
                    } else {
                        bStart = true;
                        y++;
                    }

                int byt = Command[y];

                if (bStart)
                    memcpy(szText + wcslen(szText), (LPWSTR)&byt, sizeof(byt));
            }
        }
        sLine* sl = new sLine;
        sl->isBool = CLine[x].isBool;
        wcscpy_s(sl->Param, _countof(sl->Param), CLine[x].Param);
        if (!sl->isBool)
            wcscpy_s(sl->szText, _countof(sl->szText), szText);

        aCommand.Add(sl);
    }
}

sLine* GetCommand(LPWSTR Param) {
    for (int x = 0; x < aCommand.GetSize(); x++)
        if (!_wcsicmp(aCommand[x]->Param, Param))
            return aCommand[x];

    return 0;
}