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

#ifndef _COMMANDLINE_H
#define _COMMANDLINE_H

#include <ArrayEx.h>


struct sLine{
	CHAR Param[200];
	BOOL isBool;
	CHAR szText[300];
};

void ParseCommandLine(LPSTR Command);
sLine *GetCommand(LPSTR Param);

extern CArrayEx<sLine*, sLine*> aCommand;

#endif