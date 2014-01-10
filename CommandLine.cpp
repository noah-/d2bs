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
#include "Offset.h"
#include "File.h"
#include "CommandLine.h"
#include "ArrayEx.h"
#define ArraySize(x) (sizeof((x)) / sizeof((x)[0]))

CArrayEx<sLine*, sLine*> aCommand;

//Commands.
sLine CLine[] = {
	{"-c0",0},
	{"-c1",0},
	{"-c2",0},
	{"-e0",0},
	{"-e1",0},
	{"-e2",0},
	{"-mpq",0},
	{"-profile", 0},
	{"-handle", 0}
};

DWORD ParseStringForText(LPSTR Source,LPSTR text)
{
	CHAR BUF[4059];
	memset(BUF,0x00,4059);

	for(unsigned int x = 0; x < strlen(Source); x++)
	{
		if(strlen(text) + x > strlen(Source))
			break;

		for(unsigned int y = 0; y < strlen(text); y++)
		{
			INT cC = Source[x+y];
			memcpy(BUF+strlen(BUF),(LPSTR)&cC,sizeof(cC));
		}
		if(!_strcmpi(BUF,text))
			return x;

		memset(BUF,0x00,4059);
	}
	return -1;
}


VOID ParseCommandLine(LPSTR Command)
{
	for(int x = 0; x < ArraySize(CLine); x++)
	{
		DWORD id = ParseStringForText(Command,CLine[x].Param);
		if(id == -1)
			continue;

		CHAR szText[100];
		BOOL bStart = false;

		memset(szText,0x00,100);

		if(!CLine[x].isBool)
		{
			for(unsigned int y = (id+(strlen(CLine[x].Param))); y < strlen(Command); y++)
			{
				if(Command[y] == '"')
					if(bStart){
						bStart = false;
						break;
					}
					else{
						bStart = true;
						y++;
					}

					int byt = Command[y];

				if(bStart)
					memcpy(szText+strlen(szText),(LPSTR)&byt,sizeof(byt));
			}
		}
		sLine *sl = new sLine;
		sl->isBool = CLine[x].isBool;
		strcpy_s(sl->Param,sizeof(sl->Param),CLine[x].Param);
		if(!sl->isBool)
			strcpy_s(sl->szText,sizeof(sl->szText),szText);

		aCommand.Add(sl);
	}
}

sLine *GetCommand(LPSTR Param)
{
	for(int x = 0; x < aCommand.GetSize(); x++)
		if(!_strcmpi(aCommand[x]->Param,Param))
			return aCommand[x];

	return 0;
}